#include "elegantarrowline.hpp"

#include <math.h>

#include <QGraphicsSceneMouseEvent>
#include <QJsonObject>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionGraphicsItem>

namespace ObjectItems {

ElegantArrowLine::ElegantArrowLine(QGraphicsItem *parent) :
    LineItem(parent)
{
    setSystemName("Соединение (изящное)");

    setType(ObjectItems::OBJECTTYPE_ARROWLINE);

    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemClipsToShape, true);

    m_line = new QGraphicsPathItem(this);
    registerSubitem(m_line);
    m_line->setBrush(Qt::NoBrush);
    m_line->setZValue(1);

    m_lineSelected = new QGraphicsPathItem(this);
    registerSubitem(m_lineSelected);
    m_lineSelected->setBrush(Qt::NoBrush);
    m_lineSelected->setZValue(0);
    m_lineSelected->hide();

    m_pArrowHeadPolygon = new QGraphicsPolygonItem(this);
    registerSubitem(m_pArrowHeadPolygon);

    m_penGradient.setColorAt(0.0, Qt::black);
    m_penGradient.setColorAt(1.0, Qt::lightGray);
    m_penGradient.setCoordinateMode(QLinearGradient::ObjectMode);
    m_line->setBrush(m_penGradient);
    m_line->setPen(QPen(Qt::transparent));
    m_pArrowHeadPolygon->setPen(QPen(Qt::transparent));

    ElegantArrowLine::setWeight(1);
}


void ElegantArrowLine::setGradient1Color(const QColor &penColor)
{
    m_penGradient.setColorAt(0.0, penColor);
    m_line->setBrush(m_penGradient);
    m_pArrowHeadPolygon->setBrush(m_penGradient);

    if (isSelected()) {
        m_pArrowHeadPolygon->setPen(
            QPen(getSelectionColor(), 8, Qt::SolidLine, Qt::RoundCap));
    } else {
        m_pArrowHeadPolygon->setPen(QPen(Qt::transparent));
    }
}

void ElegantArrowLine::setGradient2Color(const QColor &penColor)
{
    m_penGradient.setColorAt(1.0, penColor);
    m_line->setBrush(m_penGradient);
    m_pArrowHeadPolygon->setBrush(m_penGradient);

    if (isSelected()) {
        m_pArrowHeadPolygon->setPen(
            QPen(getSelectionColor(), 8, Qt::SolidLine, Qt::RoundCap));
    } else {
        m_pArrowHeadPolygon->setPen(QPen(Qt::transparent));
    }
}

void ElegantArrowLine::setSelectionColor(const QColor& penColor) {
    ItemBase::setSelectionColor(penColor);

    m_lineSelected->setPen(QPen(penColor, 8, Qt::SolidLine, Qt::RoundCap));

    if (isSelected()) {
        m_pArrowHeadPolygon->setPen(
            QPen(penColor, 8, Qt::SolidLine, Qt::RoundCap));
    } else {
        m_pArrowHeadPolygon->setPen(QPen(Qt::transparent));
    }
}

void ElegantArrowLine::setArrowSize(qreal size) {
    m_arrowSize = size;
}

qreal ElegantArrowLine::getArrowSize() const {
    return m_arrowSize;
}

void ElegantArrowLine::updatePolygon() {
    m_boundingRect = {};
    m_line->setPath(createLinePath());
    m_lineSelected->setPath(m_line->path());

    auto straightLine = getLine();

    bool isP1Lefter = straightLine.x2() > straightLine.x1();
    bool isP1Higher = straightLine.y2() > straightLine.y1();

    m_boundingRect.moveTop(
        (isP1Higher ? straightLine.y1() : straightLine.y2()) - m_arrowSize);
    m_boundingRect.moveLeft(
        (isP1Lefter ? straightLine.x1() : straightLine.x2()) - m_arrowSize);
    m_boundingRect.setWidth(
        std::fabs(straightLine.x2() - straightLine.x1()) + m_arrowSize * 2);
    m_boundingRect.setHeight(
        std::fabs(straightLine.y2() - straightLine.y1()) + m_arrowSize * 2);
}

