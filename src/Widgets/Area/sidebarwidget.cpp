#include "sidebarwidget.hpp"

#include <QCoreApplication>
#include <QVariantAnimation>

#include <math.h>

#include <Components/Logger/Logger.h>

namespace QtCustom::Widgets {

static const uint ANIMATION_ZERO_DURATION {500};

SidebarWidget::SidebarWidget(QWidget *parent)
    : QWidget{parent}
{
    m_pCurrentAnimation = new QVariantAnimation(this);
    connect(m_pCurrentAnimation, &QVariantAnimation::valueChanged, this,
            [this](auto varCurrentValue) {
                if (!m_pWidget || !parentWidget()) {
                    widget_stopAnimations();
                    return;
                }
                double currentStep = varCurrentValue.toInt();
                QRect widgetRect;

                // Size changing
                if (m_showDirection & (Direction::Top | Direction::Bottom)) {
                    widgetRect.setHeight(int(std::floor(m_widgetTargetHeight * 0.01 * currentStep)));
                } else {
                    widgetRect.setHeight(parentWidget()->height());
                }
                if (m_showDirection & (Direction::Right | Direction::Left)) {
                    widgetRect.setWidth(int(std::floor(m_widgetTargetWidth * 0.01 * currentStep)));
                } else {
                    widgetRect.setWidth(parentWidget()->width());
                }

                if (m_showDirection & Direction::Right) {
                    widgetRect.moveTo(
                        parentWidget()->width() - widgetRect.width(),
                        widgetRect.y()
                        );
                }
                if (m_showDirection & Direction::Top) {
                    widgetRect.moveTo(
                        widgetRect.x(),
                        parentWidget()->height() - widgetRect.height());
                }

                // Margins
                widgetRect.moveTo(widgetRect.x() + 1, widgetRect.y() + 1);
                widgetRect.setWidth(widgetRect.width() > 2 ? widgetRect.width() - 2 : 0);
                widgetRect.setHeight(widgetRect.height() > 2 ? widgetRect.height() - 2 : 0);

                m_pWidget->setGeometry(widgetRect);
                COMPLOG_DEBUG("Widget geometry changed:",
                              widgetRect.x(),
                              widgetRect.y(),
                              widgetRect.width(),
                              widgetRect.height());
                COMPLOG_DEBUG("Parent geometry:",
                              parentWidget()->width(),
                              parentWidget()->height());
                slot_updateButtonPosition();
            });
    connect(m_pCurrentAnimation, &QVariantAnimation::finished,
            m_pCurrentAnimation, [this](){
                if (!m_pWidget) { return; }
                if (m_widgetHideState & HS_Hidden) {
                    m_pWidget->hide();
                }
                m_widgetHideState = m_pWidget->isHidden() ? HS_Hidden : HS_Shown;
                emit sig_toggled();
            });

    setToggleCallback([](auto* pButton, auto nextState){
        if (nextState == HS_Hidden) {
            pButton->setText("=");
        } else if (nextState == HS_Shown) {
            pButton->setText("-");
        } else {
            pButton->setText("?");
        }
    });
    auto pDefaultButton = new QPushButton;
    pDefaultButton->setGeometry(0, 0, 30, 30);
    pDefaultButton->setText("=");
    setButton(pDefaultButton);
    slot_updateButtonPosition();
}

void SidebarWidget::setParentWidget(QWidget *pWidget)
{
    if (m_pButton) {
        m_pButton->setParent(pWidget);
    }

    if (m_pWidget) {
        m_pWidget->setParent(pWidget);
    }
    QWidget::setParent(pWidget);
}

void SidebarWidget::setWidget(QWidget *pWidget)
{
    while (qApp && (m_widgetHideState & HS_InProcess)) { // Otherwise will cause artefacts
        qApp->processEvents(QEventLoop::AllEvents);
    }
    m_pWidget = pWidget;
    if (!m_pWidget) {
        return;
    }
    m_pWidget->setParent(this);
    widget_stopAnimations();
    m_widgetHideState = m_pWidget->isHidden() ? HS_Hidden : HS_Shown;
    slot_updateButtonPosition();
}

void SidebarWidget::setWidgetWidth(int wwidth)
{
    m_widgetTargetWidth = wwidth;
}

int SidebarWidget::getWidgetWidth() const
{
    return m_widgetTargetWidth;
}

void SidebarWidget::setWidgetHeight(int wheight)
{
    m_widgetTargetHeight = wheight;
}

int SidebarWidget::getWidgetHeight() const
{
    return m_widgetTargetHeight;
}

void SidebarWidget::setShowDirection(Direction sdir)
{
    m_showDirection = sdir;
}

SidebarWidget::Direction SidebarWidget::getShowDirection() const
{
    return m_showDirection;
}

QWidget *SidebarWidget::getWidget() const
{
    return m_pWidget;
}

void SidebarWidget::setButtonPosition(const QPoint& offsetP)
{
    m_buttonOffset = offsetP;
    slot_updateButtonPosition();
}

QPoint SidebarWidget::getButtonPosition() const
{
    return m_buttonOffset;
}

SidebarWidget::HideState SidebarWidget::getWidgetState() const
{
    return m_widgetHideState;
}

void SidebarWidget::setAnimationSpeed(AnimationSpeed asp, HideState prevState, HideState nextState)
{
    if (prevState == HS_Hidden && nextState == HS_Shown) {
        m_widgetShowSpeed = asp;
    } else if (prevState == HS_Hidden && nextState == HS_Shown) {
        m_widgetHideSpeed = asp;
    }
}

void SidebarWidget::setButton(QPushButton *pButton)
{
    if (m_pButton) {
        disconnect(m_pButton, &QPushButton::clicked,
                   this, &SidebarWidget::slot_toggle);
    }
    m_pButton = pButton;
    if (!m_pButton) { return; }
    m_pButton->setParent(parentWidget());
    connect(m_pButton, &QPushButton::clicked,
               this, &SidebarWidget::slot_toggle);
    slot_updateButtonPosition();
}

void SidebarWidget::setToggleCallback(SidebarWidget::toggleCallback_t &&cbk)
{
    m_buttonToggleCallback = std::move(cbk);
}

QPushButton *SidebarWidget::getButton() const
{
    return m_pButton;
}

SidebarWidget::AnimationSpeed SidebarWidget::getShowmode(HideState prevState, HideState nextState) const
{
    if (prevState == HS_Hidden && nextState == HS_Shown) {
        return m_widgetShowSpeed;
    } else if (prevState == HS_Hidden && nextState == HS_Shown) {
        return m_widgetHideSpeed;
    }
    return AnimationSpeed::Medium;
}

void SidebarWidget::slot_toggle()
{
    if (!m_pWidget) { return; }

    emit sig_aboutToToggle();
    auto nextState = m_widgetHideState & HS_Shown ? HS_Hidden : HS_Shown;
    if (m_buttonToggleCallback) {
        m_buttonToggleCallback(m_pButton, nextState);
    }
    if (nextState == HS_Shown) {
        widget_startShow(m_widgetHideSpeed);
    } else if (nextState == HS_Hidden) {
        widget_startHide(m_widgetShowSpeed);
    }
}

void SidebarWidget::slot_updateButtonPosition()
{
    auto buttonRect = m_pButton->rect();
    buttonRect.moveTo(calculateCurrentButtonPosition());
    m_pButton->setGeometry(buttonRect);

    COMPLOG_DEBUG("Button geo change:",
                  buttonRect.x(),
                  buttonRect.y(),
                  buttonRect.width(),
                  buttonRect.height());
}

double SidebarWidget::calculateAnimationDuration(AnimationSpeed spd) const
{
    double res {ANIMATION_ZERO_DURATION};
    switch (spd)
    {
    case AnimationSpeed::UltraFast:
        res *= 0.01;
        break;
    case AnimationSpeed::Fast:
        res *= 0.1;
        break;
    case AnimationSpeed::Medium:
        break;
    case AnimationSpeed::Slow:
        res *= 1.2;
        break;
    case AnimationSpeed::UltraSlow:
        res *= 1.6;
        break;
    }
    return res;
}

QPoint SidebarWidget::calculateCurrentButtonPosition() const
{
    auto curpos = m_buttonOffset;

    if (!parentWidget() || !m_pButton || !m_pWidget) { return curpos; }

    // From bottom to top
    if (m_showDirection & Direction::Top) {
        curpos.setY(std::max(parentWidget()->height() - m_pButton->height() - 1, 0));
    }

    // From right to left
    if (m_showDirection & Direction::Left) {
        curpos.setX(std::max(parentWidget()->width() - m_pButton->width() - 1, 0));
    }

    // From left to right or from top to bottom
    if (m_showDirection & (Direction::Right | Direction::Bottom)) {

    }

    return curpos;
}

void SidebarWidget::widget_stopAnimations()
{
    if (m_widgetHideState & HS_InProcess) {
        if (m_widgetHideState & HS_Hidden) {
            widget_stopHide();
        } else {
            widget_stopShow();
        }
        slot_updateButtonPosition();
    }
}

void SidebarWidget::widget_startShow(AnimationSpeed asp)
{
    widget_stopAnimations();

    m_widgetHideState = HS_InProcess_show;
    m_pWidget->show();
    m_pCurrentAnimation->setStartValue(0);
    m_pCurrentAnimation->setEndValue(100);
    m_pCurrentAnimation->setDuration(calculateAnimationDuration(asp));
    m_pCurrentAnimation->start();
}

void SidebarWidget::widget_stopShow()
{
    m_pCurrentAnimation->stop();
    m_widgetHideState = HS_Shown;
}

void SidebarWidget::widget_startHide(AnimationSpeed asp)
{
    widget_stopAnimations();

    m_widgetHideState = HS_InProcess_hide;
    m_pCurrentAnimation->setStartValue(100);
    m_pCurrentAnimation->setEndValue(0);
    m_pCurrentAnimation->setDuration(calculateAnimationDuration(asp));
    m_pCurrentAnimation->start();
}

void SidebarWidget::widget_stopHide()
{
    m_pCurrentAnimation->stop();
    m_widgetHideState = HS_Hidden;
}

}