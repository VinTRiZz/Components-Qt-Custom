#include "ovcontextmenulayer.hpp"

#include <QContextMenuEvent>

#include <QAction>

#include "ovinternalscene.hpp"

namespace ObjectViewLayers {

OVContextMenuLayer::OVContextMenuLayer(QWidget *parent) :
    OVMeasurementLayer(parent)
{
    auto pGridAction = m_mainContextMenu.addAction("Сетка", [this]() {
        getScene()->setGridEnabled(!getScene()->getIsGridEnabled());
        for (auto* pAction : m_mainContextMenu.actions()) {
            if (pAction->text() == "Сетка") {
                pAction->setChecked(getScene()->getIsGridEnabled());
                break;
            }
        }
    });
    pGridAction->setCheckable(true);

    m_opacityAction = m_mainContextMenu.addAction("Прозрачный", [this]() {
        auto currentCursorPos = mapFromGlobal(m_mainContextMenu.pos());
        auto pTargetItem = getTopItem(currentCursorPos);
        if (pTargetItem == nullptr) {
            return;
        }
        auto currentOpacity = pTargetItem->opacity();
        if (currentOpacity < 0.9) {
            pTargetItem->setOpacity(1);
        } else {
            pTargetItem->setOpacity(0.2);
        }
        m_opacityAction->setChecked(currentOpacity > 0.9);
    });
    m_opacityAction->setCheckable(true);
    m_opacityAction->setEnabled(false);
}

void OVContextMenuLayer::mouseMoveEvent(QMouseEvent *e)
{
    OVMeasurementLayer::mouseMoveEvent(e);

    auto pHoverItem = getTopItem(e->pos());
    if (nullptr != pHoverItem) {
        m_opacityAction->setEnabled(true);
        m_opacityAction->setChecked(pHoverItem->opacity() < 0.9);
    } else {
        m_opacityAction->setEnabled(false);
    }
}

void OVContextMenuLayer::contextMenuEvent(QContextMenuEvent *e)
{
    // Подразумевается, что это меню не было определено
    m_mainContextMenu.exec(e->globalPos());
}

} // namespace ObjectViewLayers
