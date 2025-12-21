#pragma once

#include "basicitem.hpp"

namespace OVLayers {
class OVCanvasLayer;
}

namespace ObjectItems {

class MarkerItem : public BasicItem
{
    Q_OBJECT
    OBJECTITEMS_ITEM
public:
    explicit MarkerItem(QGraphicsItem* parent = nullptr);
    ~MarkerItem() override = default;

    void setTrackedItem(BasicItem *item);
    void setView(OVLayers::OVCanvasLayer* view);
    void updateMarkerPosition();

    void setMarkerHeight(double marHeight);

private:
    QPointF calculateTargetCenter() const;
    QPointF calculateMarkerPosition(const QPointF& targetCenter) const;
    QPainterPath createMarkerPath() const;

    BasicItem* m_trackedItem {nullptr};
    OVLayers::OVCanvasLayer* m_view {nullptr};
    QGraphicsPathItem* m_markerVisibleItem {nullptr};
    double m_markerHeight {35};

    objectId_t m_trackedItemId {}; // TODO: Сериализовать иначе?

    mutable bool m_isHeightChanged {true};
    mutable QPainterPath m_cachedMarkerPath;

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* e) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* e) override;
};

} // namespace ObjectItems

OBJECTITEMS_REGISTER_ITEM_WITH_FIELDS(
        ObjectItems::MarkerItem,
        ObjectItems::BasicItem,
        m_trackedItemId,
        m_markerHeight);
