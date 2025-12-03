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

        m_mainContextMenu.addAction("Вернуться в центр", [this](){
            auto pThis = static_cast<BaseView*>(this);
            pThis->resetScale();
            pThis->centerOn(pThis->getCanvas()->boundingRect().center());
        });
    }

    void addSubmenu(const QString& text, const std::function<bool(QGraphicsItem*)>& activator) {
        auto pMenu = new QMenu(text, static_cast<BaseView*>(this));
        m_mainContextMenu.addMenu(pMenu);
    }

private:
    QMenu m_mainContextMenu;  //! Основное контекстное меню

    QMenu* m_subContextMenu; //! Меню с названием "Контекст", которое вызывается для айтемов под курсором

    std::map<QMenu*, std::function<bool(QGraphicsItem*)> > m_submenus; //! Меню, добавленные через addSubmenu

protected:
    void executeContextMenu(QContextMenuEvent* e) {
        auto pHoverItem = static_cast<BaseView*>(this)->getTopItem(e->pos());

        auto pHoverItemObject = dynamic_cast<ObjectItems::BasicItem*>(pHoverItem);
        m_subContextMenu->setEnabled(pHoverItemObject != nullptr);
        if (m_subContextMenu->isEnabled()) {
            m_subContextMenu->clear();
            auto pMenu = pHoverItemObject->createContextMenu();
            m_subContextMenu->addMenu(pMenu);
        }

        for (auto& [pMenu, activator] : m_submenus) {
            pMenu->setEnabled(activator(pHoverItem));
        }

        // Подразумевается, что это меню не было определено
        m_mainContextMenu.exec(e->globalPos());
    }
};

} // namespace OVLayers
