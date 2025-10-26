#include "abstractconnectionline.hpp"

#include <QPainterPath>

#include <math.h>

namespace ObjectItems {

AbstractConnectionLine::AbstractConnectionLine(QGraphicsItem* parent) :
    BasicItem(parent)
{
    setSystemName("Connection line");
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

    emit graphicalDataChanged();
}

QSizeF AbstractConnectionLine::getArrowSize() const
{
    return m_arrowSize;
}

void AbstractConnectionLine::setLine(const QLineF& line) {
    m_straightLine = line;
    emit graphicalDataChanged();
}

void AbstractConnectionLine::setLine(const QPointF& p1, const QPointF& p2) {
    setLine(QLineF(p1, p2));
}

QLineF AbstractConnectionLine::getLine() const {
    return m_straightLine;
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

void AbstractConnectionLine::setStyle(Qt::PenStyle pst)
{
    m_stylePen.setStyle(pst);
    emit graphicalDataChanged();
}

void AbstractConnectionLine::setWidth(double w)
{
    m_stylePen.setWidth(w);
    emit graphicalDataChanged();
}

QPainterPath AbstractConnectionLine::createArrowPath() const
{
    QPainterPath p;
    p.lineTo(-m_arrowSize.width(), 0);
    p.lineTo(0, -m_arrowSize.height());
    p.lineTo(m_arrowSize.width(), 0);
    p.lineTo(0, 0);
    return p;
}


} // namespace ObjectItems
