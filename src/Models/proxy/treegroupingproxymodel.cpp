#include "treegroupingproxymodel.hpp"

#include <Components/ExtraClasses/Containers/TreeObject.h>
#include <Components/ExtraClasses/Containers/CommitableObject.h>

#include <QPersistentModelIndex>

#include <functional>

namespace QtCustom::Models
{

namespace {

/**
 * @brief The ItemMetadata class Holds metadata of a node to handle tree
 */
struct ItemMetadata
{
    TreeGroupingProxyModel::GroupKey_t m_selfKey;
    QPersistentModelIndex m_itemRef;
};
using Node_t = ExtraClasses::TreeNode<ItemMetadata>;
}

struct TreeGroupingProxyModel::Impl
{
    Node_t::ptr_type    m_invisibleRootNode;
    QAbstractItemModel* m_sourceModel {nullptr};
};


TreeGroupingProxyModel::TreeGroupingProxyModel(QObject *parent)
    : QAbstractItemModel{parent},
    d {new Impl}
{
    d->m_invisibleRootNode = std::make_shared<Node_t>();
}

TreeGroupingProxyModel::~TreeGroupingProxyModel()
{

}

QModelIndex TreeGroupingProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        if (row < 0 || row >= d->m_invisibleRootNode->getNodeCount()) {
            return {};
        }
        return createIndex(row, column, d->m_invisibleRootNode->getNode(row).get());
    }
    auto pTargetNode = static_cast<Node_t*>(parent.internalPointer());
    if (nullptr == pTargetNode) {
        return {};
    }
    if (row < 0 || row >= pTargetNode->getNodeCount()) {
        return {};
    }
    return createIndex(row, column, pTargetNode->getNode(row).get());
}

QModelIndex TreeGroupingProxyModel::parent(const QModelIndex &child) const
{
    if (!child.isValid()) {
        return {};
    }
    auto pTargetNode = static_cast<Node_t*>(child.internalPointer());
    if (nullptr == pTargetNode ||
        nullptr == pTargetNode->getParent()) {
        return {};
    }
    auto pParent = pTargetNode->getParent();
    std::size_t parentRow {};
    if (pParent->getParent()) {
        parentRow = pParent->getParent()->getNodeRow(pParent);
    } else {
        parentRow = d->m_invisibleRootNode->getNodeRow(pParent);
    }
    return createIndex(parentRow, 0, pTargetNode->getParent().get());
}

int TreeGroupingProxyModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return d->m_invisibleRootNode->getNodeCount();
    }
    auto pTargetNode = static_cast<Node_t*>(parent.internalPointer());
    if (nullptr == pTargetNode) {
        return {};
    }
    return pTargetNode->getNodeCount();
}

int TreeGroupingProxyModel::columnCount(const QModelIndex &parent) const
{
    return d->m_sourceModel->columnCount();
}

QVariant TreeGroupingProxyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }
    auto pTargetNode = static_cast<Node_t*>(index.internalPointer());
    if (nullptr == pTargetNode) {
        return {};
    }
    auto& itemRef = pTargetNode->getData().m_itemRef;
    if (itemRef.isValid()) {
        auto idx = itemRef.model()->sibling(itemRef.row(), index.column(), {});
        return itemRef.model()->data(idx, role);
    }
    return getGroupData(pTargetNode->getData().m_selfKey, index.column(), role);
}

bool TreeGroupingProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) {
        return {};
    }
    auto pTargetNode = static_cast<Node_t*>(index.internalPointer());
    if (nullptr == pTargetNode) {
        return {};
    }
    auto& itemRef = pTargetNode->getData().m_itemRef;
    if (itemRef.isValid()) {
        return d->m_sourceModel->setData(itemRef, value, role);
    }
    return setGroupData(pTargetNode->getData().m_selfKey, index.column(), value, role);
}

QVariant TreeGroupingProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (!d->m_sourceModel) {
        return {};
    }
    return d->m_sourceModel->headerData(section, orientation, role);
}

Qt::ItemFlags TreeGroupingProxyModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return {};
    }
    auto pTargetNode = static_cast<Node_t*>(index.internalPointer());
    if (nullptr == pTargetNode) {
        return {};
    }
    auto mappedIdx = pTargetNode->getData().m_itemRef;
    if (!mappedIdx.isValid()) {
        return QAbstractItemModel::flags(index);
    }
    return d->m_sourceModel->flags(mappedIdx);
}

void TreeGroupingProxyModel::setSourceModel(QAbstractItemModel *pModel)
{
    beginResetModel();
    d->m_sourceModel = pModel;
    d->m_invisibleRootNode->clearNodes();
    if (d->m_sourceModel) {
        for (int row = 0; row < d->m_sourceModel->rowCount(); ++row) {
            addNode(pModel->index(row, 0));
        }
    }
    endResetModel();
}

QAbstractItemModel *TreeGroupingProxyModel::sourceModel() const
{
    return d->m_sourceModel;
}

