#pragma once

#include "itembase.hpp"


namespace ObjectViewItems {

class LineItem : public ItemBase
{
public:
    LineItem(QGraphicsItem* parent = nullptr);

    virtual void setLine(const QLineF& line);
    virtual void setLine(const QPointF& p1, const QPointF& p2);
    QLineF getLine() const;

    virtual void setPositionFrom(const QPointF& posFrom);
    virtual void setPositionTo(const QPointF& posTo);

private:
    QLineF m_straightLine;

    bool m_prevSelectedState{false};

protected:
};

}
