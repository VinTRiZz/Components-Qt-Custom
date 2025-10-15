#include "arrowline.hpp"

#include <math.h>

#include <QGraphicsLineItem>
#include <QGraphicsPathItem>
#include <QPainter>
#include <QPen>

namespace ObjectViewItems {

ArrowedLine::ArrowedLine(QGraphicsItem* parent) : ItemBase(parent) {
    setSystemName("Соединение (радиальное)");
    setType(ObjectViewItems::OBJECTTYPE_ARROWLINE);
    m_line = new QGraphicsLineItem(this);
    registerSubitem(m_line);

    m_forwardArrow = new QGraphicsPathItem(this);
    registerSubitem(m_forwardArrow);

    m_backwardArrow = new QGraphicsPathItem(this);
    registerSubitem(m_backwardArrow);
}

void ArrowedLine::setPen(const QPen& drawPen) {
    m_drawPen = drawPen;
}

void ArrowedLine::setSelectedPen(const QPen& drawPen) {
    m_drawSelectedPen = drawPen;
}

void ArrowedLine::setLine(const QLineF& iLine) {
    m_line->setLine(iLine);
    updateArrows();
}

void ArrowedLine::setLine(const QPointF& p1, const QPointF& p2) {
    setLine(QLineF(p1, p2));
}

QLineF ArrowedLine::getLine() const {
    return m_line->line();
}

void ArrowedLine::setArrowType(LineArrowType t)
{
    m_arrowType = t;

    // Для None
    m_forwardArrow->hide();
    m_backwardArrow->hide();

    if (m_arrowType & LineArrowType::Forward) {
        m_forwardArrow->show();
    }

    if (m_arrowType & LineArrowType::Backward) {
        m_backwardArrow->show();
    }

    // Обработка Bidirectional не нужна

    updateArrows();
}

void ArrowedLine::setArrowSize(qreal arrowSize) {
    m_arrowSize = arrowSize;
    updateArrows();
}

qreal ArrowedLine::getArrowSize() const {
    return m_arrowSize;
}

QPainterPath ArrowedLine::createArrow() const
{
    QPainterPath p;
    p.lineTo(m_arrowSize / 3.0, 0);
    p.lineTo(0, m_arrowSize);
    p.lineTo(0, m_arrowSize / 3.0);
    return p;
}

void ArrowedLine::updateArrows()
{
    auto forwardPath = createArrow();
    QTransform transf;
    transf.rotate(m_line->line().angle());
    forwardPath = transf.map(forwardPath);
    m_forwardArrow->setPath(forwardPath);

    auto backwardPath = createArrow();
    transf = QTransform();
    transf.rotate(m_line->line().angle());
    backwardPath = transf.map(backwardPath);
    m_backwardArrow->setPath(backwardPath);
}

QVariant ArrowedLine::itemChange(GraphicsItemChange change,
                                 const QVariant& value) {
    if (change == ItemSelectedChange) {
        if (value.toBool()) {
            m_line->setPen(m_drawSelectedPen);
        } else {
            m_line->setPen(m_drawPen);
        }
    }

    return ItemBase::itemChange(change, value);
}

}  // namespace ObjectViewItems
