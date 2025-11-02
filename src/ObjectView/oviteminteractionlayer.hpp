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
    void setInteractionEnabled(bool isEn);

private:
    bool m_isInteractionEnabled {false};

    QGraphicsItem*  m_pCurrentGrabItem {nullptr};
};

} // namespace ObjectItems

