#pragma once

#include "basicitem.hpp"

namespace ObjectItems {

class GroupItem : public BasicItem
{
    Q_OBJECT
public:
    explicit GroupItem(QGraphicsItem* parent = nullptr);

    // Установка комментируемых элементов
    void setCommentedItems(const QList<BasicItem*>& items);
    void addCommentedItem(BasicItem* item);
    void clearCommentedItems();

private:
    void updateBoundingPolygon();

    QGraphicsPolygonItem* m_bPolygon {nullptr};
    QList<BasicItem*> m_commentedItems;
};

} // namespace ObjectItems

