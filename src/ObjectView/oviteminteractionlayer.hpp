#pragma once

#include "ovcanvaslayer.hpp"

#include <Components/CustomQt/ObjectView/ObjectItems.h>

namespace OVLayers {

/**
 * @brief The OVItemGeometryLayer class Уровень интеракции с объектами
 */
class OVItemInteractionLayer : public OVCanvasLayer
{
public:
    explicit OVItemInteractionLayer(QWidget* parent = nullptr);

    void connectGrabItem(QGraphicsItem* pItem, const QPointF &offsetPos = {});
    std::list<QGraphicsItem*> getGrabbedItems() const;
    void disconnectGrabItem(QGraphicsItem* pItem);

    void connectGrabLine(ObjectItems::AbstractConnectionLine* pLine);
    std::list<ObjectItems::AbstractConnectionLine*> getGrabbedLines() const;
    void disconnectGrabLine(ObjectItems::AbstractConnectionLine* pLine);

private:
    bool m_isInteractionEnabled {false};

    std::map<QGraphicsItem*, QMetaObject::Connection> m_grabbedItemConnections;
    ObjectItems::BasicItem* m_grabberItem {nullptr};

    QGraphicsPathItem* m_highlightItem {nullptr};

private slots:
    void updateGrabberPosition();

protected:
    void mouseMoveEvent(QMouseEvent* e) override;

    void enterEvent(QEvent* e) override;
    void leaveEvent(QEvent* e) override;
};

} // namespace ObjectItems

