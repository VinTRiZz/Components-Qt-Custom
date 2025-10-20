#include "ovnavigationlayer.hpp"

#include <QWheelEvent>

#include <QScrollBar>

#include <QVariantAnimation>

namespace ObjectViewLayers {

OVNavigationLayer::OVNavigationLayer(QWidget* parent) :
    OVCanvasLayer(parent)
{

}

void OVNavigationLayer::zoomIn() {
    auto anim = new QVariantAnimation(this);
    anim->setStartValue(1.01);
    anim->setEndValue(1.2);
    anim->setDuration(100);
    connect(anim, &QVariantAnimation::valueChanged,
            this, [this](const QVariant& deltaV){
        customZoom(deltaV.toDouble());
    });
    anim->start(QVariantAnimation::DeleteWhenStopped);
}

void OVNavigationLayer::zoomOut() {
    auto anim = new QVariantAnimation(this);
    anim->setStartValue(0.99);
    anim->setEndValue(0.8);
    anim->setDuration(100);
    connect(anim, &QVariantAnimation::valueChanged,
            this, [this](const QVariant& deltaV){
        customZoom(deltaV.toDouble());
    });
    anim->start(QVariantAnimation::DeleteWhenStopped);
}

void OVNavigationLayer::customZoom(double scaleCoeff) {    
    scale(scaleCoeff, scaleCoeff);
    emit scaleChanged(scaleCoeff);
}

double OVNavigationLayer::getCurrentScale() const {
    return transform().m11();
}

void OVNavigationLayer::setNavigationEnabled(bool isEn)
{
    m_isNavigationEnabled = isEn;
}


void OVNavigationLayer::wheelEvent(QWheelEvent* e) {
    if (!m_isNavigationEnabled) {
        return;
    }

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
    if (!m_isNavigationEnabled) {
        return;
    }

    m_isHoldingMiddleButton = (e->button() == Qt::MiddleButton);
    if (m_isHoldingMiddleButton) {
        setCursor(Qt::SizeAllCursor);
        m_prevPos = mapToScene(e->pos());
    }
}

void OVNavigationLayer::mouseMoveEvent(QMouseEvent* e) {
    if (!m_isNavigationEnabled) {
        return;
    }

    if (m_isHoldingMiddleButton) {
        auto deltaPos = e->pos() - mapFromScene(m_prevPos);
        horizontalScrollBar()->setSliderPosition(
            horizontalScrollBar()->sliderPosition() - deltaPos.x());
        verticalScrollBar()->setSliderPosition(
            verticalScrollBar()->sliderPosition() - deltaPos.y());
    }
}

void OVNavigationLayer::mouseReleaseEvent(QMouseEvent* e) {
    if (!m_isNavigationEnabled) {
        return;
    }

    setCursor(Qt::ArrowCursor);
    m_isHoldingMiddleButton = false;
}


} // namespace ObjectViewLayers
