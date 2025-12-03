#include "basicitem.hpp"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include <math.h>

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>

namespace ObjectItems
{

BasicItem::BasicItem(QGraphicsItem *parent) :
    QObject(),
    QGraphicsItem(parent),
    BasicItemInterface()
{
    setFlag(ItemSendsScenePositionChanges, true);
    setFlag(ItemHasNoContents, true);
    setFlag(ItemSendsGeometryChanges, true);
    setHandlesChildEvents(true);
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

QMenu *BasicItem::createContextMenu()
{
    auto res = new QMenu;

    auto pOpacityAction = new QAction("Прозрачный");
    connect(pOpacityAction, &QAction::triggered,
            this, [this, pOpacityAction]() {
        if (opacity() < 0.9) {
            setOpacity(1);
        } else {
            setOpacity(0.2);
        }
    });
    pOpacityAction->setCheckable(true);
    pOpacityAction->setChecked(opacity() < 0.9);

    res->addAction(pOpacityAction);

    return res;
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
        emit itemMoved(pos());
        break;

    case ItemScenePositionHasChanged:
        emit itemMovedOnScene(scenePos());
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
    QGraphicsItem::mousePressEvent(e);
    m_prevClickScreenPos = e->screenPos();
    m_clickOffset = e->scenePos() - scenePos();
    e->setAccepted(true);
}

void BasicItem::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{
    QGraphicsItem::mouseMoveEvent(e);
    if (m_isDeltaGot) {
        processMoveEvent(e);
    } else if (QLineF(m_prevClickScreenPos, e->screenPos()).length() > m_startMoveDelta) {
        processMoveEvent(e);
        m_isDeltaGot = true;
    }
}

void BasicItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    QGraphicsItem::mouseReleaseEvent(e);
    if (!m_isDeltaGot) {
        emit itemClicked();
    }
    m_isDeltaGot = false;
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

void BasicItem::processMoveEvent(QGraphicsSceneMouseEvent *e)
{
    auto targetPos = e->scenePos() - m_clickOffset;
    if (parentItem()) {
        targetPos = parentItem()->mapFromScene(targetPos);
    }
    setPos(targetPos);
}

}
