#include "basicitem.hpp"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include <math.h>

#include <QDebug>

namespace ObjectItems
{

BasicItem::BasicItem(QGraphicsItem *parent) :
    QObject(),
    QGraphicsItem(parent),
    BasicItemInterface()
{
    connect(this, &QObject::destroyed,
            this, &BasicItem::itemDeleted);
    emit itemCreated();
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
    case ItemPositionChange:
        emit itemMoved();
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
    pItem->setData(ObjectDataRole::OBJECTDATAROLE_PARENTITEM_ID, getItemId());
}


void BasicItem::processIdChange()
{
    emit idChanged();
    setData(ObjectDataRole::OBJECTDATAROLE_ID, getItemId());
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

DebugMaster::DebugMaster(BasicItem *pTargetItem) :
    m_targetItem{pTargetItem}
{
    m_targetItem->createSubitem(m_debugRectItem);
    m_debugRectItem->setZValue(1'000'000);
    m_debugRectItem->setBrush(QBrush(Qt::magenta, Qt::DiagCrossPattern));
    m_debugRectItem->setPen(QPen(Qt::red, 2, Qt::DotLine));
    m_debugRectItem->hide();

    m_targetItem->createSubitem(m_debugSizeRectItem);
    m_debugSizeRectItem->setZValue(1'001'000);
    m_debugSizeRectItem->setBrush(QBrush(Qt::darkMagenta, Qt::DiagCrossPattern));
    m_debugSizeRectItem->setPen(QPen(Qt::green, 4, Qt::SolidLine));
    m_debugSizeRectItem->hide();

    m_targetItem->createSubitem(m_debugCustomRectItem);
    m_debugCustomRectItem->setZValue(1'000'001);
    m_debugCustomRectItem->setBrush(QBrush(Qt::darkCyan, Qt::BDiagPattern));
    m_debugCustomRectItem->setPen(QPen(Qt::darkCyan, 3, Qt::DashDotDotLine));
    m_debugCustomRectItem->hide();
}

DebugMaster::~DebugMaster()
{

}

BasicItem *DebugMaster::getTargetItem() const
{
    return m_targetItem;
}

void DebugMaster::debug_setCustomRectVisible(const QRectF &rect, bool isCRectVisible)
{
    m_debugCustomRectItem->setRect(rect);
    if (isCRectVisible) {
        m_debugCustomRectItem->show();
    } else {
        m_debugCustomRectItem->setRect({});
        m_debugCustomRectItem->hide();
    }
}

void DebugMaster::debug_setCenterVisible(bool isCenterVisible)
{
    if (isCenterVisible) {
        QTransform scaleTrasnf;
        scaleTrasnf.scale(0.2, 0.2);
        m_debugSizeRectItem->setRect(scaleTrasnf.mapRect(m_targetItem->boundingRect()));
        m_debugSizeRectItem->show();
    } else {
        m_debugSizeRectItem->setRect({});
        m_debugSizeRectItem->hide();
    }
}

void DebugMaster::debug_setBoundingRectVisible(bool isBRectVisible)
{
    if (isBRectVisible) {
        m_debugRectItem->setRect(m_targetItem->boundingRect());
        m_debugRectItem->show();
    } else {
        m_debugRectItem->setRect({});
        m_debugRectItem->hide();
    }
}

}
