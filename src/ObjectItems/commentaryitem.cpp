#include "commentaryitem.hpp"

#include <QPainterPath>

namespace ObjectItems {

CommentItem::CommentItem(QGraphicsItem *parent) :
    GroupItem(parent)
{
    setSystemName("Commentary");

    createSubitem(m_referencerItem);
    connect(this, &BasicItem::graphicalDataChanged,
            this, [this](){
        m_referencerItem->setPen(getLinePen());

        auto currentBRect = getBoundingPolygon().boundingRect();

        const double commentTextOffset {20};

        QPainterPath p;
        p.moveTo(currentBRect.topRight() + QPointF(-commentTextOffset, -commentTextOffset));
        p.lineTo(currentBRect.topRight() + QPointF(commentTextOffset, -commentTextOffset));
        p.lineTo(currentBRect.bottomRight() + QPointF(commentTextOffset, commentTextOffset));
        p.lineTo(currentBRect.bottomRight() + QPointF(-commentTextOffset, commentTextOffset));
        p.moveTo(QPointF(currentBRect.right() + commentTextOffset, currentBRect.center().y()));

        auto curpos = p.currentPosition();
        curpos.setX(curpos.x() + 30);
        p.lineTo(curpos);

        m_referencerItem->setPath(p);
        auto refBRect = m_referencerItem->path().boundingRect();
        m_commentaryText->setPos(QPointF(refBRect.right(), refBRect.center().y() - m_commentaryText->boundingRect().center().y()));
    });

    createSubitem<false>(m_commentaryText);
    connect(this, &BasicItem::displayNameChanged,
            this, [this](){
        m_commentaryText->setDisplayName(getDisplayName());
    });

    auto defaultCommentColor = QColor(70, 180, 80);
    auto linesPen = QPen(defaultCommentColor, 3, Qt::DashDotLine, Qt::RoundCap);
    linesPen.setCosmetic(true);
    setLinePen(linesPen);

    auto bgrBrush = QBrush(defaultCommentColor, Qt::DiagCrossPattern);
    setBackgroundBrush(bgrBrush);
}

} // namespace ObjectItems

