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
    void debug_setCenterVisible(bool isCenterVisible = true);
    void debug_setBoundingRectVisible(bool isBRectVisible = true);

    QRectF boundingRect() const override;

signals:
    void idChanged();
    void internalDataChanged();
    void graphicalDataChanged();

    void itemCreated();
    void itemDeleted();


private:
    QRectF m_boundingRect;

    // ОТЛАДКА
    bool m_isCenterVisible {false};
    bool m_isBoundingRectVisible {false};

    // ОТЛАДКА
    bool m_isCenterRectUpdated {false};
    QRect m_centerRect;
    QRect m_centerRoundRect;

    // ОТЛАДКА
    QRect createDebugRect(double rectScale = 1) const;

    void updateSubitemsParent();

protected:
    void setBoundingRect(const QRectF& bRect);

    // QGraphicsItem interface
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    template <typename T>
    std::enable_if_t<std::is_base_of_v<QGraphicsItem, T>, void> createSubitem(T*& pItem) {
        pItem = new T(this);
        pItem->setData(ObjectDataRole::OBJECTDATAROLE_PARENTITEM_ID, getItemId());
    }

    // BasicItemInterface interface
private:
    void processIdChange() override;
    void processInternalDataChange() override;
    void processColorChange() override;
};

}
