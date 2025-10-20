#pragma once

#include <QGraphicsView>

#include <Components/CustomQt/ObjectScene/SceneFieldItem.h>
#include <Components/CustomQt/ObjectScene/CenterItem.h>

namespace ObjectViewLayers {

class OVInternalScene;

/**
 * @brief The OVSceneConfigLayer class "Нулевой" уровень, настраивает сцену и полотно
 */
class OVCanvasLayer : public QGraphicsView
{
    // Блокировка интерфейса
    using QGraphicsView::setScene;      // Запрещено к использованию извне
    using QGraphicsView::setSceneRect;  // Используйте setCanvasRect
    using QGraphicsView::items;         // Используйте getItems
    using QGraphicsView::itemAt;        // Используйте getTopItem

public:
    explicit OVCanvasLayer(QWidget* parent = nullptr);

    void setCanvasRect(const QRectF& iRect);
    QRectF getCanvasRect() const;

    OVInternalScene* getScene() const;
    ObjectViewItems::SceneFieldItem* getCanvas() const;
    ObjectViewItems::CenterItem* getCenterItem() const;

    QGraphicsItem*          getTopItem(const QPoint& viewportPos) const;
    QList<QGraphicsItem*>   getItems(const QPoint& viewportPos, bool sorted = true) const;

protected:
    virtual bool isSystemItem(QGraphicsItem* pItem) const;

private:
    OVInternalScene* m_pInternalScene {nullptr};
    ObjectViewItems::SceneFieldItem* m_pCanvasItem {nullptr};
    ObjectViewItems::CenterItem *m_pCenterItem {nullptr};
};

}
