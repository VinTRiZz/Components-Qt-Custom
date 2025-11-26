#pragma once

#include <QMenu>
#include <QGraphicsItem>

#include "ovcontextmenulayer.hpp"
#include "ovinternalscene.hpp"

#include "../ObjectItems/basicitem.hpp"

#include <QContextMenuEvent>

#include <QAction>

namespace OVLayers {

template <typename BaseView>
class OVContextMenuLayer
{
public:
    OVContextMenuLayer() {
        m_subContextMenu = new QMenu("Объект", static_cast<BaseView*>(this));
        m_mainContextMenu.addMenu(m_subContextMenu);
        m_subContextMenu->setEnabled(false);

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
            pThis->resetScale();
            pThis->centerOn(pThis->getCanvas()->boundingRect().center());
        });
    }

    void addContextMenuAction(const QString& text, const std::function<void(QGraphicsItem*)>& processor) {
        auto pAction = new QAction(text);
        connect(pAction, &QAction::triggered,
                pAction, [this, processor](){
            processor(m_contextMenuItem);
        });
        m_subContextMenu->addAction(pAction);
    }

    void addSubmenu(const QString& text, const std::function<bool(QGraphicsItem*)>& activator) {
        auto pMenu = new QMenu(text, static_cast<BaseView*>(this));
        m_mainContextMenu.addMenu(pMenu);
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

    QMenu* m_subContextMenu; //! Меню с названием "Контекст", которое вызывается для айтемов под курсором

    std::map<QMenu*, std::function<bool(QGraphicsItem*)> > m_submenus; //! Меню, добавленные через addSubmenu

protected:
    void executeContextMenu(QContextMenuEvent* e) {
        auto pHoverItem = static_cast<BaseView*>(this)->getTopItem(e->pos());
        if (nullptr != pHoverItem) {
            m_opacityAction->setEnabled(true);
            m_opacityAction->setChecked(pHoverItem->opacity() < 0.9);
        } else {
            m_opacityAction->setEnabled(false);
        }
        m_contextMenuItem = pHoverItem;
        m_subContextMenu->setEnabled(dynamic_cast<ObjectItems::BasicItem*>(m_contextMenuItem) != nullptr);

        for (auto& [pMenu, activator] : m_submenus) {
            pMenu->setEnabled(activator(m_contextMenuItem));
        }

        // Подразумевается, что это меню не было определено
        m_mainContextMenu.exec(e->globalPos());

        // Во избежание подвешенных состояний
        m_contextMenuItem = nullptr;
    }
};

} // namespace OVLayers
