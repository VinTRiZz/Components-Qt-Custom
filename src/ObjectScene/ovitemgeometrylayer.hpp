#pragma once

#include "ovnavigationlayer.hpp"

namespace ObjectViewLayers {

/**
 * @brief The OVItemGeometryLayer class Уровень изменения геометрии объектов
 */
class OVItemGeometryLayer : public OVNavigationLayer
{
public:
    explicit OVItemGeometryLayer(QWidget* parent = nullptr);

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

} // namespace ObjectViewItems

