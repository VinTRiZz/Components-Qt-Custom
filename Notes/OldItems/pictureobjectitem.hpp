#ifndef PICTUREOBJECTITEM_H
#define PICTUREOBJECTITEM_H

#include <QGraphicsItem>
#include <QPen>
#include <set>

#include "itembase.hpp"

namespace ObjectItems {

class PictureObjectItem : public ItemBase {
public:
    explicit PictureObjectItem(QGraphicsItem* parent = nullptr);

    void setImage(const QImage& img);

    void setBorderColor(const QColor& penColor) override;
    void setBackgroundColor(const QColor& penColor) override;
    void setSelectionColor(const QColor& penColor) override;

    QImage getImage() const;
    QRectF getImageRect() const;

    void setRect(const QRectF& iRect);

    QPainterPath shape() const override;

private:
    QGraphicsPathItem* m_selectedRectItem{nullptr};
    QGraphicsPixmapItem* m_vertexImage{nullptr};
    QGraphicsEllipseItem* m_vertexEllipse{nullptr};

protected:
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant& value) override;
};

}  // namespace ObjectItems

#endif  // PICTUREOBJECTITEM_H
