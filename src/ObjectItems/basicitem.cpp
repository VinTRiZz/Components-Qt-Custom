#include "basicitem.hpp"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include <math.h>

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

void BasicItem::debug_setCenterVisible(bool isCenterVisible)
{
    m_isCenterVisible = isCenterVisible;
    update(boundingRect());
}

void BasicItem::debug_setBoundingRectVisible(bool isBRectVisible)
{
    m_isBoundingRectVisible = isBRectVisible;
    update(boundingRect());
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

QRect BasicItem::createDebugRect(double rectScale) const
{
    auto bRect = boundingRect();
    const int rectSize = std::round(std::max(25.0, std::max(bRect.width(), bRect.height())));

    QRect targetRect;
    targetRect.setWidth(rectSize);
    targetRect.setHeight(rectSize);

    QTransform scaleTransf;
    scaleTransf.scale(rectScale, rectScale);
    return scaleTransf.mapRect(targetRect);
}

void BasicItem::paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget)
{
    if (m_isBoundingRectVisible) [[unlikely]] {
        auto bRect = boundingRect();
        painter->fillRect(bRect, Qt::darkMagenta);
        QTransform scaleTransf;
        scaleTransf.scale(0.5, 0.5);
        scaleTransf.translate(bRect.center().x(), bRect.center().y());
        painter->fillRect(scaleTransf.mapRect(bRect), Qt::white);
    }

    if (m_isCenterVisible) [[unlikely]] {
        if (!m_isCenterRectUpdated) {
            m_centerRect = createDebugRect();
            m_centerRect.moveCenter(boundingRect().center().toPoint());
            m_centerRoundRect = createDebugRect(2);
            m_centerRoundRect.moveCenter(m_centerRect.center());
        }
        painter->fillRect(m_centerRoundRect, Qt::darkMagenta);
        painter->fillRect(m_centerRect, Qt::darkGray);
    }

    if (m_isBoundingRectVisible || m_isCenterVisible) [[unlikely]] {
        auto textRect = createDebugRect(2);
        textRect.moveCenter(boundingRect().center().toPoint());
        QFont pFont;
        pFont.setPointSize(6);
        painter->setFont(pFont);
        painter->setPen(Qt::black);
        painter->drawText(textRect,
                          Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextWordWrap,
                          getSystemName());
    }
}

QVariant BasicItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    auto res = QGraphicsItem::itemChange(change, value);

    switch (change)
    {
    case ItemPositionChange:
        m_isCenterRectUpdated = false;
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

}
