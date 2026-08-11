#pragma once

#include <QObject>

#include "waitindicatorconfigurations.hpp"

class QPainter;
class QVariantAnimation;

namespace QtCustom::Widgets {

/**
 * @brief The WaitIndicatorDrawHelper class Base for widget draw helpers
 */
class WaitIndicatorDrawHelper : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

    static WaitIndicatorDrawHelper* create(const IndicatorConfigurationBasePtr& cfg, QObject* parent);

    void setConfiguration(const IndicatorConfigurationBasePtr& pConfig);

    template <typename ConfigT = IndicatorConfigurationBase>
    std::shared_ptr<ConfigT> getConfig() const {
        return std::dynamic_pointer_cast<ConfigT>(m_pConfig);
    }

    virtual void init() = 0;
    virtual void paint(QPainter* pPainter,
                       const QRect& targetWidgetRect,
                       double currentPercent,
                       uint8_t indicatorStatus) = 0;

    virtual void startAnimation() = 0;
    virtual void pauseAnimation() = 0;
    virtual void continueAnimation() = 0;
    virtual void stopAnimation() = 0;
    virtual void pollAnimation() = 0;

private:
    IndicatorConfigurationBasePtr m_pConfig;
};

template <>
inline IndicatorConfigurationBasePtr WaitIndicatorDrawHelper::getConfig() const {
    return m_pConfig;
}



/**
 * @brief The CircleDrawHelper class Standard loading circle
 */
class CircleDrawHelper : public WaitIndicatorDrawHelper
{
public:
    using WaitIndicatorDrawHelper::WaitIndicatorDrawHelper;

    // WaitIndicatorDrawHelper interface
    void init() override;
    virtual void paint(QPainter* pPainter,
                       const QRect &targetWidgetRect,
                       double currentPercent,
                       uint8_t indicatorStatus) override;

    void startAnimation() override;
    void pauseAnimation() override;
    void continueAnimation() override;
    void stopAnimation() override;
    void pollAnimation() override;

private:
    static constexpr auto CIRCLE_RECT_OFFSET {5};
    QRect createCircleRect() const;

    QVariantAnimation* m_pPrimaryAnimation      {nullptr};  // Main action (for example, percent change)
    QVariantAnimation* m_pSecondaryAnimation    {nullptr};  // Passive actions (for example, particles behind widget)

    QRect getCircleArea(QRect targetWidgetRect) const;

    void paintTitle(QPainter *pPainter, const QRect& targetWidgetRect) const;
    void paintDescription(QPainter* pPainter, const QRect& targetWidgetRect) const;
    void paintPercent(QPainter* pPainter, const QRect& targetWidgetRect, uint8_t indicatorStatus, double currentPercent) const;

    void paintIndicatorCircle(QPainter* pPainter, double currentPercent) const;
};

}