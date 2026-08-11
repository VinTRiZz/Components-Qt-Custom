#pragma once

#include <memory>

#include <QPen>
#include <QString>

namespace QtCustom::Widgets {

/**
 * @brief The IndicatorConfigurationBase class Config with basical settings of any indicator
 */
class IndicatorConfigurationBase
{
public:
    IndicatorConfigurationBase() = default;
    virtual ~IndicatorConfigurationBase() = default;

    QSize m_size {60, 60}; // Size of area to draw indicator in it

    bool    m_isDisablingParent {true};
    uint    m_titleTextSymbolLimit          {25};
    uint    m_descriptionTextSymbolLimit    {150};

    QPen m_textPen       {QPen(QColor(85, 220, 190), 1, Qt::SolidLine, Qt::RoundCap)};

    void setTitle(const QString& text);
    QString getTitle() const { return m_titleText; }

    void setDescription(const QString& text);
    QString getDescription() const { return m_descriptionText; }

private:
    QString m_titleText                     {"Please, be patient..."};
    QString m_descriptionText               {"Operation is in process."};
};
using IndicatorConfigurationBasePtr = std::shared_ptr<IndicatorConfigurationBase>;


/**
 * @brief The IndicatorCircleConfiguration class Standard loading circle configuration
 */
class IndicatorCircleConfiguration : public IndicatorConfigurationBase
{
public:
    bool    m_isPercentVisible  {true};
    bool    m_isAnimationEnabled {false};

    QPen m_primaryPen    {QPen(QColor(45, 210, 170), 4, Qt::SolidLine, Qt::RoundCap)};
    QPen m_secondaryPen  {QPen(QColor(65, 120, 110), 2, Qt::SolidLine, Qt::RoundCap)};
};

}