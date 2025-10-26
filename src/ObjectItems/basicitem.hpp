#pragma once

#include <QGraphicsItem>

#include "basiciteminterface.hpp"

namespace ObjectItems {

class BasicItem :
        public QObject,
        public QGraphicsItem,
        public BasicItemInterface
{
    Q_OBJECT
public:
    explicit BasicItem(QGraphicsItem* parent = nullptr);
    ~BasicItem();

    // Для отладки
    void setCenterVisible(bool isCenterVisible = true);
    void setBoundingRectVisible(bool isBRectVisible = true);

    QRectF boundingRect() const override;

signals:
    void idChanged();
    void dataChanged();
    void colorChanged();

    void itemCreated();
    void itemDeleted();

private:
    bool m_isCenterVisible {false};
    QRectF m_boundingRect;

    bool m_isCenterRectUpdated {false};
    QRect m_centerRect;
    QRect m_centerRoundRect;

    bool m_isBoundingRectVisible {false};

    QRect createDebugRect(double rectScale = 1) const;

protected:
    void setBoundingRect(const QRectF& bRect);

    // QGraphicsItem interface
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    // BasicItemInterface interface
    void processIdChange() override;
    void processDataChange() override;
    void processColorChange() override;
};

}
