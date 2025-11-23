#pragma once

#include "oviteminteractionlayer.hpp"

#include <QLabel>

namespace ObjectItems {
class TextLabel;
}

namespace OVLayers {

class OVInformationLayer : public OVItemInteractionLayer
{
    Q_OBJECT
public:
    explicit OVInformationLayer(QWidget* parent = nullptr);

    ObjectItems::TextLabel* getCursorLabel() const;
    QLabel* getInformationLabel() const;

    void setCurrentToolname(const QString& toolName);

    // Через аргументы %0..%N отображать информацию
    // TODO: Задокументировать по окончании разработки
    void setInformationFormat(const QString& infoFormat);

    void setCursorValuesPresenter(const std::function<QString(const QPointF&)>& pres);

public slots:
    void setCursorLabelEnabled(bool isEn);
    void setInformationLabelEnabled(bool isEn);
    void setHighlightEnabled(bool isEn);

private:

    bool m_isCursorLabelEnabled {true};
    ObjectItems::TextLabel* m_pCursorLabel{
        nullptr};  //! Объект, который показывает информацию у курсора

    bool m_isHighlightEnabled {false};
    QGraphicsPathItem* m_highlightItem {nullptr};

    bool m_isInfoLabelEnabled {true};
    QLabel* m_pInformationLabel{nullptr};
    QString m_currentToolName;
    QString m_currentInfoFormat {"Масштаб: 1:%0   Сетка: %1\nИнструмент: %2"};

    std::function<QString(const QPointF&)> m_cursorValuesPresenter;

private slots:
    void updateCursorLabel();
    void updateInformationLabel();
    void updateHighlight();

protected:
    void mouseMoveEvent(QMouseEvent* e) override;

    void enterEvent(QEvent* e) override;
    void leaveEvent(QEvent* e) override;

    void resizeEvent(QResizeEvent* e) override;
};

} // namespace OVLayers