QPainterPath ElegantArrowLine::createLinePath() {
    auto straightLine = getLine();

    auto pointFrom = straightLine.p1();
    pointFrom.setY(pointFrom.y() + m_arrowSize);

    auto pointTo = straightLine.p2();
    pointTo.setX(pointTo.x() +
                 (pointFrom.x() > pointTo.x() ? m_arrowSize : -m_arrowSize));
    pointTo.setY(pointTo.y() - m_arrowSize);

    QPainterPath p;
    p.moveTo(straightLine.p1());
    p.lineTo(pointFrom);

    auto firstControlPoint = straightLine.center();
    firstControlPoint.setY(pointFrom.y());

    auto secondControlPoint = straightLine.center();
    secondControlPoint.setY(pointTo.y());

    p.cubicTo(pointFrom, firstControlPoint, straightLine.center());
    p.cubicTo(straightLine.center(), secondControlPoint, pointTo);

    auto arrowLine = QLineF(pointTo, straightLine.p2());
    if (arrowLine.length() != 0) {
        // Угол линии
        double angle = (arrowLine.angle() + 180) * M_PI / 180.0;

        const auto PI_DELIM_3 = M_PI / 3;
        const auto PI_2_DELIM_3 = M_PI * 2 / 3;

        QPointF arrowP1 = arrowLine.p2();

        QPointF arrowP2 =
            arrowLine.p2() + QPointF(sin(angle + PI_DELIM_3) * m_arrowSize,
                                     cos(angle + PI_DELIM_3) * m_arrowSize);

        QPointF arrowP3 =
            arrowLine.p2() + QPointF(sin(angle + PI_2_DELIM_3) * m_arrowSize,
                                     cos(angle + PI_2_DELIM_3) * m_arrowSize);

        p.lineTo(straightLine.p2());
        p.lineTo(arrowP2);
        p.lineTo(arrowP3);
        p.lineTo(arrowP1);
    }

    QPainterPathStroker stroker;
    stroker.setWidth(6.0);
    stroker.setCapStyle(Qt::RoundCap);
    stroker.setJoinStyle(Qt::RoundJoin);

    // Возвращаем "обведенную" форму
    return stroker.createStroke(p);
}

QPolygonF ElegantArrowLine::createPolygon(const QLineF& line) {
    QRectF rect;
    QTransform transf;

    auto leftX = line.x1() > line.x2() ? line.x1() : line.x2();
    auto bottomY = line.y1() > line.y2() ? line.y2() : line.y1();

    rect.setLeft(leftX);
    rect.setBottom(bottomY);

    rect.setHeight(line.length());
    rect.setWidth(5);

    rect.moveTo(line.center().x() - leftX, line.center().y() - bottomY);

    transf.rotate(line.angle());

    return transf.mapRect(rect);
}

void ElegantArrowLine::updatePen() {
    if (isSelected() == m_prevSelectedState) {
        return;
    }
    m_prevSelectedState = isSelected();

    auto currentPen = isSelected() ? getSelectionColor() : Qt::transparent;
    m_pArrowHeadPolygon->setPen(currentPen);
}

QVariant ElegantArrowLine::itemChange(GraphicsItemChange change,
                                          const QVariant& value) {
    if (change == ItemSelectedChange) {
        if (value.toBool()) {
            m_lineSelected->show();
        } else {
            m_lineSelected->hide();
        }
    }

    return ItemBase::itemChange(change, value);
}

QRectF ElegantArrowLine::boundingRect() const {
    return m_boundingRect;
}

bool ElegantArrowLine::contains(const QPointF& p) const {
    return m_pArrowHeadPolygon->contains(p) || m_line->contains(p);
}

QPainterPath ElegantArrowLine::shape() const {
    QPainterPath res;
    res.addPath(m_line->shape());
    res.addPath(m_pArrowHeadPolygon->shape());
    return res;
}

void ElegantArrowLine::setLine(const QLineF &line) {
    LineItem::setLine(line);
    m_penGradient.setStart(getLine().p1());
    m_penGradient.setFinalStop(getLine().p2());
    updatePolygon();
}

void ElegantArrowLine::setWeight(double w) {
    m_weight = w;
    auto linePen = m_line->pen();
    linePen.setWidth(m_weight + 1);
    m_line->setPen(linePen);

    auto lineSelPen = m_lineSelected->pen();
    lineSelPen.setWidth(linePen.width() + 4);
    m_lineSelected->setPen(lineSelPen);
}

double ElegantArrowLine::getWeight() const
{
    return m_weight;
}


void ElegantArrowLine::setPositionFrom(const QPointF& posFrom) {
    LineItem::setPositionFrom(posFrom);
    m_penGradient.setStart(getLine().p1());
    updatePolygon();
}

void ElegantArrowLine::setPositionTo(const QPointF& posTo) {
    LineItem::setPositionTo(posTo);
    m_penGradient.setFinalStop(getLine().p2());
    updatePolygon();
}



}
