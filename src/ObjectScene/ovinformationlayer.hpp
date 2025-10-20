#pragma once

#include "ovmeasurementlayer.hpp"

#include <QLabel>

#include <Components/CustomQt/ObjectScene/LabelItem.h>

namespace ObjectViewLayers {

class OVInformationLayer : public OVMeasurementLayer
{
public:
    explicit OVInformationLayer(QWidget* parent = nullptr);

    ObjectViewItems::LabelItem* getCursorLabel() const;
    QLabel* getInformationLabel() const;

    void setCurrentToolname(const QString& toolName);

    // Через аргументы %0..%N отображать информацию
    // TODO: Задокументировать по окончании разработки
    void setInformationFormat(const QString& infoFormat);

    void setCursorValuesPresenter(const std::function<QString(const QPointF&)>& pres);

private:
    ObjectViewItems::LabelItem* m_pCursorLabel{
        nullptr};  //! Объект, который показывает информацию у курсора

    QLabel* m_pInformationLabel{nullptr};
    QString m_currentToolName;
    QString m_currentInfoFormat {"Масштаб: 1:%0   Сетка: %1\nИнструмент: %2"};

    std::function<QString(const QPointF&)> m_cursorValuesPresenter;

private slots:
    void updateCursorLabel();
    void updateInformationLabel();

protected:
    void wheelEvent(QWheelEvent* e) override;

    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

    void enterEvent(QEvent* e) override;
    void leaveEvent(QEvent* e) override;

    void resizeEvent(QResizeEvent* e) override;
};

} // namespace ObjectViewLayers

