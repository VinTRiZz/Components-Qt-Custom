#include "waitindicatorconfigurations.hpp"

void QtCustom::Widgets::IndicatorConfigurationBase::setTitle(const QString &text)
{
    if (text.size() > m_titleTextSymbolLimit) {
        m_titleText = text.left(m_titleTextSymbolLimit);
        return;
    }
    m_titleText = text;
}

void QtCustom::Widgets::IndicatorConfigurationBase::setDescription(const QString &text)
{
    if (text.size() > m_descriptionTextSymbolLimit) {
        m_descriptionText = text.left(m_descriptionTextSymbolLimit);
        return;
    }
    m_descriptionText = text;
}
