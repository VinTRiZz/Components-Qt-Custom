#pragma once

#include <QWidget>
#include <QPushButton>

class QVariantAnimation;

namespace QtCustom::Widgets {

/**
 * @brief The SidebarWidget class Widget to show / hide other widgets as a side panel
 * @note Set parent of this widget to use. Do not add into layout to avoid artefacts
 */
class SidebarWidget : public QWidget
{
    Q_OBJECT
    using QWidget::setParent; // Can break internal logic if allowed to use
public:
    explicit SidebarWidget(QWidget *parent = nullptr);

    // Overrides parent method because require extra logic
    void setParentWidget(QWidget* pWidget);

    /**
     * @brief setWidget Set widget inside bar
     * @param pWidget
     */
    void setWidget(QWidget* pWidget);
    QWidget* getWidget() const;

    /**
     * @brief setWidgetWidth Set width of a widget to be set after showing
     * @param wwidth
     * @note works only when direction is right / left
     */
    void setWidgetWidth(int wwidth);
    int getWidgetWidth() const;

    /**
     * @brief setWidgetHeight Set height of a widget to be set after showing
     * @param wheight
     * @note works only when direction is top / bottom
     */
    void setWidgetHeight(int wheight);
    int getWidgetHeight() const;

    /**
     * @brief The Direction enum Used to work with directions in widget
     */
    enum Direction : uint8_t
    {
        Top         = 0b0001,
        Left        = 0b0010,
        Right       = 0b0001,
        Bottom      = 0b0010,

        LeftTop     = Top       | Left,
        LeftBottom  = Bottom    | Left,
        RightTop    = Top       | Right,
        RightBottom = Bottom    | Right,
    };

    /**
     * @brief setShowDirection Set direction for a widget to be shown
     * @param sdir
     * @note Changes logic of using widget's target height and width
     */
    void setShowDirection(Direction sdir);
    Direction getShowDirection() const;

    /**
     * @brief setButtonPosition Set position offset of a button referred to a parent widget
     * @param offsetP
     */
    void setButtonPosition(const QPoint &offsetP);
    QPoint getButtonPosition() const;

    enum HideState : uint8_t
    {
        HS_Invalid          = 0, // For invalid widget set cases
        HS_Hidden           = 0b001,
        HS_Shown            = 0b010,
        HS_InProcess        = 0b100,
        HS_InProcess_hide   = HS_InProcess | HS_Hidden,
        HS_InProcess_show   = HS_InProcess | HS_Shown
    };
    HideState getWidgetState() const;

    /**
     * @brief The AnimationSpeed enum For simple configuring
     * @note Must not be used with widget that can change size
     */
    enum class AnimationSpeed : uint8_t
    {
        UltraFast = 0,
        Fast,
        Medium,
        Slow,
        UltraSlow,
    };

    // Widget show / hide animations. Work only for HIDE --> SHOW and SHOW --> HIDE states
    void setAnimationSpeed(AnimationSpeed asp, HideState prevState, HideState nextState);
    AnimationSpeed getShowmode(HideState prevState, HideState nextState) const;

    // Used to customize button
    void setButton(QPushButton* pButton);
    using toggleCallback_t = std::function<void(QPushButton*, HideState)>; // Button to toggle and state to toggle in
    void setToggleCallback(toggleCallback_t&& cbk);
    QPushButton* getButton() const;

signals:
    void sig_aboutToToggle();
    void sig_toggled();

public slots:
    /**
     * @brief slot_toggle Toggles widget to be hidden / shown. Called by this widget button
     * @note If widget not set, does nothing (even not call button toggle callback)
     */
    void slot_toggle();

    /**
     * @brief slot_updateButtonPosition Updates button position
     * @note    Must be called from resizeEvent() of parentwidget
     *          Important slot for widgets, allowed to change their size
     */
    void slot_updateButtonPosition();

private:
    // Main functionality
    QPushButton*m_pButton   {nullptr};
    QWidget*    m_pWidget   {nullptr};

    // Dynamic state processing
    int                 m_widgetTargetWidth     {300};
    int                 m_widgetTargetHeight    {300};
    QVariantAnimation*  m_pCurrentAnimation     {nullptr};
    HideState           m_widgetHideState {HS_Invalid};
    AnimationSpeed      m_widgetHideSpeed {AnimationSpeed::Medium};
    AnimationSpeed      m_widgetShowSpeed {AnimationSpeed::Medium};
    toggleCallback_t    m_buttonToggleCallback;

    // Viewing configuration
    Direction   m_showDirection {Direction::Right};
    QPoint      m_buttonOffset  {};

    // Utility
    double calculateAnimationDuration(AnimationSpeed spd) const;
    QPoint calculateCurrentButtonPosition() const;

    // Widget animation methods
    void widget_stopAnimations();
    void widget_startShow(AnimationSpeed asp);
    void widget_stopShow();
    void widget_startHide(AnimationSpeed asp);
    void widget_stopHide();
};

}