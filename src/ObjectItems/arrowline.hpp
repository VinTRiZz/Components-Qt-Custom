#pragma once

#include <QGraphicsLineItem>
#include <QPen>

#include "lineitem.hpp"
#include "constants.hpp"

namespace ObjectItems {

class ArrowLine : public LineItem {
public:
    explicit ArrowLine(QGraphicsItem* parent = nullptr);

    void setPen(const QPen& drawPen);
    void setSelectedPen(const QPen& drawPen);

    void setLine(const QLineF& iLine) override;

    void setArrowType(LineArrowType t);

    /**
     * @brief setArrowSize  Задать размер стрелки
     * @param arrowSize     Размеры стрелки (стрелка квадратная)
     */
    void setArrowSize(qreal arrowSize);

    /**
     * @brief getArrowSize  Получить размер стрелки
     * @return              Размер полигона стрелки (стрелка квадратная)
     */
    qreal getArrowSize() const;

private:
    QPainterPath createArrow() const; // Направлена вертикально вверх после создания
    void updateArrows();

    LineArrowType m_arrowType {LineArrowType::Forward};

    QGraphicsLineItem* m_line{nullptr};
    QGraphicsPathItem* m_forwardArrow {nullptr};
    QGraphicsPathItem* m_backwardArrow {nullptr};

    qreal m_arrowSize{10};  // Размер стрелки
    QPen m_drawPen;
    QPen m_drawSelectedPen;

protected:
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant& value) override;
};

}  // namespace ObjectItems
