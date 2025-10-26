#pragma once

#include "ovcanvaslayer.hpp"

namespace ObjectViewLayers {

/**
 * @brief The OVItemGeometryLayer class Уровень изменения геометрии объектов
 */
class OVItemInteractionLayer : public OVCanvasLayer
{
public:
    explicit OVItemInteractionLayer(QWidget* parent = nullptr);

public slots:
    void setGeometryChangingEnabled(bool isEn);

protected:
    void wheelEvent(QWheelEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

private:
    bool m_isGeometryChangingEnabled {false};

    QGraphicsPathItem*  m_pSystemGrabHeadItem {nullptr};
    QGraphicsItem*      m_pCurrentGrabItem {nullptr};
};

/**
 * @brief The OVInteractiveItemMx class Миксин для добавления обработки объектом нажатий
 */
template <typename T>
class OVInteractiveItemMx : public T {
    void processPress(const QPointF pressScenePos, Qt::MouseButtons pressButtons) {
        static_cast<T*>(this)->mousePressEvent(pressScenePos, pressButtons);
    }

    void processHover(const QPointF& hoverPos) {
        static_cast<T*>(this)->mouseHoverEvent(hoverPos);
    }

    void processRelease(const QPointF pressScenePos, Qt::MouseButtons pressButtons) {
        static_cast<T*>(this)->mouseReleaseEvent(pressScenePos, pressButtons);
    }

    friend class OVItemInteractionLayer;
};

} // namespace ObjectViewItems

