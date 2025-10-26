#pragma once

#include "lineitem.hpp"

namespace ObjectItems {

class ElegantArrowLine : public LineItem
{
public:
    ElegantArrowLine(QGraphicsItem* parent = nullptr);

    void setArrowSize(qreal size);
    qreal getArrowSize() const;

    void setGradient1Color(const QColor& penColor);
    void setGradient2Color(const QColor& penColor);

    void setSelectionColor(const QColor& penColor) override;

    QRectF boundingRect() const override;
    bool contains(const QPointF& p) const override;
    QPainterPath shape() const override;

    void setLine(const QLineF& line) override;

    void setWeight(double w);
    double getWeight() const;

    void setPositionFrom(const QPointF& posFrom) override;
    void setPositionTo(const QPointF& posTo) override;

private:
    QLinearGradient m_penGradient;

    QRectF m_boundingRect;

    QGraphicsPathItem* m_line{nullptr};
    QGraphicsPathItem* m_lineSelected{nullptr};
    QGraphicsPolygonItem* m_pArrowHeadPolygon{nullptr};

    bool m_prevSelectedState{false};
    qreal m_arrowSize{8};

    double m_weight{0};

    void updatePolygon();
    QPainterPath createLinePath();
    QPolygonF createPolygon(const QLineF& line);
    void updatePen();

protected:
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant& value) override;
};

}
