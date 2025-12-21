#include "groupitem.hpp"

#include <QPainter>
#include <Components/Maths/Graphical.h>

namespace ObjectItems {

GroupItem::GroupItem(QGraphicsItem* parent)
    : BasicItem(parent)
{
    setSystemName("Group");
    setObjectType(ObjectType::OIT_GroupItem);

    setFlag(ItemIsSelectable, false);
    setFlag(ItemIsMovable, false);
    setFlag(ItemIsFocusable, false);

    createSubitem(m_bPolygon);
    connect(this, &BasicItem::graphicalDataChanged,
            this, [this](){
        m_bPolygon->setPen(getLinePen());
        m_bPolygon->setBrush(getBackgroundBrush());
    });

    connect(this, &BasicItem::itemMoved,
            this, [this](auto& sceneP){
        auto deltaPos = m_prevScenePos - sceneP;
        for (auto* pItem : m_groupItems) {
            pItem->setPos(mapFromScene(sceneP + deltaPos));
        }
        m_prevScenePos = sceneP;
    });
}

void GroupItem::setCommentedItems(const QList<BasicItem*>& items)
{
    QList<BasicItem*> diffItems;
    std::set_difference(items.begin(), items.end(),
                        m_groupItems.begin(), m_groupItems.end(),
                        std::back_inserter(diffItems));
    for (auto* pItem : diffItems) {
        connect(pItem, &BasicItem::itemMovedOnScene,
                   this, &GroupItem::updateBoundingPolygon);
    }
    m_groupItems = items;
    for (auto* pItem : items) {
        m_groupItemIds.insert(pItem->getItemId());
    }
    updateBoundingPolygon();
}

void GroupItem::addCommentedItem(BasicItem* item)
{
    if (item && !m_groupItems.contains(item)) {
        m_groupItems.append(item);
        m_groupItemIds.insert(item->getItemId());
        connect(item, &BasicItem::itemMovedOnScene,
                   this, &GroupItem::updateBoundingPolygon);
        updateBoundingPolygon();
    }
}

void GroupItem::removeCommentedItem(BasicItem *item)
{
    disconnect(item, &BasicItem::itemMovedOnScene,
               this, nullptr);
    m_groupItems.removeOne(item);
    m_groupItemIds.remove(item->getItemId());
}

void GroupItem::clearCommentedItems()
{
    while (m_groupItems.size()) {
        removeCommentedItem(m_groupItems.front());
    }
    m_groupItemIds.clear();
    updateBoundingPolygon();
}

QPolygonF GroupItem::getBoundingPolygon() const
{
    return m_bPolygon->polygon();
}

void GroupItem::updateBoundingPolygon()
{
    std::vector<Maths::Point> points;
    const auto offsetTopRPoint = QPointF(20, -20);
    const auto offsetTopLPoint = QPointF(-20, -20);
    const auto offsetButtRPoint = QPointF(20, 20);
    const auto offsetButtLPoint = QPointF(-20, 20);
    for (auto* pItem : m_groupItems) {

        auto itemParent = pItem->parentItem();

        // Для выпуклой оболочки
        auto bRect = pItem->boundingRect();
        bRect.moveTo(pItem->pos());

        if (itemParent) {
            bRect = itemParent->mapRectFromItem(itemParent, bRect);
            bRect.moveTo(pItem->scenePos());
        }
        points.push_back(bRect.topLeft() + offsetTopLPoint);
        points.push_back(bRect.topRight() + offsetTopRPoint);
        points.push_back(bRect.bottomLeft() + offsetButtLPoint);
        points.push_back(bRect.bottomRight() + offsetButtRPoint);
    }

    auto thisParent = parentItem();
    if (thisParent) {
        for (auto& p : points) {
            p = thisParent->mapFromScene(p);
        }
    }
    auto roundPoly = Maths::Graphical::createOuterPolygon(points);
    QPolygonF boundingPoly;
    roundPoly.reserve(roundPoly.size());
    for (auto& p : roundPoly) {
        boundingPoly.push_back(p);
    }

    m_bPolygon->setPolygon(boundingPoly);
    emit graphicalDataChanged();
}


} // namespace ObjectItems
