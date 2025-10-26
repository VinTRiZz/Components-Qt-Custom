#include "lineitem.hpp"

namespace ObjectItems {

LineItem::LineItem(QGraphicsItem *parent) :
    ItemBase(parent)
{
    setSystemName("Линия");
}

void LineItem::setLine(const QLineF& line) {
    m_straightLine = line;
}

void LineItem::setLine(const QPointF& p1, const QPointF& p2) {
    setLine(QLineF(p1, p2));
}

QLineF LineItem::getLine() const {
    return m_straightLine;
}

void LineItem::setPositionFrom(const QPointF &posFrom)
{
    setLine(posFrom, m_straightLine.p2());
}

void LineItem::setPositionTo(const QPointF &posTo)
{
    setLine(m_straightLine.p1(), posTo);
}

}
