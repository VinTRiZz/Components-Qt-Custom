#include "ovmodelayer.hpp"

namespace OVLayers {

OVMode::OVMode(OVCanvasLayer *pHostView) : m_pHostView{pHostView} { }

OVCanvasLayer *OVMode::getHostView() const {
    return m_pHostView;
}

void OVMode::process_mousePressEvent(QMouseEvent *event) { }

void OVMode::process_mouseReleaseEvent(QMouseEvent *event) { }

void OVMode::process_mouseMoveEvent(QMouseEvent *event) { }

void OVMode::process_enterEvent(QEvent *event) { }

void OVMode::process_leaveEvent(QEvent *event) { }

void OVMode::process_contextMenuEvent(QContextMenuEvent *event) { }

OVModeLayer::OVModeLayer(QWidget *parent) :
    OVCanvasLayer(parent),
    OVContextMenuLayer<OVModeLayer>()
{

}

void OVModeLayer::setMode(OVMode *pMode) {
    m_currentMode = pMode;
}

void OVModeLayer::mousePressEvent(QMouseEvent *event) {
    if (m_currentMode) {
        m_currentMode->process_mousePressEvent(event);
    }
    OVCanvasLayer::mousePressEvent(event);
}

void OVModeLayer::mouseReleaseEvent(QMouseEvent *event) {
    if (m_currentMode) {
        m_currentMode->process_mouseReleaseEvent(event);
    }
    OVCanvasLayer::mouseReleaseEvent(event);
}

void OVModeLayer::mouseMoveEvent(QMouseEvent *event) {
    if (m_currentMode) {
        m_currentMode->process_mouseMoveEvent(event);
    }
    OVCanvasLayer::mouseMoveEvent(event);
}

void OVModeLayer::enterEvent(QEvent *event) {
    if (m_currentMode) {
        m_currentMode->process_enterEvent(event);
    }
    OVCanvasLayer::enterEvent(event);
}

void OVModeLayer::leaveEvent(QEvent *event) {
    if (m_currentMode) {
        m_currentMode->process_leaveEvent(event);
    }
    OVCanvasLayer::leaveEvent(event);
}

void OVModeLayer::contextMenuEvent(QContextMenuEvent *event) {
    if (m_currentMode) {
        m_currentMode->process_contextMenuEvent(event);
    } else {
        OVContextMenuLayer::executeContextMenu(event);
    }
}

} // namespace OVLayers