QModelIndex TreeGroupingProxyModel::mapToSource(const QModelIndex &idx) const
{
    auto pTargetNode = static_cast<Node_t*>(idx.internalPointer());
    if (nullptr == pTargetNode ||
        0 != pTargetNode->getNodeCount()) {
        return {};
    }
    return pTargetNode->getData().m_itemRef;
}

QModelIndex TreeGroupingProxyModel::mapFromSource(const QModelIndex &idx) const
{
    Node_t::ptr_type pTargetNode;
    d->m_invisibleRootNode->callRecursive([&idx, &pTargetNode](auto pNode) -> bool {
        if (0 != pNode->getNodeCount()) {
            return false;
        }
        auto isTargetNode = idx == pNode->getData().m_itemRef;
        if (isTargetNode) {
            pTargetNode = pNode;
        }
        return isTargetNode;
    });
    if (pTargetNode) {
        auto pParent = pTargetNode->getParent();
        if (!pParent) {
            return createIndex(d->m_invisibleRootNode->getNodeRow(pTargetNode), 0, pTargetNode.get());
        }
        return createIndex(pParent->getNodeRow(pTargetNode), 0, pTargetNode.get());
    }
    return {};
}

TreeGroupingProxyModel::GroupKey_t TreeGroupingProxyModel::getGroup(int sourceModelRow) const
{
    auto sourceColCount = d->m_sourceModel->columnCount();
    if (sourceColCount < 2) {
        return {};
    }
    auto rowIndex = d->m_sourceModel->index(sourceModelRow, d->m_sourceModel->columnCount() - 2);
    if (!rowIndex.isValid()) {
        return {};
    }
    return qHash(QPersistentModelIndex(rowIndex), qGlobalQHashSeed());
}

TreeGroupingProxyModel::GroupKey_t TreeGroupingProxyModel::getParentGroup(GroupKey_t groupKey) const
{
    return {};
}

QVariant TreeGroupingProxyModel::getGroupData(GroupKey_t groupKey, int column, int role) const
{
    return {};
}

bool TreeGroupingProxyModel::canMergeGroups(const GroupKey_t &lgk, const GroupKey_t &rgk) const
{
    return false;
}

bool TreeGroupingProxyModel::setGroupData(GroupKey_t groupKey, int column, const QVariant &value, int role)
{
    return false;
}

void TreeGroupingProxyModel::addNode(const QModelIndex &idx)
{
    // Setup node
    auto rowNode = std::make_shared<Node_t::item_type>();
    ItemMetadata mdata {};
    mdata.m_itemRef = QPersistentModelIndex(idx);
    mdata.m_selfKey = getGroup(idx.row());
    if (!mdata.m_itemRef.isValid()) { // No mergables found
        return;
    }
    rowNode->setData(std::move(mdata));

    // Get groups
    auto getBranchGroups = [this](GroupKey_t rowNodeGroup) -> std::vector<GroupKey_t> {
        std::vector<GroupKey_t> branchGroups;
        uint8_t overflowProtector {};
        while (rowNodeGroup.isValid()) {
            ++overflowProtector;
            if (overflowProtector > 100) {
                throw std::runtime_error("Group detect infinity recursion"); // According to model using experience
            }

            branchGroups.push_back(rowNodeGroup);
            rowNodeGroup = getParentGroup(rowNodeGroup);
        }
        return branchGroups;
    };
    auto rowNodeGroups = getBranchGroups(getParentGroup(rowNode->getData().m_selfKey));

    // Go down and create branch if need
    auto pCurrentNode = d->m_invisibleRootNode;
    std::reverse(rowNodeGroups.data(), rowNodeGroups.data() + rowNodeGroups.size());
    for (auto& gKey : rowNodeGroups) {
        bool foundMergable {false};
        for (int r = 0; r < pCurrentNode->getNodeCount(); ++r) {
            auto pNode = pCurrentNode->getNode(r);
            foundMergable = canMergeGroups(gKey, pNode->getData().m_selfKey);
            if (foundMergable) {
                pCurrentNode = pNode;
                break;
            }
        }
        if (foundMergable) {
            continue;
        }
        auto pSubnode = std::make_shared<Node_t>();
        ItemMetadata mdata {};
        mdata.m_selfKey = gKey;
        pSubnode->setData(std::move(mdata));
        pSubnode->setParent(pCurrentNode);
        pCurrentNode = pSubnode;
    }
    rowNode->setParent(pCurrentNode);
}

void TreeGroupingProxyModel::removeNode(const QModelIndex &idx)
{
    d->m_invisibleRootNode->callRecursive([&idx](auto& pNode) -> bool {
        auto& nodeV = pNode->getData().m_itemRef;
        if (!nodeV.isValid() || (nodeV != idx)) {
            return false;
        }
        pNode->setParent(nullptr); // Removes node actually
        return true;
    });
}

}