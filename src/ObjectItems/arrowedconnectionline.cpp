#include "arrowedconnectionline.hpp"

#include <Components/Logger/Logger.h>

namespace ObjectItems {

ArrowedConnectionLine::ArrowedConnectionLine(QGraphicsItem* parent) :
    AbstractConnectionLine(parent)
{
    setSystemName("Arrowed connection line");

    createSubitem(m_line);

    createSubitem(m_forwardArrow);
    m_forwardArrow->setZValue(1);

    createSubitem(m_backwardArrow);
    m_backwardArrow->setZValue(1);

    connect(this, &BasicItem::graphicalDataChanged,
            this, [this](){
        m_line->setLine(getLine());
        m_line->setPen(getStylePen());

        auto lineDirection = getDirection();
        if (lineDirection & LineDirectionType::Forward) {
            // Arrow
            m_forwardArrow->setPath(createArrowPath());
            m_forwardArrow->setTransformOriginPoint(m_forwardArrow->boundingRect().center());
            m_forwardArrow->setRotation(90 - getLine().angle());

            // Position
            m_forwardArrow->setPos(getPositionTo() - m_forwardArrow->boundingRect().center());

            // Drawing
            m_forwardArrow->show();
            m_forwardArrow->setBrush(getLineColor());
            m_forwardArrow->setPen(getStylePen());
        } else {
            m_forwardArrow->hide();
        }

        if (lineDirection & LineDirectionType::Backward) {
            // Arrow
            m_backwardArrow->setPath(createArrowPath());
            m_backwardArrow->setTransformOriginPoint(m_backwardArrow->boundingRect().center());
            m_backwardArrow->setRotation(270 - getLine().angle());

            // Position
            m_backwardArrow->setPos(getPositionFrom() - m_backwardArrow->boundingRect().center());

            // Drawing
            m_backwardArrow->show();
            m_backwardArrow->setBrush(getLineColor());
            m_backwardArrow->setPen(getStylePen());
        } else {
            m_backwardArrow->hide();
        }
    });
}

} // namespace ObjectItems

