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

    QPainterPath shape() const override;
    QRectF boundingRect() const override;

signals:
    void idChanged();
    void displayNameChanged();
    void internalDataChanged();
    void graphicalDataChanged();

    void itemCreated();
    void itemDeleted();

    void itemSelected();
    void itemDeselected();

    void itemMoved();

protected:
    // QGraphicsItem interface
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    template <typename T>
    std::enable_if_t<std::is_base_of_v<QGraphicsItem, T>, void> createSubitem(T*& pItem) {
        pItem = new T(this);
        registerSubitem(pItem);
    }

    void registerSubitem(QGraphicsItem* pItem);

    // BasicItemInterface interface
private:
    void processIdChange() override;
    void processDisplayNameChange() override;
    void processInternalDataChange() override;
    void processColorChange() override;

    friend class DebugMaster;
};

class DebugMaster {
    BasicItem* m_targetItem {nullptr};
public:
    DebugMaster(BasicItem* pTargetItem);
    ~DebugMaster();

    BasicItem* getTargetItem() const;

    void debug_setCustomRectVisible(const QRectF& rect, bool isCRectVisible = true);
    void debug_setCenterVisible(bool isCenterVisible = true);
    void debug_setBoundingRectVisible(bool isBRectVisible = true);

private:
    QGraphicsRectItem* m_debugCustomRectItem {nullptr};
    QGraphicsRectItem* m_debugRectItem {nullptr};
    QGraphicsRectItem* m_debugSizeRectItem {nullptr};
};

}
