#include "oviteminteractionlayer.hpp"

#include <QMouseEvent>

#include "ovconstants.hpp"

namespace OVLayers {

OVItemInteractionLayer::OVItemInteractionLayer(QWidget* parent) :
    OVModeLayer(parent)
{
    m_grabberItem = new ObjectItems::BasicItem(getCanvas());
    m_grabberItem->setZValue(ItemLayers::GrabItemLayer);
    m_grabberItem->show();

    connect(this, &OVCanvasLayer::scaleChanged,
            this, &OVItemInteractionLayer::updateGrabberPosition);
}

void OVItemInteractionLayer::connectGrabItem(QGraphicsItem *pItem, const QPointF& offsetPos)
{
    m_grabbedItemConnections[pItem] = QObject::connect(m_grabberItem, &ObjectItems::BasicItem::itemMovedOnScene,
                     m_grabberItem, [this, pItem, offsetPos](){
        pItem->setPos(m_grabberItem->pos() + offsetPos);
    });
}

std::list<QGraphicsItem *> OVItemInteractionLayer::getGrabbedItems() const
{
    std::list<QGraphicsItem*> res;

    for (auto& [item, con] : m_grabbedItemConnections) {
        res.push_back(item);
    }

    return res;
}

void OVItemInteractionLayer::disconnectGrabItem(QGraphicsItem *pItem)
{
    if (!m_grabbedItemConnections.count(pItem)) {
        return;
    }
    QObject::disconnect(m_grabbedItemConnections.at(pItem));
    m_grabbedItemConnections.erase(pItem);
}

void OVItemInteractionLayer::connectGrabLine(ObjectItems::AbstractConnectionLine *pLine)
{
    m_grabbedItemConnections[pLine] = QObject::connect(m_grabberItem, &ObjectItems::BasicItem::itemMovedOnScene,
                     m_grabberItem, [this, pLine](){
        pLine->setPositionTo(m_grabberItem->pos());
    });
}

std::list<ObjectItems::AbstractConnectionLine*> OVItemInteractionLayer::getGrabbedLines() const
{
    std::list<ObjectItems::AbstractConnectionLine*> res;

    for (auto& [item, con] : m_grabbedItemConnections) {
        auto pCastedItem = dynamic_cast<ObjectItems::AbstractConnectionLine*>(item);
        if (!pCastedItem) {
            continue;
        }
        res.push_back(pCastedItem);
    }

    return res;
}

void OVItemInteractionLayer::disconnectGrabLine(ObjectItems::AbstractConnectionLine *pLine)
{
    if (!m_grabbedItemConnections.count(pLine)) {
        return;
    }
    QObject::disconnect(m_grabbedItemConnections.at(pLine));
    m_grabbedItemConnections.erase(pLine);
}

void OVItemInteractionLayer::updateGrabberPosition()
{
    m_grabberItem->setPos(mapToScene(mapFromGlobal(QCursor::pos())));
}

void OVItemInteractionLayer::mouseMoveEvent(QMouseEvent *e)
{
    updateGrabberPosition();    
    OVCanvasLayer::mouseMoveEvent(e);
}

void OVItemInteractionLayer::enterEvent(QEvent *e)
{
    updateGrabberPosition();
    OVCanvasLayer::enterEvent(e);
}

void OVItemInteractionLayer::leaveEvent(QEvent *e)
{
    updateGrabberPosition();
    OVCanvasLayer::leaveEvent(e);
}

} // namespace ObjectItems
