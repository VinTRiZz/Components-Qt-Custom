#include "arrowedconnectionline.hpp"

#include <Components/Logger/Logger.h>

namespace ObjectItems {

ArrowedConnectionLine::ArrowedConnectionLine(QGraphicsItem* parent) :
    AbstractConnectionLine(parent)
{
    setSystemName("Arrowed connection line");

    createSubitem(m_line);
    m_line->setZValue(1);
    createSubitem(m_lineSelected);
    m_lineSelected->hide();

    createSubitem(m_forwardArrow);
    m_forwardArrow->setZValue(2);
    m_forwardArrow->setBrush(getLineColor());
    m_forwardArrow->setPen(getStylePen());

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
        auto line = getLine();

        auto stylePen = getStylePen();
        m_line->setLine(line);
        m_line->setPen(stylePen);

        stylePen.setWidth(stylePen.width() + 2);
        stylePen.setColor(getSelectionColor());
        m_lineSelected->setLine(line);
        m_lineSelected->setPen(stylePen);

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

