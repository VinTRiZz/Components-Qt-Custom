#pragma once

#include "basicitem.hpp"

#include <QPen>

namespace ObjectItems {

/**
 * @brief The LineDirectionType enum Тип направления линии соединения
 */
enum LineDirectionType : short {
    None = 0,
    Forward = 0b01,
    Backward = 0b10,
    Bidirectional = Forward | Backward,
};

/**
 * @brief The LineAngleType enum Угол между сторонами треугольника стрелки
 */
enum LineAngleType : short {
    A_90,
    A_75,
    A_60,
    A_45,
    A_30,
    A_15,
};

class AbstractConnectionLine : public BasicItem
{
public:
    explicit AbstractConnectionLine(QGraphicsItem *parent = nullptr);

    void setDirection(LineDirectionType arrType);
    LineDirectionType getDirection() const;

    void setArrowHeight(double arHeight);
    QSizeF getArrowSize() const;

    void setLine(const QLineF& line);
    void setLine(const QPointF& p1, const QPointF& p2);
    QLineF getLine() const;

    void setPositionFrom(const QPointF& posFrom);
    QPointF getPositionFrom() const;

    void setPositionTo(const QPointF& posTo);
    QPointF getPositionTo() const;

    void setStyle(Qt::PenStyle pst);
    void setWidth(double w);

private:
    QLineF              m_straightLine  {};
    LineDirectionType   m_arrowType     {LineDirectionType::None};
    LineAngleType       m_arrowAngle    {LineAngleType::A_30};
    QSizeF              m_arrowSize     {10, 10};
    QPen                m_stylePen      {Qt::black, 1, Qt::SolidLine, Qt::RoundCap};

protected:
    QPainterPath createArrowPath() const; // Направлена вверх после создания
};

} // namespace ObjectItems

