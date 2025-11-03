#include "commentaryitem.hpp"

namespace ObjectItems {

CommentItem::CommentItem(QGraphicsItem *parent) :
    GroupItem(parent)
{
    setSystemName("Commentary");

    auto defaultCommentColor = QColor(130, 220, 90);
    auto linesPen = QPen(defaultCommentColor, 5, Qt::DashDotLine, Qt::RoundCap);
    linesPen.setCosmetic(true);
    setLinePen(linesPen);

    auto bgrBrush = QBrush(defaultCommentColor, Qt::DiagCrossPattern);
    setBackgroundBrush(bgrBrush);
}

} // namespace ObjectItems

