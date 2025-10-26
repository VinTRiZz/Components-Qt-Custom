#pragma once

#include <QGraphicsView>

#include <Components/CustomQt/ObjectItems/SceneFieldItem.h>
#include <Components/CustomQt/ObjectItems/CenterItem.h>

namespace OVLayers {

class OVInternalScene;

/**
 * @brief The OVSceneConfigLayer class "Нулевой" уровень, настраивает сцену и полотно
 */
class OVCanvasLayer : public QGraphicsView
{
    Q_OBJECT

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
    ObjectItems::SceneFieldItem* getCanvas() const;
    ObjectItems::CenterItem* getCenterItem() const;

    QGraphicsItem*          getTopItem(const QPoint& viewportPos) const;
    QList<QGraphicsItem*>   getItems(const QPoint& viewportPos, bool sorted = true) const;

    double getCurrentScale() const;

public slots:
    void setNavigationEnabled(bool isEn);

    void zoomIn();
    void zoomOut();
    void customZoom(double scaleCoeff);

signals:
    void scaleChanged(double);

protected:
    virtual bool isSystemItem(QGraphicsItem* pItem) const;

    void wheelEvent(QWheelEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

    bool eventFilter(QObject* object, QEvent* event) override;

private:
    OVInternalScene* m_pInternalScene {nullptr};
    ObjectItems::SceneFieldItem* m_pCanvasItem {nullptr};
    ObjectItems::CenterItem *m_pCenterItem {nullptr};

    bool m_isNavigationEnabled {true}; //! Флаг включения навигации

    bool m_isHoldingMiddleButton{
        false};  //! Флаг факта того, что пользователь кликнул СКМ на сцене

    QPointF m_prevPos;  //! Позиция нажатия на графе
};

}
