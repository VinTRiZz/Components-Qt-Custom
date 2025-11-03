#pragma once

#include "groupitem.hpp"

namespace ObjectItems {

class CommentItem : public GroupItem
{
    Q_OBJECT
public:
    explicit CommentItem(QGraphicsItem* parent = nullptr);
};

} // namespace ObjectItems

