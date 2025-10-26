#include "arrowedconnectionline.hpp"

#include <Components/Logger/Logger.h>

namespace ObjectItems {

ArrowedConnectionLine::ArrowedConnectionLine(QGraphicsItem* parent) :
    AbstractConnectionLine(parent)
{
    createSubitem(m_line);
    createSubitem(m_forwardArrow);
    createSubitem(m_backwardArrow);

    connect(this, &BasicItem::graphicalDataChanged,
            this, [this](){
        m_line->setLine(getLine());

        auto lineDirection = getDirection();
        if (lineDirection & LineDirectionType::Forward) {
            // Arrow
            m_forwardArrow->setPath(createArrowPath());
            m_forwardArrow->setTransformOriginPoint(m_forwardArrow->boundingRect().center());
            m_forwardArrow->setRotation(90 - getLine().angle());

            // Position
            m_forwardArrow->setPos(getPositionTo() - m_forwardArrow->boundingRect().center());
            m_forwardArrow->show();
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
            m_backwardArrow->show();
        } else {
            m_backwardArrow->hide();
        }
    });
}

} // namespace ObjectItems

