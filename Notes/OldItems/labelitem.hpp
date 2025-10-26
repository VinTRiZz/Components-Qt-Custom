#ifndef LABELITEM_H
#define LABELITEM_H

#include <QFont>

#include "itembase.hpp"

namespace ObjectItems {

class LabelItem : public ItemBase {
public:
    explicit LabelItem(QGraphicsItem* parent = nullptr);

    void setDisplayName(const QString& iText) override;

    void setBorderColor(const QColor& iColor) override;
    void setBackgroundColor(const QColor& iColor) override;

    void setTextStyle(QFont::Style fStyle);
    void setTextSize(int pixelSize);

    QRectF boundingRect() const override;

private:
    QGraphicsRectItem* m_vertexTextRect{nullptr};
    QGraphicsTextItem* m_vertexText{nullptr};
};

}  // namespace ObjectItems

#endif  // LABELITEM_H
