#pragma once

#include "ovitemgeometrylayer.hpp"

#include <QLabel>

#include <Components/CustomQt/ObjectScene/LabelItem.h>

namespace ObjectViewLayers {

class OVInformationLayer : public OVItemGeometryLayer
{
public:
    explicit OVInformationLayer(QWidget* parent = nullptr);

    ObjectViewItems::LabelItem* getCursorLabel() const;

    void setCursorValuesPresenter(const std::function<QString(const QPointF&)>& pres);

private:
    ObjectViewItems::LabelItem* m_pCursorLabel{
        nullptr};  //! Объект, который показывает координаты пользователю

    QLabel* m_pInformationLabel{nullptr};

    std::function<QString(const QPointF&)> m_cursorValuesPresenter;

private slots:
    void updateCursorLabel();

protected:
    void wheelEvent(QWheelEvent* e) override;

    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

    void enterEvent(QEvent* e) override;
    void leaveEvent(QEvent* e) override;
};

} // namespace ObjectViewLayers

