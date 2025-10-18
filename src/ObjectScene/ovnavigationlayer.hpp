#pragma once

#include "ovcanvaslayer.hpp"

namespace ObjectViewLayers {

/**
 * @brief The OVNavigationLayer class Уровень, определяющий поведение навигации по сцене
 */
class OVNavigationLayer : public OVCanvasLayer
{
    Q_OBJECT
public:
    explicit OVNavigationLayer(QWidget* parent = nullptr);

    double getCurrentScale() const;

public slots:
    void zoomIn();
    void zoomOut();
    void customZoom(double scaleCoeff);

signals:
    void scaleChanged(double);

protected:
    void wheelEvent(QWheelEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

private:
    bool m_isNavigationEnabled {true}; //! Флаг включения навигации

    bool m_isHoldingMiddleButton{
        false};  //! Флаг факта того, что пользователь кликнул СКМ на сцене

    QPointF m_prevPos;  //! Позиция нажатия на графе
};

} // namespace ObjectViewLayers

