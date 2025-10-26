#include "elegantconnectionline.hpp"

#include <math.h>

#include <Components/Logger/Logger.h>

namespace ObjectItems {

ElegantConnectionLine::ElegantConnectionLine(QGraphicsItem *parent) :
    AbstractConnectionLine(parent)
{
    setSystemName("Elegant connection line");

    createSubitem(m_line);
    m_line->setZValue(1);
    createSubitem(m_lineSelected);
    m_lineSelected->hide();

    createSubitem(m_forwardArrow);
    m_forwardArrow->setZValue(2);
    m_forwardArrow->setBrush(getLineColor());
    m_forwardArrow->setPen(getStylePen());
    m_forwardArrow->setRotation(180);

    createSubitem(m_backwardArrow);
    m_backwardArrow->setZValue(2);
    m_backwardArrow->setBrush(getLineColor());
    m_backwardArrow->setPen(getStylePen());

    connect(this, &BasicItem::itemSelected,
            this, [this](){
        m_lineSelected->show();
        m_forwardArrow->setPen(m_lineSelected->pen());
        m_backwardArrow->setPen(m_lineSelected->pen());
    });

    connect(this, &BasicItem::itemDeselected,
            this, [this](){
        m_lineSelected->hide();
        m_forwardArrow->setPen(getStylePen());
        m_backwardArrow->setPen(getStylePen());
    });

    connect(this, &BasicItem::graphicalDataChanged,
            this, [this](){
        auto linePen = getStylePen();

        m_line->setPath(createLinePath());
        m_line->setPen(linePen);
        m_line->setBrush(getLineColor());

        m_lineSelected->setPath(m_line->path());
        linePen.setWidth(linePen.width() + 2);
        linePen.setColor(getSelectionColor());
        m_lineSelected->setPen(linePen);

        auto lineDirection = getDirection();
        if (lineDirection & LineDirectionType::Forward) {
            // Arrow
            m_forwardArrow->setPath(createArrowPath());
            m_forwardArrow->setTransformOriginPoint(m_forwardArrow->boundingRect().center());

            // Position
            m_forwardArrow->setPos(getPositionTo() - m_forwardArrow->boundingRect().center());

            // Drawing
            m_forwardArrow->show();
        } else {
            m_forwardArrow->hide();
        }

        if (lineDirection & LineDirectionType::Backward) {
            // Arrow
            m_backwardArrow->setPath(createArrowPath());
            m_backwardArrow->setTransformOriginPoint(m_backwardArrow->boundingRect().center());

            // Position
            m_backwardArrow->setPos(getPositionFrom() - m_backwardArrow->boundingRect().center());

            // Drawing
            m_backwardArrow->show();
        } else {
            m_backwardArrow->hide();
        }
    });
}

QPainterPath ElegantConnectionLine::createLinePath() const
{
    auto arrowHeight = getArrowSize().height();

    auto pointFrom = getPositionFrom();
    pointFrom.setY(pointFrom.y() + arrowHeight);

    auto pointTo = getPositionTo();
    pointTo.setY(pointTo.y() - arrowHeight);

    auto centerPoint = getLine().center();
    auto referPoint1 = centerPoint;
    referPoint1.setY(pointFrom.y());

    auto referPoint2 = centerPoint;
    referPoint2.setY(pointTo.y());

    QPainterPath p;
    p.moveTo(getPositionFrom());
    p.lineTo(pointFrom);
    p.cubicTo(pointFrom, referPoint1, centerPoint);
    p.cubicTo(centerPoint, referPoint2, pointTo);
    p.lineTo(pointTo);
    p.lineTo(getPositionTo());

    // Для правильной области определения
    QPainterPathStroker stroker;
    stroker.setWidth(getStylePen().widthF());
    stroker.setCapStyle(Qt::RoundCap);
    stroker.setJoinStyle(Qt::RoundJoin);

    return stroker.createStroke(p);
}

} // namespace ObjectItems
