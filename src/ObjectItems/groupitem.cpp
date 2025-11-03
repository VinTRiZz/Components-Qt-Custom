#include "groupitem.hpp"

#include <QPainter>
#include <Components/Maths/Graphical.h>

namespace ObjectItems {

GroupItem::GroupItem(QGraphicsItem* parent)
    : BasicItem(parent)
{
    setSystemName("Item group");

    setFlag(ItemIsSelectable, false);
    setFlag(ItemIsMovable, false);
    setFlag(ItemIsFocusable, false);

    createSubitem(m_bPolygon);
    connect(this, &BasicItem::graphicalDataChanged,
            this, [this](){
        m_bPolygon->setPen(getLinePen());
        m_bPolygon->setBrush(getBackgroundBrush());
    });
}

void GroupItem::setCommentedItems(const QList<BasicItem*>& items)
{
    m_commentedItems = items;
    updateBoundingPolygon();
}

void GroupItem::addCommentedItem(BasicItem* item)
{
    if (item && !m_commentedItems.contains(item)) {
        m_commentedItems.append(item);
        updateBoundingPolygon();
    }
}

void GroupItem::clearCommentedItems()
{
    m_commentedItems.clear();
    updateBoundingPolygon();
}

void GroupItem::updateBoundingPolygon()
{
    std::vector<Maths::Point> points;
    const auto offsetTopRPoint = QPointF(20, -20);
    const auto offsetTopLPoint = QPointF(-20, -20);
    const auto offsetButtRPoint = QPointF(20, 20);
    const auto offsetButtLPoint = QPointF(-20, 20);
    for (auto* pItem : m_commentedItems) {

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
}


} // namespace ObjectItems
