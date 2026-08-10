#include "sidebarwidget.hpp"

#include <QEventLoop>
#include <QVariantAnimation>

#include <math.h>

#include <Components/Logger/Logger.h>

namespace QtCustom::Widgets {

namespace {
// I want uint, but Qt does not work with it in QVariant
constexpr int   ANIMATION_ZERO_DURATION {500};
constexpr int   ANIMATION_STEP_COUNT    {100};
}

SidebarWidget::SidebarWidget(QWidget *parent)
    : QWidget{parent}
{
    // Work with source transparency for better displaying
    // setWindowFlags(Qt::FramelessWindowHint);
    // setAttribute(Qt::WA_TranslucentBackground, true);
    setStyleSheet("background-color: red");

    m_pCurrentAnimation = new QVariantAnimation(this);
    connect(m_pCurrentAnimation, &QVariantAnimation::valueChanged, this,
            [this](auto varCurrentValue) {
                setAnimationStep(varCurrentValue.toInt());
            });
    connect(m_pCurrentAnimation, &QVariantAnimation::finished,
            m_pCurrentAnimation, [this](){
                if (!m_pWidget) { return; }
                if (m_widgetHideState & HS_Hidden) {
                    m_pWidget->hide();
                    setGeometry({});
                }
                m_widgetHideState = m_pWidget->isHidden() ? HS_Hidden : HS_Shown;
                emit sig_toggled();
            });

    setToggleCallback([](auto* pButton, auto nextState){
        if (nextState & HS_Hidden) {
            pButton->setText(">>>");
        } else if (nextState & HS_Shown) {
            pButton->setText("<<<");
        } else {
            pButton->setText("?");
        }
    });
    auto pDefaultButton = new QPushButton;
    pDefaultButton->setGeometry(0, 0, 50, 30);
    setButton(pDefaultButton);
}

void SidebarWidget::setParentWidget(QWidget *pWidget)
{
    stopWidgetAnimations();
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
    stopWidgetAnimations();
    m_pWidget = pWidget;
    if (!m_pWidget) {
        return;
    }
    m_pWidget->setParent(this);
    m_widgetHideState = m_pWidget->isHidden() ? HS_Hidden : HS_Shown;
    if (m_buttonToggleCallback) {
        m_buttonToggleCallback(m_pButton, m_widgetHideState);
    }
    updateVisualState();
}

void SidebarWidget::setWidgetWidth(int wwidth)
{
    m_widgetTargetWidth = wwidth;
    updateVisualState();
}

void SidebarWidget::setWidgetHeight(int wheight)
{
    m_widgetTargetHeight = wheight;
    updateVisualState();
}

void SidebarWidget::setShowTowardsDirection(Direction sdir)
{
    m_showTowardsDirection = sdir;
    updateVisualState();
}

void SidebarWidget::setButtonPosition(const QPoint& offsetP)
{
    m_buttonOffset = offsetP;
    slot_updateButtonPosition();
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

void SidebarWidget::slot_toggle()
{
    if (!m_pWidget) { return; }

    pollWidgetAnimation();
    emit sig_aboutToToggle();
    auto nextState = m_widgetHideState & HS_Shown ? HS_Hidden : HS_Shown;
    if (m_buttonToggleCallback) {
        m_buttonToggleCallback(m_pButton, nextState);
    }
    if (nextState == HS_Shown) {
        startShowAnimation(m_widgetHideSpeed);
    } else if (nextState == HS_Hidden) {
        startHideAnimation(m_widgetShowSpeed);
    }
}

void SidebarWidget::slot_updateButtonPosition()
{
    auto buttonRect = m_pButton->rect();
    buttonRect.moveTo(calculateCurrentButtonPosition());
    m_pButton->setGeometry(buttonRect);
}

double SidebarWidget::calculateAnimationDuration(AnimationSpeed spd) const
{
    double res {ANIMATION_ZERO_DURATION};
    switch (spd)
    {
    case AnimationSpeed::NoAnimation:
        res = 0;
        break;
    case AnimationSpeed::UltraFast:
        res *= 0.1;
        break;
    case AnimationSpeed::Fast:
        res *= 0.25;
        break;
    case AnimationSpeed::Medium:
        res *= 0.5;
        break;
    case AnimationSpeed::Slow:
        res *= 0.75;
        break;
    case AnimationSpeed::UltraSlow:
        res *= 1.1;
        break;
    }
    return res;
}

QPoint SidebarWidget::calculateCurrentButtonPosition() const
{
    auto curpos = m_buttonOffset;

    if (!parentWidget() || !m_pButton || !m_pWidget) { return curpos; }

    // curpos.setX( (curpos.x() + m_pWidget->width()) * bool(m_showDirection & Direction::Right) +
    //             (parentWidget()->width() - m_pWidget->width() - m_pButton->width() - curpos.x()) * bool(m_showDirection & Direction::Left)
    // );
    // curpos.setY(curpos.y() + m_pWidget->height() - m_pButton->height());

    // COMPLOG_DEBUG("CURPOS:", curpos.x(), curpos.y());

    return curpos;
}

void SidebarWidget::pollWidgetAnimation()
{
    // No animation pending
    if ((m_widgetHideState ^ ~HS_InProcess) && (QVariantAnimation::State::Running != m_pCurrentAnimation->state())) {
        return;
    }

    // Wait for animation to complete
    QEventLoop loop;
    connect(m_pCurrentAnimation, &QVariantAnimation::finished,
            &loop, &QEventLoop::quit);
    loop.exec();
}

void SidebarWidget::stopWidgetAnimations()
{
    if (m_widgetHideState ^ ~HS_InProcess) {
        return;
    }
    m_pCurrentAnimation->stop();
    m_widgetHideState = m_widgetHideState & HS_Hidden ? HS_Hidden : HS_Shown;
}

void SidebarWidget::updateVisualState()
{
    setAnimationStep(ANIMATION_STEP_COUNT);
    if (m_widgetHideState & HS_Hidden) {
        setGeometry({});
    }
}

void SidebarWidget::setAnimationStep(int step)
{
    const double currentStep = step;

    QRect widgetRect;

    // Size changing
    bool isVertical = m_showTowardsDirection & (Direction::Top | Direction::Bottom);
    bool isToTop = (m_showTowardsDirection & Direction::Top);
    bool isHorizontal = m_showTowardsDirection & (Direction::Right | Direction::Left);
    bool isToLeft = (m_showTowardsDirection & Direction::Left);

    if (isVertical) {
        int targetH = std::floor(m_widgetTargetHeight * 0.01 * currentStep);
        widgetRect.setTop((m_widgetTargetHeight - targetH) * isToTop);
        widgetRect.setBottom(targetH * !isToTop + m_widgetTargetHeight * isToTop);
    } else {
        widgetRect.setHeight(parentWidget()->height());
    }

    if (isHorizontal) {
        int targetW = std::floor(m_widgetTargetWidth * 0.01 * currentStep);
        widgetRect.setLeft((m_widgetTargetWidth - targetW) * isToLeft);
        widgetRect.setRight(targetW * !isToLeft + m_widgetTargetWidth * isToLeft);
    } else {
        widgetRect.setWidth(parentWidget()->width());
    }
    m_pWidget->setGeometry(widgetRect);

    widgetRect.moveTo(isToLeft ? parentWidget()->width() - widgetRect.width() : 0,
                      isToTop ? parentWidget()->height() - widgetRect.height() : 0);
    setGeometry(widgetRect);

    slot_updateButtonPosition();
}

void SidebarWidget::startShowAnimation(AnimationSpeed asp)
{
    m_widgetHideState = HS_InProcess_show;
    m_pWidget->show();
    m_pCurrentAnimation->setStartValue(0);
    m_pCurrentAnimation->setEndValue(ANIMATION_STEP_COUNT);
    m_pCurrentAnimation->setDuration(calculateAnimationDuration(asp));
    m_pCurrentAnimation->start();
}

void SidebarWidget::startHideAnimation(AnimationSpeed asp)
{
    m_widgetHideState = HS_InProcess_hide;
    m_pCurrentAnimation->setStartValue(ANIMATION_STEP_COUNT);
    m_pCurrentAnimation->setEndValue(0);
    m_pCurrentAnimation->setDuration(calculateAnimationDuration(asp));
    m_pCurrentAnimation->start();
}

}