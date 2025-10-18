#include "ovitemgeometrylayer.hpp"

#include "ovinternalscene.hpp"

namespace ObjectViewLayers {

OVItemGeometryLayer::OVItemGeometryLayer(QWidget* parent) :
    OVNavigationLayer(parent)
{
    m_pSystemGrabHeadItem = new QGraphicsPathItem;
    getScene()->addItem(m_pSystemGrabHeadItem);
    m_pSystemGrabHeadItem->show();
}

void OVItemGeometryLayer::setGeometryChangingEnabled(bool isEn)
{
    m_isGeometryChangingEnabled = isEn;
}

void OVItemGeometryLayer::wheelEvent(QWheelEvent *e)
{
    OVNavigationLayer::wheelEvent(e);
}

void OVItemGeometryLayer::mousePressEvent(QMouseEvent *e)
{
    OVNavigationLayer::mousePressEvent(e);
}

void OVItemGeometryLayer::mouseMoveEvent(QMouseEvent *e)
{
    OVNavigationLayer::mouseMoveEvent(e);
}

void OVItemGeometryLayer::mouseReleaseEvent(QMouseEvent *e)
{
    OVNavigationLayer::mouseReleaseEvent(e);
}

} // namespace ObjectViewItems
