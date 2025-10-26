#ifndef DYNAMICAREAITEM_H
#define DYNAMICAREAITEM_H

#include <QBrush>
#include <QPen>
#include <map>

#include "itembase.hpp"

namespace ObjectItems {

class SceneFieldItem : public ItemBase {
public:
    explicit SceneFieldItem(QGraphicsItem* parent = nullptr);

    void setFieldRect(const QRectF& iRect);
    QRectF getFieldRect() const;

    void setBrush(const QBrush& iBrush);
    void setPen(const QPen& iPen);

    void registerItem(ItemBase* pItem);
    ItemBase* getItem(
        ObjectItems::objectId_t targetItemId,
        ObjectItems::ObjectType targetItemType =
            ObjectItems::ObjectType::OBJECTTYPE_NONE) const;
    std::list<ItemBase*> getRegisteredItems() const;
    std::list<ObjectItems::objectId_t> getRegisteredIds() const;
    void clearRegisteredItems();

    void removeRegisteredItems(ObjectItems::ObjectType objT);
    void removeRegisteredItem(ItemBase* pItem);
    void removeRegisteredItemById(ObjectItems::objectId_t targetItemId);

    bool isIdAvailable(ObjectItems::objectId_t itemId) const;

private:
    QGraphicsRectItem* m_canvasRect{nullptr};
    std::map<ObjectItems::ObjectType, std::list<ItemBase*>>
        m_registeredItems;
};

}  // namespace ObjectItems

#endif  // DYNAMICAREAITEM_H
