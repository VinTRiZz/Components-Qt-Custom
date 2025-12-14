#include "abstractconnectionline.hpp"

#include <QPainterPath>

#include <math.h>

namespace ObjectItems {

AbstractConnectionLine::AbstractConnectionLine(QGraphicsItem* parent) :
    BasicItem(parent)
{
    setSystemName("Connection");
    setObjectType(ObjectType::OIT_AbstractConnectionLine);

    setFlag(QGraphicsItem::ItemClipsChildrenToShape, false);
}

void AbstractConnectionLine::subscribeForMoves(BasicItem *pItem, bool isFrom, const QPointF &offsetPos)
{
    connect(pItem, &BasicItem::itemMovedOnScene,
            this, [this, pItem, offsetPos, isFrom](){
        auto targetPos = mapFromScene(pItem->mapToScene(pItem->pos())) + offsetPos;
        if (isFrom) {
            setPositionFrom(targetPos);
        } else {
            setPositionTo(targetPos);
        }
    });
}

void AbstractConnectionLine::unsubscribeForMoves(BasicItem *pItem)
{
    disconnect(pItem, &BasicItem::itemMovedOnScene, this, nullptr);
}

void AbstractConnectionLine::setDirection(LineDirectionType arrType)
{
    m_arrowType = arrType;
    emit graphicalDataChanged();
}

LineDirectionType AbstractConnectionLine::getDirection() const
{
    return m_arrowType;
}

void AbstractConnectionLine::setArrowHeight(double arHeight)
{
    double arrowAngle {0};
    switch (m_arrowAngle)
    {
    case LineAngleType::A_90: arrowAngle = M_PI / 2.0; break;
    case LineAngleType::A_75: arrowAngle = M_PI / 2.4; break;
    case LineAngleType::A_60: arrowAngle = M_PI / 3.0; break;
    case LineAngleType::A_45: arrowAngle = M_PI / 4.0; break;
    case LineAngleType::A_30: arrowAngle = M_PI / 6.0; break;
    case LineAngleType::A_15: arrowAngle = M_PI / 12.0; break;
    }

    m_arrowSize.setHeight(arHeight);
    m_arrowSize.setWidth(arHeight / 2.0 * std::sin(arrowAngle));

    m_isArrowSizeChanged = true;
    emit graphicalDataChanged();
}

QSizeF AbstractConnectionLine::getArrowHeight() const
{
    return m_arrowSize;
}

void AbstractConnectionLine::setArrowAngle(LineAngleType lineAngle)
{
    m_arrowAngle = lineAngle;
    setArrowHeight(getArrowHeight().height());
}

LineAngleType AbstractConnectionLine::getArrowAngle() const
{
    return m_arrowAngle;
}

QLineF AbstractConnectionLine::getLine() const
{
    return m_straightLine;
}

void AbstractConnectionLine::setLine(const QLineF &line)
{
    m_straightLine = line;
    emit graphicalDataChanged();
}

void AbstractConnectionLine::setLine(const QPointF &p1, const QPointF &p2)
{
    setLine({p1, p2});
}

void AbstractConnectionLine::setPositionFrom(const QPointF &posFrom)
{
    setLine(posFrom, m_straightLine.p2());
}

QPointF AbstractConnectionLine::getPositionFrom() const
{
    return m_straightLine.p1();
}

void AbstractConnectionLine::setPositionTo(const QPointF &posTo)
{
    setLine(m_straightLine.p1(), posTo);
}

QPointF AbstractConnectionLine::getPositionTo() const
{
    return m_straightLine.p2();
}

QPainterPath AbstractConnectionLine::createArrowPath() const
{
    if (!m_isArrowSizeChanged) {
        return m_cachedArrowpath;
    }
    QPainterPath p;
    p.lineTo(-m_arrowSize.width(), 0);
    p.lineTo(0, -m_arrowSize.height());
    p.lineTo(m_arrowSize.width(), 0);
    p.lineTo(0, 0);
    m_cachedArrowpath = p;
    m_isArrowSizeChanged = false;
    return p;
}


} // namespace ObjectItems
