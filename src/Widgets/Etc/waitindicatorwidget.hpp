#pragma once

#include <QWidget>

#include <memory>

namespace QtCustom::Widgets {

class WaitIndicatorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WaitIndicatorWidget(QWidget *parent = nullptr);
    ~WaitIndicatorWidget();

    /**
     * @brief setTarget Set widget to work with. Must be child of this widget's pWidget widget
     * @param pWidget
     */
    void setTarget(QWidget* pWidget);
    QWidget* getTarget() const;

    /**
     * @brief setIndicatorRect Set fixed size of a indicator. By default, size is 60x60
     * @param indRect
     */
    void setIndicatorRect(const QSize& indSize);
    QSize getIndicatorRect() const;

    /**
     * @brief setDisablingParent On start, disables parent widget
     * @param isDisabling
     */
    void setDisablingParent(bool isDisabling);
    bool isDisablingParent() const;

    // Basic interaction
    void start();
    void pauseIndicator();
    void continueIndicator();
    void stop();

    /**
     * @brief The Status enum Describes status of an indicator
     */
    enum Status : uint8_t {
        InProcess   = 0b00001, // Used to determine whether status is not final

        Ready       = 0b00010,              // Indicator is hidden and ready to work
        Working     = 0b00100,              // Displaying wait status (for example, rotating circle)
        Starting    = InProcess | Working,  // Playing indicator show animation
        Paused      = 0b01000,              // Indication is paused, causing cycle animation with no percent displaying
        Pausing     = InProcess | Paused,   // Playing pause animation
        Stopping    = InProcess | Ready,    // Playing indicator hide animation
    };
    Status getStatus() const;

    /**
     * @brief pollAnimation Used to wait while current status is not final
     */
    void pollAnimation() const;

    /**
     * @brief setAnimationEnabled Toggle all animations of a widget
     * @param isEn
     */
    void setAnimationEnabled(bool isEn);
    bool isAnimationEnabled() const;

    /**
     * @brief setPercent Set indicator percent
     * @param perc
     */
    void setPercent(double perc);
    double getCurrentPercent() const;

    /**
     * @brief setPercentVisible Enable or disable percent displaying
     * @param isPercentVisible
     */
    void setPercentVisible(bool isPercentVisible);
    bool isPercentVisible() const;

    /**
     * @brief setTitle Set title to display accented in widget
     * @param text
     */
    void setTitle(const QString& text);
    void setTitleSymbolLimit(int maxSymbols);
    QString getTitle() const;

    /**
     * @brief setDescription Set description to display as regular text in widget
     * @param text
     */
    void setDescription(const QString& text);
    void setDescriptionSymbolLimit(int maxSymbols);
    QString getDescription() const;

    /**
     * @brief The Shape enum Describes shape of indicator
     */
    enum class Shape : uint8_t {
        Circle = 0,
        Line,
    };

    /**
     * @brief setShape Set indicator type to display
     * @param itype
     */
    void setShape(Shape itype);
    Shape getShape() const;

    /**
     * @brief setTextPen Define pen to draw text
     * @param textPen
     */
    void setTextPen(const QPen& textPen);
    QPen getTextPen() const;

    /**
     * @brief setShapePenPrimary Define main pen, used to draw main shapes (such as circle parts)
     * @param primaryPen
     */
    void setShapePenPrimary(const QPen& primaryPen);
    QPen getShapePenPrimary() const;

    /**
     * @brief setShapePenSecondary Define secondary pen, used to draw extra parts of a shape
     * @param secondaryPen
     */
    void setShapePenSecondary(const QPen& secondaryPen);
    QPen getShapePenSecondary() const;

private:
    struct Impl;
    std::unique_ptr<Impl> d;

    // Widget logic updates
    void  updateVisualState();

    // Painting
    QRect createWorkingRect() const; // TODO: Think about naming
    QRect getWorkingRect() const;
    void  paintTitle() const;
    void  paintDescription() const;
    void  paintPercent() const;
    void  paintCircleIndicator() const;

    // Utility
    double utilityPieFromDegree(const double degree) const;

private slots:
    void slot_switchState(const QVariant& animationValue);
    void slot_finishSwitchChange();

    void slot_updateSecondary(const QVariant& animationValue);
    void slot_finishSecondary();

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent *event) override;
};

}