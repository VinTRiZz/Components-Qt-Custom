#pragma once

#include "groupitem.hpp"
#include "textlabel.hpp"

namespace ObjectItems {

class CommentItem : public GroupItem
{
    Q_OBJECT
    OBJECTITEMS_ITEM
public:
    explicit CommentItem(QGraphicsItem* parent = nullptr);

private:
    QGraphicsPathItem* m_referencerItem {nullptr};
    TextLabel* m_commentaryText {nullptr};
};

} // namespace ObjectItems

OBJECTITEMS_REGISTER_ITEM_WITH_FIELDS(
        ObjectItems::CommentItem,
        ObjectItems::GroupItem,
        m_commentaryText);
