#include "arrowline.hpp"

#include <math.h>

#include <QGraphicsLineItem>
#include <QGraphicsPathItem>
#include <QPainter>
#include <QPen>

namespace ObjectItems {

ArrowLine::ArrowLine(QGraphicsItem* parent) : LineItem(parent) {
    setSystemName("Соединение (радиальное)");
    setType(ObjectItems::OBJECTTYPE_ARROWLINE);
    m_line = new QGraphicsLineItem(this);
    registerSubitem(m_line);

    m_forwardArrow = new QGraphicsPathItem(this);
    registerSubitem(m_forwardArrow);

    m_backwardArrow = new QGraphicsPathItem(this);
    registerSubitem(m_backwardArrow);
}

void ArrowLine::setPen(const QPen& drawPen) {
    m_drawPen = drawPen;
}

void ArrowLine::setSelectedPen(const QPen& drawPen) {
    m_drawSelectedPen = drawPen;
}

void ArrowLine::setLine(const QLineF& iLine) {
    m_line->setLine(iLine);
    updateArrows();
}

void ArrowLine::setArrowType(LineArrowType t)
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

void ArrowLine::setArrowSize(qreal arrowSize) {
    m_arrowSize = arrowSize;
    updateArrows();
}

qreal ArrowLine::getArrowSize() const {
    return m_arrowSize;
}

QPainterPath ArrowLine::createArrow() const
{
    QPainterPath p;
    p.lineTo(m_arrowSize / 3.0, 0);
    p.lineTo(0, m_arrowSize);
    p.lineTo(0, m_arrowSize / 3.0);
    return p;
}

void ArrowLine::updateArrows()
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

QVariant ArrowLine::itemChange(GraphicsItemChange change,
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

}  // namespace ObjectItems
