#pragma once

#include <QGraphicsItem>
#include <QAction>
#include <QMenu>

#include "basiciteminterface.hpp"

#include "debugmaster.hpp"

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

    QPainterPath shape() const override;
    QRectF boundingRect() const override;

    virtual QMenu* createContextMenu();

    // Require connect -- указывает, надо ли соединять айтем с изменением QPen / QBrush
    template <bool requireConnect = true, typename ItemTypeT, typename...InitArgs>
    std::enable_if_t<std::is_base_of_v<QGraphicsItem, ItemTypeT>, void> createSubitem(ItemTypeT*& pItem, InitArgs&&...args) {
        pItem = new ItemTypeT(args..., this);
        registerSubitem(pItem);

        if constexpr (std::is_base_of_v<BasicItem, ItemTypeT> && requireConnect) {
            QObject::connect(this, &BasicItem::graphicalDataChanged,
                             pItem, [this, pItem](){
                pItem->setLinePen(getLinePen());
                pItem->setHoverPen(getHoverPen());
                pItem->setSelectionPen(getSelectionPen());
            });
        }
    }

signals:
    void idChanged();
    void displayNameChanged();
    void internalDataChanged();
    void graphicalDataChanged();

    void itemSelected();
    void itemDeselected();

    void itemClicked();
    void itemMoved(const QPointF& newPos);
    void itemMovedOnScene(const QPointF& newScenePos);

protected:
    // QGraphicsItem interface
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    void registerSubitem(QGraphicsItem* pItem);

    void mousePressEvent(QGraphicsSceneMouseEvent* e) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* e) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* e) override;

    void hoverEnterEvent(QGraphicsSceneHoverEvent* e) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* e) override;

    bool getIsHovered() const;
    QPen getCurrentPen() const;
    QBrush getCurrentBrush() const;

    // BasicItemInterface interface
private:
    void processIdChange() override;
    void processDisplayNameChange() override;
    void processInternalDataChange() override;
    void processColorChange() override;

    bool m_isClickedOnMe {false};
    bool m_isHovered {false};

    QPointF m_prevClickScreenPos; // TODO: Придумать получше вариант, маппинг тут геморройный
    QPointF m_clickOffset;
    bool    m_isDeltaGot {false};   // Оптимизация
    double  m_startMoveDelta {2.0}; // Расстояние между точкой нажатия и точкой релиза для начала перемещения
    void    processMoveEvent(QGraphicsSceneMouseEvent* e);

    friend class DebugMaster;

    template<typename, typename>
    friend struct boost::hana::accessors_impl;
};

}

OBJECTITEMS_REGISTER_HIERARCHY(ObjectItems::BasicItem, ObjectItems::BasicItemInterface)
BOOST_HANA_ADAPT_STRUCT(
    ObjectItems::BasicItem

//    m_linePen                    ,
//    m_lineHoverPen               ,
//    m_selectionPen               ,
//    m_backgroundBrush            ,
//    m_backgroundHoverBrush       ,
//    m_backgroundSelectionBrush
);
