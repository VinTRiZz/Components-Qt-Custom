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

//    auto pObject = getGrabObject();
//    if (nullptr != pObject) {
//        if (getIsGridEnabled()) {
//            int gridSizeHalf = std::round(getGridSize() / 2.0);

//            QPointF magnetPos;
//            if (auto magnetX = int(currentPos.x()) % gridSizeHalf;
//                magnetX != 0) {
//                magnetPos.setX(std::round(currentPos.x() / gridSizeHalf) *
//                               gridSizeHalf);
//            }

//            if (auto magnetY = int(currentPos.y()) % gridSizeHalf;
//                magnetY != 0) {
//                magnetPos.setY(std::round(currentPos.y() / gridSizeHalf) *
//                               gridSizeHalf);
//            }

//            pObject->setPos(magnetPos - pObject->boundingRect().center());
//        } else {
//            pObject->setPos(currentPos - pObject->boundingRect().center());
//        }
//    }
}

void OVItemGeometryLayer::mouseReleaseEvent(QMouseEvent *e)
{
    OVNavigationLayer::mouseReleaseEvent(e);
}

} // namespace ObjectViewItems
