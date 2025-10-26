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

}

BasicItem::~BasicItem()
{

}

void BasicItem::setCenterVisible(bool isCenterVisible)
{
    m_isCenterVisible = isCenterVisible;
    update(boundingRect());
}

void BasicItem::setBoundingRectVisible(bool isBRectVisible)
{
    m_isBoundingRectVisible = isBRectVisible;
    update(boundingRect());
}

QRectF BasicItem::boundingRect() const
{
    return m_boundingRect;
}

QRect BasicItem::createDebugRect(double rectScale) const
{
    const int rectSize = std::round(std::max(25.0, std::max(m_boundingRect.width(), m_boundingRect.height())));

    QRect targetRect;
    targetRect.setWidth(rectSize);
    targetRect.setHeight(rectSize);

    QTransform scaleTransf;
    scaleTransf.scale(rectScale, rectScale);
    return scaleTransf.mapRect(targetRect);
}

void BasicItem::setBoundingRect(const QRectF &bRect)
{
    m_boundingRect = bRect;
}

void BasicItem::paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget)
{
    if (m_isBoundingRectVisible) [[unlikely]] {
        painter->fillRect(m_boundingRect, Qt::darkMagenta);
        QTransform scaleTransf;
        scaleTransf.scale(0.5, 0.5);
        scaleTransf.translate(m_boundingRect.center().x(), m_boundingRect.center().y());
        painter->fillRect(scaleTransf.mapRect(m_boundingRect), Qt::white);
    }

    if (m_isCenterVisible) [[unlikely]] {
        if (!m_isCenterRectUpdated) {
            m_centerRect = createDebugRect();
            m_centerRect.moveCenter(m_boundingRect.center().toPoint());
            m_centerRoundRect = createDebugRect(2);
            m_centerRoundRect.moveCenter(m_centerRect.center());
        }
        painter->fillRect(m_centerRoundRect, Qt::darkMagenta);
        painter->fillRect(m_centerRect, Qt::darkGray);

        QFont pFont;
        pFont.setPointSize(6);
        painter->setFont(pFont);
        painter->setPen(Qt::black);
    }

    if (m_isBoundingRectVisible || m_isCenterVisible) [[unlikely]] {
        auto textRect = createDebugRect(2);
        textRect.moveCenter(m_boundingRect.center().toPoint());
        painter->drawText(textRect,
                          Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextWordWrap,
                          getSystemName());
    }
}

QVariant BasicItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange) {
        m_isCenterRectUpdated = false;
    }
    return QGraphicsItem::itemChange(change, value);
}


void BasicItem::processIdChange()
{
    emit idChanged();
}

void BasicItem::processDataChange()
{
    emit dataChanged();
}

void BasicItem::processColorChange()
{
    emit colorChanged();
}

}
