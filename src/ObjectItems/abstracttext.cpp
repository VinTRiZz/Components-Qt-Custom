#include "abstracttext.hpp"

#include <QFont>

#include <QTextOption>
#include <QTextDocument>

namespace ObjectItems {

AbstractText::AbstractText(QGraphicsItem* parent) :
    BasicItem(parent)
{
    setObjectName("Text");
    setObjectType(ObjectType::OIT_AbstractText);

    createSubitem(m_textItem);
    m_textItem->setZValue(100); // Чтобы не перекрывали в будущем

    connect(this, &BasicItem::displayNameChanged,
            this, [this](){
        m_textItem->setPlainText(getDisplayName());
    });

    connect(this, &BasicItem::graphicalDataChanged,
            this, [this](){
        m_textItem->setDefaultTextColor(getLinePen().color());
    });

    // Шрифты
    auto font = m_textItem->font();
    font.setPixelSize(14);
    font.setStyle(QFont::StyleItalic);
    setFont(font);
    setTextAlignment(Qt::AlignCenter);

    setLinePen({Qt::black});
    m_textItem->setTextWidth(100);
}

void AbstractText::setFont(const QFont &f)
{
    m_textItem->setFont(f);
}

QFont AbstractText::getFont() const
{
    return m_textItem->font();
}

void AbstractText::setMaxSymbolCount(int maxSymbolCount)
{
    m_textItem->setTextWidth(maxSymbolCount);
}

void AbstractText::setTextAlignment(int algn)
{
    auto doc = m_textItem->document();
    auto option = doc->defaultTextOption();
    option.setAlignment(Qt::AlignmentFlag(algn));
    doc->setDefaultTextOption(option);
}

void AbstractText::setTextSizePt(double textSizePt)
{
    auto font = getFont();
    font.setPointSizeF(textSizePt);
    setFont(font);
}

QGraphicsTextItem *AbstractText::getTextItem() const
{
    return m_textItem;
}

} // namespace ObjectItems
