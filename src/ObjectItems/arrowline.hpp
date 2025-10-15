#pragma once

#include <QGraphicsLineItem>
#include <QPen>

#include "itembase.hpp"
#include "constants.hpp"

namespace ObjectViewItems {

class ArrowedLine : public ItemBase {
public:
    explicit ArrowedLine(QGraphicsItem* parent = nullptr);

    void setPen(const QPen& drawPen);
    void setSelectedPen(const QPen& drawPen);

    void setLine(const QLineF& iLine);
    void setLine(const QPointF& p1, const QPointF& p2);
    QLineF getLine() const;

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

}  // namespace ObjectViewItems
