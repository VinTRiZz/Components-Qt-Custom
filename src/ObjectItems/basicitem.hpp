#pragma once

#include <QGraphicsItem>

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

signals:
    void idChanged();
    void displayNameChanged();
    void internalDataChanged();
    void graphicalDataChanged();

    void itemSelected();
    void itemDeselected();

    void itemClicked();
    void itemMoved();
    void itemMovedOnScene();

protected:
    // QGraphicsItem interface
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    template <typename ItemTypeT, typename...InitArgs>
    std::enable_if_t<std::is_base_of_v<QGraphicsItem, ItemTypeT>, void> createSubitem(ItemTypeT*& pItem, InitArgs&&...args) {
        pItem = new ItemTypeT(args..., this);
        registerSubitem(pItem);

        if constexpr (std::is_base_of_v<BasicItem, ItemTypeT>) {
            QObject::connect(this, &BasicItem::graphicalDataChanged,
                             pItem, [this, pItem](){
                pItem->setLinePen(getLinePen());
                pItem->setHoverPen(getHoverPen());
                pItem->setSelectionPen(getSelectionPen());
            });
        }
    }

    void registerSubitem(QGraphicsItem* pItem);

    void mousePressEvent(QGraphicsSceneMouseEvent* e) override;

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

    friend class DebugMaster;
};

}
