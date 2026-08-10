#pragma once

#include <QWidget>
#include <QPushButton>

class SidebarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SidebarWidget(QWidget *parent = nullptr);

    enum HideState : uint8_t
    {
        HS_Invalid, // For invalid widget set cases
        HS_Hidden,
        HS_Hiding,
        HS_Showing,
        HS_Shown,
    };

    QPushButton* getButton() const;

    void setWidget(QWidget* pWidget);
    QWidget* getWidget() const;

    void setShowDuration(uint timeMs);
    void setHideDuration(uint timeMs);

    bool isHovered() const;
    void setHovered(bool isHover);

    bool isWidgetHidden() const;

signals:
    void sig_aboutToToggle();
    void sig_toggled();

public slots:
    void slot_toggle();

private:
    QPushButton* m_pButton  {nullptr};
    QWidget* m_pWidget      {nullptr};

    // Dynamic state marks
    bool m_isWidgetHidden   {true};

    // Viewing configuration
    uint m_animationDuration    {0};
    bool m_isHoveredMode        {false};
};
