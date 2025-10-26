#include "basiciteminterface.hpp"

namespace ObjectItems {

objectId_t BasicItemInterface::createSystemId() {
    static objectId_t currentId{-1};
    return --currentId;
}

bool BasicItemInterface::isSystemObject() const { return m_id < 0; }

void BasicItemInterface::setItemId(objectId_t id) { m_id = id; processIdChange(); }

void BasicItemInterface::setSystemId() { setItemId(createSystemId()); }

objectId_t BasicItemInterface::getItemId() const { return m_id; }

QString BasicItemInterface::getSystemName() const { return m_systemName; }

void BasicItemInterface::setDisplayName(const QString &text) { m_displayName = text; processInternalDataChange(); }

QString BasicItemInterface::getDisplayName() const { return m_displayName; }

void BasicItemInterface::setDescription(const QString &text) { m_description = text; processInternalDataChange(); }

QString BasicItemInterface::getDescription() const { return m_description; }

void BasicItemInterface::setLineColor(const QColor &color) { m_lineColor = color; processColorChange(); }

QColor BasicItemInterface::getLineColor() const { return m_lineColor; }

void BasicItemInterface::setBackgroundColor(const QColor &color) { m_backgroundColor = color; processColorChange(); }

QColor BasicItemInterface::getBackgroundColor() const { return m_backgroundColor; }

void BasicItemInterface::setSelectionColor(const QColor &color) { m_selectionColor = color; processColorChange(); }

QColor BasicItemInterface::getSelectionColor() const { return m_selectionColor; }

void BasicItemInterface::setSystemName(const QString &iText) { m_systemName = iText; }

}
