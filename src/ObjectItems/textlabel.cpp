#include "textlabel.hpp"

#include <QPen>
#include <QBrush>

namespace ObjectItems {

TextLabel::TextLabel(QGraphicsItem* parent) :
    AbstractText(parent)
{
    createSubitem(m_contrastRect);

    connect(this, &BasicItem::displayNameChanged,
            this, [this](){
        m_contrastRect->setRect(getTextItem()->boundingRect());
    });

    connect(this, &BasicItem::graphicalDataChanged,
            this, [this](){
        m_contrastRect->setPen(getLineColor());
        m_contrastRect->setBrush(getBackgroundColor());
    });
}

void TextLabel::setContrastRectVisible(bool isCREctVisible)
{
    m_contrastRect->setVisible(isCREctVisible);
}

} // namespace ObjectItems
