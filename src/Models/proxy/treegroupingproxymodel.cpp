#include "treegroupingproxymodel.hpp"

#include <Components/ExtraClasses/Containers/TreeObject.h>
#include <Components/ExtraClasses/Containers/CommitableObject.h>

#include <QPersistentModelIndex>

#include <Components/Logger/Logger.h>

namespace QtCustom::Models
{

/**
 * @brief The ItemMetadata class Holds metadata of a node to handle tree
 */
struct TreeGroupingProxyModel::ItemMetadata
{
    TreeGroupingProxyModel::GroupKey_t m_selfKey;
    QPersistentModelIndex m_itemRef;
};

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
    Node_t* pTargetNode =
        (parent.isValid() && (nullptr != parent.internalPointer())) ?
            static_cast<Node_t*>(parent.internalPointer()) :
            d->m_invisibleRootNode.get();
    if (row < 0 || row >= pTargetNode->getNodeCount()) {
        return {};
    }
    return toModelIndex(pTargetNode->getNode(row), column);
}

QModelIndex TreeGroupingProxyModel::parent(const QModelIndex &child) const
{
    if (!child.isValid()) {
        return {};
    }
    auto pTargetNode = static_cast<Node_t*>(child.internalPointer());
    if (nullptr == pTargetNode->getParent()) {
        return {};
    }
    return toModelIndex(pTargetNode->getParent());
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
    auto mappedIndex = mapToSource(index);
    if (mappedIndex.isValid()) {
        auto idx = d->m_sourceModel->sibling(mappedIndex.row(), index.column(), {});
        return idx.data(role);
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
        auto targetIdx = itemRef.sibling(itemRef.row(), index.column());
        auto isDataChanged = d->m_sourceModel->setData(targetIdx, value, role);
        if (isDataChanged) {
            emit dataChanged(index, index, {role});
        }
        return isDataChanged;
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
    auto mappedIdx = mapToSource(index);
    if (mappedIdx.isValid()) {
        auto targetIdx = mappedIdx.sibling(mappedIdx.row(), index.column());
        return d->m_sourceModel->flags(targetIdx);
    }
    return QAbstractItemModel::flags(index);
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
    if ((nullptr == pTargetNode) || (0 != pTargetNode->getNodeCount())) {
        return {};
    }
    if (pTargetNode->getData().m_itemRef.isValid()) {
        return pTargetNode->getData().m_itemRef;
    }
    return {};
}

QModelIndex TreeGroupingProxyModel::mapFromSource(const QModelIndex &idx) const
{
    Node_t::ptr_type pTargetNode;
    d->m_invisibleRootNode->callRecursive([&idx, &pTargetNode](auto pNode) -> bool {
        if (0 != pNode->getNodeCount() || !pNode->getData().m_itemRef.isValid()) {
            return false;
        }
        auto isTargetNode = idx == pNode->getData().m_itemRef;
        if (isTargetNode) {
            pTargetNode = pNode;
        }
        return isTargetNode;
    });
    if (pTargetNode) {
        return toModelIndex(pTargetNode, idx.column());
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
    d->m_invisibleRootNode->callRecursive([&](auto& pNode) -> bool {
        auto& nodeV = pNode->getData().m_itemRef;
        if (!nodeV.isValid() || (nodeV != idx)) {
            return false;
        }
        auto pParent = pNode->getParent();
        while ((pParent->getNodeCount() == 1) && pParent->getParent()) {
            pParent = pParent->getParent();
        }
        int nodeRow {};
        if (pParent->getNodeCount() == 1) {
            if (pParent->getParent()) {
                nodeRow = pParent->getParent()->getNodeRow(pParent);
            } else {
                nodeRow = d->m_invisibleRootNode->getNodeRow(pParent);
            }
            beginRemoveRows(toModelIndex(pParent, 0), nodeRow, nodeRow);
            pParent->setParent(nullptr); // Removes node actually
        } else {
            nodeRow = pNode->getParent()->getNodeRow(pParent);
            beginRemoveRows(toModelIndex(pNode, 0), nodeRow, nodeRow);
            pNode->setParent(nullptr); // Removes node actually
        }
        endRemoveRows();
        return true;
    });
}

QModelIndex TreeGroupingProxyModel::toModelIndex(const std::shared_ptr<Node_t> &pNode, int column) const
{
    auto pParent = (pNode->getParent() ? pNode->getParent() : d->m_invisibleRootNode);
    return createIndex(pParent->getNodeRow(pNode), column, pNode.get());
}

}