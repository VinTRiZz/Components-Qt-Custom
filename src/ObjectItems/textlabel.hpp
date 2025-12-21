#pragma once

#include "abstracttext.hpp"

namespace ObjectItems {

class TextLabel : public AbstractText
{
    OBJECTITEMS_ITEM
public:
    explicit TextLabel(QGraphicsItem* parent = nullptr);

    void setContrastRectVisible(bool isCREctVisible = true);

private:
    QGraphicsRectItem* m_contrastRect {nullptr};
};

} // namespace ObjectItems

OBJECTITEMS_REGISTER_ITEM(
        ObjectItems::TextLabel,
        ObjectItems::AbstractText);
