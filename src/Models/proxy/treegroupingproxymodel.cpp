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
    ItemMetadata() = default;
    ItemMetadata(const QPersistentModelIndex& idx) : m_itemRef(idx) {}

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
    auto rowNodeGroups = getBranchGroups(getGroup(idx.row()));

    // Setup node
    auto rowNode = std::make_shared<Node_t::item_type>();
    ItemMetadata mdata {};
    mdata.m_itemRef = QPersistentModelIndex(idx);
    if (rowNodeGroups.empty()) { // No upper group detected
        rowNode->setData(mdata);
        return;
    }
    mdata.m_selfKey = rowNodeGroups.front();
    rowNode->setData(mdata);

    // Search and create group if not exist
    std::function<Node_t::ptr_type(const Node_t::ptr_type&, const std::vector<GroupKey_t>&, int)>
        getMergableNode =
        [this, &getMergableNode](const Node_t::ptr_type& pParentNode,
               const std::vector<GroupKey_t>& nodeGroups,
               int compareGroupNo) -> Node_t::ptr_type {
        if (compareGroupNo == 0) {
            return pParentNode;
        }

        // Check subnodes if can merge
        for (int rootRow = 0; rootRow < pParentNode->getNodeCount(); ++rootRow) {
            auto pNode = pParentNode->getNode(rootRow);
            if (!canMergeGroups(
                    pNode->getData().m_selfKey,
                    nodeGroups[compareGroupNo])) {
                continue;
            }
            return getMergableNode(pNode, nodeGroups, --compareGroupNo);
        }
        return {};
    };
    auto pMergableNode = getMergableNode(d->m_invisibleRootNode, rowNodeGroups, rowNodeGroups.size() - 1);
    if (pMergableNode) {
        pMergableNode->addNode(rowNode);
        return;
    }

    // Create branch
    auto pCurrentNode = rowNode;
    Node_t::ptr_type maxNode;
    rowNodeGroups.erase(rowNodeGroups.begin());
    for (auto& gKey : rowNodeGroups) {
        auto parentNode = std::make_shared<Node_t::item_type>();
        ItemMetadata mdata {};
        mdata.m_selfKey = gKey;
        parentNode->setData(std::move(mdata));
        pCurrentNode->setParent(parentNode);
        pCurrentNode = parentNode;

        // To add into root node
        maxNode = pCurrentNode;
    }
    d->m_invisibleRootNode->addNode(maxNode);
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