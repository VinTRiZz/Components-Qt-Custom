#pragma once

#include "ovcanvaslayer.hpp"

namespace OVLayers {

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
    bool m_isInteractionEnabled {false};

    QGraphicsItem*  m_pCurrentGrabItem {nullptr};
};

} // namespace ObjectItems

