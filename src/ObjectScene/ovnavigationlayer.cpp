#include "ovnavigationlayer.hpp"

#include <QWheelEvent>

#include <QScrollBar>

namespace ObjectViewLayers {

OVNavigationLayer::OVNavigationLayer(QWidget* parent) :
    OVCanvasLayer(parent)
{

}

void OVNavigationLayer::zoomIn() {
    customZoom(1.2);
}

void OVNavigationLayer::zoomOut() {
    customZoom(0.8);
}

void OVNavigationLayer::customZoom(double scaleCoeff) {
    scale(scaleCoeff, scaleCoeff);
    emit scaleChanged(scaleCoeff);
}

double OVNavigationLayer::getCurrentScale() const {
    return transform().m11();
}


void OVNavigationLayer::wheelEvent(QWheelEvent* e) {
    auto cursorPos = mapToScene(mapFromGlobal(QCursor::pos()));
    if (e->angleDelta().ry() > 0) {
        zoomOut();
    } else {
        zoomIn();
    }
    centerOn(cursorPos);
    auto nextCursorPos = mapToGlobal(mapFromScene(cursorPos));
    QCursor::setPos(nextCursorPos);
}

void OVNavigationLayer::mousePressEvent(QMouseEvent* e) {
    setCursor(Qt::ArrowCursor);
    m_isHoldingMiddleButton = (e->button() == Qt::MiddleButton);
    if (m_isHoldingMiddleButton) {
        setCursor(Qt::SizeAllCursor);
        m_prevPos = mapToScene(e->pos());
    }
}

void OVNavigationLayer::mouseMoveEvent(QMouseEvent* e) {
    if (m_isHoldingMiddleButton) {
        auto deltaPos = e->pos() - mapFromScene(m_prevPos);
        horizontalScrollBar()->setSliderPosition(
            horizontalScrollBar()->sliderPosition() - deltaPos.x());
        verticalScrollBar()->setSliderPosition(
            verticalScrollBar()->sliderPosition() - deltaPos.y());
    }
}

void OVNavigationLayer::mouseReleaseEvent(QMouseEvent* e) {
    setCursor(Qt::ArrowCursor);
    m_isHoldingMiddleButton = false;
}


} // namespace ObjectViewLayers
