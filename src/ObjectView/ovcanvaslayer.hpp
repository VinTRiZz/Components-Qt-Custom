#pragma once

#include <QGraphicsView>

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
    QGraphicsRectItem* getCanvas() const;

    QGraphicsItem*          getTopItem(const QPoint& viewportPos) const;
    QList<QGraphicsItem*>   getItems(const QPoint& viewportPos, bool sorted = true) const;

    double getCurrentScale() const;

    // Дублирование интерфейса сцены
    void addItem(QGraphicsItem* pItem);
    void removeItem(QGraphicsItem* pItem);

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
    QGraphicsRectItem* m_pCanvasItem {nullptr};

    bool m_isNavigationEnabled {true}; //! Флаг включения навигации

    bool m_isHoldingMiddleButton{
        false};  //! Флаг факта того, что пользователь кликнул СКМ на сцене

    QPointF m_prevPos;  //! Позиция нажатия на графе
};

}
