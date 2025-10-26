#pragma once

#include "basicitem.hpp"

namespace ObjectItems {

class AbstractText : public BasicItem
{
public:
    explicit AbstractText(QGraphicsItem* parent = nullptr);

    void setFont(const QFont& f);
    void setTextAlignment(int algn);

private:
    QGraphicsTextItem* m_textItem {nullptr};

protected:
    QGraphicsTextItem* getTextItem() const;
};

} // namespace ObjectItems

