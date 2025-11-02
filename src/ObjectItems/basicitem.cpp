#include "basicitem.hpp"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include <math.h>

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>

#include <QDebug>

namespace ObjectItems
{

BasicItem::BasicItem(QGraphicsItem *parent) :
    QObject(),
    QGraphicsItem(parent),
    BasicItemInterface()
{
    setFlag(ItemSendsScenePositionChanges, true);
    setFlag(ItemHasNoContents, true);
    setSystemName("Unknown");
}

BasicItem::~BasicItem()
{

}

QPainterPath BasicItem::shape() const
{
    QPainterPath path;
    for (QGraphicsItem* child : childItems()) {
        path.addPath(child->mapToParent(child->shape()));
    }
    return path;
}

QRectF BasicItem::boundingRect() const
{
    return shape().boundingRect();
}

void BasicItem::paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget)
{

}

QVariant BasicItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    auto res = QGraphicsItem::itemChange(change, value);

    switch (change)
    {
    case ItemPositionHasChanged:
        emit itemMoved();
        break;

    case ItemScenePositionHasChanged:
        emit itemMovedOnScene();
        break;

    case ItemSelectedHasChanged:
        if (value.toBool()) {
            emit itemSelected();
        } else {
            emit itemDeselected();
        }
        break;

    default:
        break;
    }

    return res;
}

void BasicItem::registerSubitem(QGraphicsItem *pItem)
{
    pItem->setParentItem(this);
    pItem->setData(ObjectDataRole::OBJECTDATAROLE_PARENTITEM_ID, QVariant::fromValue(this));

    auto pCon = new QMetaObject::Connection{};
    *pCon = QObject::connect(this, &BasicItem::idChanged,
                     this, [this, pItem, pCon](){
        if (!pItem) {
            QObject::disconnect(*pCon); // Если тут сегфолт или ещё что, извините
            delete pCon;
            return;
        }
        auto parentId = QVariant::fromValue(this);

        std::function<void(QGraphicsItem*)> updatePid = [&updatePid, this, parentId](QGraphicsItem* pItem){
            for (auto* pChild : pItem->childItems()) {
                if (dynamic_cast<BasicItem*>(pChild) != nullptr) {
                    continue;
                }
                updatePid(pChild);
            }
            pItem->setData(ObjectDataRole::OBJECTDATAROLE_PARENTITEM_ID, parentId);

            auto complexParentId = data(ObjectDataRole::OBJECTDATAROLE_COMPLEX_PARENTITEM_ID);
            if (complexParentId.isNull()) {
                pItem->setData(ObjectDataRole::OBJECTDATAROLE_PARENTITEM_ID, parentId);
            }
        };

        pItem->setData(ObjectDataRole::OBJECTDATAROLE_PARENTITEM_ID, parentId);
        auto complexParentId = data(ObjectDataRole::OBJECTDATAROLE_COMPLEX_PARENTITEM_ID);
        if (complexParentId.isNull()) {
            complexParentId = QVariant::fromValue(this);
        }
        pItem->setData(ObjectDataRole::OBJECTDATAROLE_COMPLEX_PARENTITEM_ID, complexParentId);

        // Апдейт дочерних НЕ объектов
        for (auto* pChild : childItems()) {
            updatePid(pChild);
        }
    });
}

void BasicItem::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    emit itemClicked();
    QGraphicsItem::mousePressEvent(e);
}

void BasicItem::hoverEnterEvent(QGraphicsSceneHoverEvent *e)
{
    m_isHovered = true;
    QGraphicsItem::hoverEnterEvent(e);
}

void BasicItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *e)
{
    m_isHovered = false;
    QGraphicsItem::hoverLeaveEvent(e);
}

bool BasicItem::getIsHovered() const
{
    return m_isHovered;
}

QPen BasicItem::getCurrentPen() const
{
    return getIsHovered() ? getHoverPen() : (isSelected() ? getSelectionPen() : getLinePen());
}

QBrush BasicItem::getCurrentBrush() const
{
    return getIsHovered() ? getBackgroundHoverBrush() : (isSelected() ? getBackgroundSelectionBrush() : getBackgroundBrush());
}

void BasicItem::processIdChange()
{
    setData(ObjectDataRole::OBJECTDATAROLE_ID, getItemId());
    emit idChanged();
}

void BasicItem::processDisplayNameChange()
{
    emit displayNameChanged();
}

void BasicItem::processInternalDataChange()
{
    emit internalDataChanged();
}

void BasicItem::processColorChange()
{
    emit graphicalDataChanged();
}

}
