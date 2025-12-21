#pragma once

#include "abstractconnectionline.hpp"

namespace ObjectItems {

class ElegantConnectionLine : public AbstractConnectionLine
{
    OBJECTITEMS_ITEM
public:
    explicit ElegantConnectionLine(QGraphicsItem* parent = nullptr);

private:
    QGraphicsPathItem* m_line{nullptr};
    QGraphicsPathItem* m_lineSelected{nullptr};
    QGraphicsPathItem* m_forwardArrow{nullptr};
    QGraphicsPathItem* m_backwardArrow{nullptr};

    QPainterPath createLinePath() const;
};

} // namespace ObjectItems

OBJECTITEMS_REGISTER_ITEM(
        ObjectItems::ElegantConnectionLine,
        ObjectItems::AbstractConnectionLine);
