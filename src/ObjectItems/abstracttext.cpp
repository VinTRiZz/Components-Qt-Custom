#include "abstracttext.hpp"

#include <QFont>

#include <QTextOption>
#include <QTextDocument>

namespace ObjectItems {

AbstractText::AbstractText(QGraphicsItem* parent) :
    BasicItem(parent)
{
    createSubitem(m_textItem);

    m_textItem->setZValue(100); // Чтобы не перекрывали в будущем

    // Шрифты
    auto font = m_textItem->font();
    font.setPixelSize(14);
    font.setStyle(QFont::StyleItalic);
    setFont(font);
    m_textItem->setDefaultTextColor(Qt::black);

    auto doc = m_textItem->document();
    auto option = doc->defaultTextOption();
    option.setAlignment(Qt::AlignCenter);
    doc->setDefaultTextOption(option);

    m_textItem->setTextWidth(100);

    connect(this, &BasicItem::displayNameChanged,
            this, [this](){
        m_textItem->setPlainText(getDisplayName());
    });

    connect(this, &BasicItem::graphicalDataChanged,
            this, [this](){
        m_textItem->setDefaultTextColor(getLineColor());
    });
}

void AbstractText::setFont(const QFont &f)
{
    m_textItem->setFont(f);
}

QGraphicsTextItem *AbstractText::getTextItem() const
{
    return m_textItem;
}

} // namespace ObjectItems
