#include "oviteminteractionlayer.hpp"

#include "ovinternalscene.hpp"

namespace OVLayers {

OVItemInteractionLayer::OVItemInteractionLayer(QWidget* parent) :
    OVCanvasLayer(parent)
{
    m_pSystemGrabHeadItem = new QGraphicsPathItem;
    getScene()->addItem(m_pSystemGrabHeadItem);
    m_pSystemGrabHeadItem->show();
}

void OVItemInteractionLayer::setGeometryChangingEnabled(bool isEn)
{
    m_isGeometryChangingEnabled = isEn;
}

void OVItemInteractionLayer::wheelEvent(QWheelEvent *e)
{
    OVCanvasLayer::wheelEvent(e);
}

void OVItemInteractionLayer::mousePressEvent(QMouseEvent *e)
{
    OVCanvasLayer::mousePressEvent(e);
}

void OVItemInteractionLayer::mouseMoveEvent(QMouseEvent *e)
{
    OVCanvasLayer::mouseMoveEvent(e);

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

void OVItemInteractionLayer::mouseReleaseEvent(QMouseEvent *e)
{
    OVCanvasLayer::mouseReleaseEvent(e);
}

} // namespace ObjectItems
