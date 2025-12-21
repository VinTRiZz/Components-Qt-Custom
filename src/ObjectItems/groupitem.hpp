#pragma once

#include "basicitem.hpp"

#include <QSet>

namespace ObjectItems {

class GroupItem : public BasicItem
{
    Q_OBJECT
    OBJECTITEMS_ITEM
public:
    explicit GroupItem(QGraphicsItem* parent = nullptr);

    // Установка комментируемых элементов
    void setCommentedItems(const QList<BasicItem*>& items);
    void addCommentedItem(BasicItem* item);
    void removeCommentedItem(BasicItem* item);
    void clearCommentedItems();

    QPolygonF getBoundingPolygon() const;

private:
    void updateBoundingPolygon();

    QGraphicsPolygonItem* m_bPolygon {nullptr};
    QList<BasicItem*> m_groupItems;

    QSet<objectId_t> m_groupItemIds; // TODO: Сериализовать дочерний массив?

    QPointF m_prevScenePos;
};

} // namespace ObjectItems

OBJECTITEMS_REGISTER_ITEM_WITH_FIELDS(
        ObjectItems::GroupItem,
        ObjectItems::BasicItem,
        m_groupItemIds);
