#pragma once

#include "basicitem.hpp"

namespace ObjectItems {

class AbstractText : public BasicItem
{
public:
    explicit AbstractText(QGraphicsItem* parent = nullptr);

    void setFont(const QFont& f);
    QFont getFont() const;

    void setTextAlignment(int algn);
    void setTextSizePt(double textSizePt);

private:
    QGraphicsTextItem* m_textItem {nullptr};

protected:
    QGraphicsTextItem* getTextItem() const;
};

} // namespace ObjectItems

