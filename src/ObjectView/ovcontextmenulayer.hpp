#pragma once

#include <QMenu>
#include <QGraphicsItem>

#include "ovcontextmenulayer.hpp"
#include "ovinternalscene.hpp"

#include <QContextMenuEvent>

#include <QAction>

namespace OVLayers {

template <typename BaseView>
class OVContextMenuLayer
{
public:
    OVContextMenuLayer() {
        auto pGridAction = m_mainContextMenu.addAction("Сетка", [this]() {
            static_cast<BaseView*>(this)->getScene()->setGridEnabled(!static_cast<BaseView*>(this)->getScene()->getIsGridEnabled());
            for (auto* pAction : m_mainContextMenu.actions()) {
                if (pAction->text() == "Сетка") {
                    pAction->setChecked(static_cast<BaseView*>(this)->getScene()->getIsGridEnabled());
                    break;
                }
            }
        });
        pGridAction->setCheckable(true);

        m_opacityAction = m_mainContextMenu.addAction("Прозрачный", [this]() {
            auto currentCursorPos = static_cast<BaseView*>(this)->mapFromGlobal(m_mainContextMenu.pos());
            auto pTargetItem = static_cast<BaseView*>(this)->getTopItem(currentCursorPos);
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

        m_mainContextMenu.addAction("Вернуться в центр", [this](){
            auto pThis = static_cast<BaseView*>(this);
            pThis->resetTransform();
            pThis->centerOn(pThis->getCanvas()->boundingRect().center());
        });
    }

private:
    QMenu m_mainContextMenu;  //! Основное контекстное меню
    QAction* m_opacityAction{
        nullptr};  //! Для переключения прозрачности объектов
    QAction* m_contextAction{
        nullptr};  //! Действие с пометкой "Контекст" в контестном меню
    QGraphicsItem* m_contextMenuItem{
        nullptr};  //! Объект, который находился под указателем мыши во время
                   //! вызова контекстного меню

protected:
    void executeContextMenu(QContextMenuEvent* e) {
        auto pHoverItem = static_cast<BaseView*>(this)->getTopItem(e->pos());
        if (nullptr != pHoverItem) {
            m_opacityAction->setEnabled(true);
            m_opacityAction->setChecked(pHoverItem->opacity() < 0.9);
        } else {
            m_opacityAction->setEnabled(false);
        }

        // Подразумевается, что это меню не было определено
        m_mainContextMenu.exec(e->globalPos());
    }
};

} // namespace OVLayers
