#pragma once

#include <QMouseEvent>

#include "ovcanvaslayer.hpp"
#include "ovcontextmenulayer.hpp"

namespace OVLayers {

class OVMode {
    OVCanvasLayer* m_pHostView;
public:
    OVMode(OVCanvasLayer* pHostView);

protected:
    OVCanvasLayer* getHostView() const;

    virtual void process_mousePressEvent(QMouseEvent *event);
    virtual void process_mouseReleaseEvent(QMouseEvent *event);
    virtual void process_mouseMoveEvent(QMouseEvent *event);

    virtual void process_enterEvent(QEvent *event);
    virtual void process_leaveEvent(QEvent *event);

    virtual void process_contextMenuEvent(QContextMenuEvent *event);

    friend class OVModeLayer;
};

/**
 * @brief The OVModeLayer class Декоратор для работы с режимами
 */
class OVModeLayer :
        public OVCanvasLayer,
        public OVContextMenuLayer<OVModeLayer>
{
public:
    explicit OVModeLayer(QWidget* parent = nullptr);

    /**
     * @brief setMode Не удаляет режим после задания
     * @param pMode
     */
    void setMode(OVMode* pMode);

private:
    OVMode* m_currentMode {nullptr};

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);

    virtual void contextMenuEvent(QContextMenuEvent *event);
};

} // namespace OVLayers

