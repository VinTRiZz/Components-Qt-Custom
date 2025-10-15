#ifndef DYNAMICAREAITEM_H
#define DYNAMICAREAITEM_H

#include <QBrush>
#include <QPen>
#include <map>

#include "itembase.hpp"

namespace ObjectViewItems {

class SceneFieldItem : public ItemBase {
public:
    explicit SceneFieldItem(QGraphicsItem* parent = nullptr);

    void setFieldRect(const QRectF& iRect);
    QRectF getFieldRect() const;

    void setBrush(const QBrush& iBrush);
    void setPen(const QPen& iPen);

    void registerItem(ItemBase* pItem);
    ItemBase* getItem(
        ObjectViewItems::objectId_t targetItemId,
        ObjectViewItems::ObjectType targetItemType =
            ObjectViewItems::ObjectType::OBJECTTYPE_NONE) const;
    std::list<ItemBase*> getRegisteredItems() const;
    std::list<ObjectViewItems::objectId_t> getRegisteredIds() const;
    void clearRegisteredItems();

    void removeRegisteredItems(ObjectViewItems::ObjectType objT);
    void removeRegisteredItem(ItemBase* pItem);
    void removeRegisteredItemById(ObjectViewItems::objectId_t targetItemId);

    bool isIdAvailable(ObjectViewItems::objectId_t itemId) const;

private:
    QGraphicsRectItem* m_canvasRect{nullptr};
    std::map<ObjectViewItems::ObjectType, std::list<ItemBase*>>
        m_registeredItems;
};

}  // namespace ObjectViewItems

#endif  // DYNAMICAREAITEM_H
