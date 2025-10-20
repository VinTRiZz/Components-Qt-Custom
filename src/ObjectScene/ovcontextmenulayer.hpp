#pragma once

#include "ovmeasurementlayer.hpp"

#include <QMenu>

namespace ObjectViewLayers {

class OVContextMenuLayer : public OVMeasurementLayer
{
public:
    explicit OVContextMenuLayer(QWidget* parent = nullptr);

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
    void mouseMoveEvent(QMouseEvent* e) override;

    void contextMenuEvent(QContextMenuEvent* e) override;
};

} // namespace ObjectViewLayers

