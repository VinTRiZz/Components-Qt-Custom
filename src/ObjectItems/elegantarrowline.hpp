#pragma once

#include "itembase.hpp"
#include "labelitem.hpp"

namespace ObjectViewItems {

class ElegantArrowLine : public ItemBase
{
public:
    ElegantArrowLine(QGraphicsItem* parent = nullptr);

    void setLine(const QLineF& line);
    void setLine(const QPointF& p1, const QPointF& p2);
    QLineF getLine() const;

    void setPositionFrom(const QPointF& posFrom);
    void setPositionTo(const QPointF& posTo);
    void resetPositions();

    void setArrowSize(qreal size);
    qreal getArrowSize() const;

    void setWeight(double w);
    double getWeight() const;

    void setDisplayName(const QString& iText) override;

    void setGradient1Color(const QColor& penColor);
    void setGradient2Color(const QColor& penColor);

    void setSelectionColor(const QColor& penColor) override;

    QRectF boundingRect() const override;
    bool contains(const QPointF& p) const override;
    QPainterPath shape() const override;

    LabelItem* getLabel() const;

private:
    QLinearGradient m_penGradient;
    QLineF m_straightLine;
    double m_weight{0};

    QRectF m_boundingRect;

    QGraphicsPathItem* m_line{nullptr};
    QGraphicsPathItem* m_lineSelected{nullptr};
    QGraphicsPolygonItem* m_pArrowHeadPolygon{nullptr};
    LabelItem* m_labelItem{nullptr};

    bool m_prevSelectedState{false};
    qreal m_arrowSize{8};

    void updatePolygon();
    QPainterPath createLinePath();
    QPolygonF createPolygon(const QLineF& line);
    void updatePen();

protected:

    QVariant itemChange(GraphicsItemChange change,
                        const QVariant& value) override;
};

}
