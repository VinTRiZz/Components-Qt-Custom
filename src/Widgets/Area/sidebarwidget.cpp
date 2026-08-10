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
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);

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
                }
                m_widgetHideState = m_pWidget->isHidden() ? HS_Hidden : HS_Shown;
                emit sig_toggled();
            });

    setToggleCallback([](auto* pButton, auto nextState){
        if (nextState & HS_Hidden) {
            pButton->setText("SHOW");
        } else if (nextState & HS_Shown) {
            pButton->setText("HIDE");
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

void SidebarWidget::setShowDirection(Direction sdir)
{
    m_showDirection = sdir;
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
        // TODO: Implement
        curpos.setX(150);
        curpos.setY(150);
    }

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
}

void SidebarWidget::setAnimationStep(int step)
{
    const double currentStep = step;

    QRect widgetRect;

    // Size changing
    bool isVertical = m_showDirection & (Direction::Top | Direction::Bottom);
    bool isFromBottom = (m_showDirection & Direction::Top);
    bool isHorizontal = m_showDirection & (Direction::Right | Direction::Left);
    bool isFromRight = (m_showDirection & Direction::Left);

    if (isVertical) {
        int targetH = std::floor(m_widgetTargetHeight * 0.01 * currentStep);
        widgetRect.setHeight((parentWidget()->height() - targetH) * isFromBottom + targetH * !isFromBottom);
    } else {
        widgetRect.setHeight(parentWidget()->height());
    }

    if (isHorizontal) {
        int targetW = std::floor(m_widgetTargetWidth * 0.01 * currentStep);
        widgetRect.setWidth((parentWidget()->width() - targetW) * isFromRight + targetW * !isFromRight);
    } else {
        widgetRect.setWidth(parentWidget()->width());
    }

    // Required to be displayed
    setGeometry(widgetRect);

    // Margins
    widgetRect.moveTo(widgetRect.x() + 1, widgetRect.y() + 1);
    widgetRect.setWidth(widgetRect.width() > 2 ? widgetRect.width() - 2 : 0);
    widgetRect.setHeight(widgetRect.height() > 2 ? widgetRect.height() - 2 : 0);

    m_pWidget->setGeometry(widgetRect);
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