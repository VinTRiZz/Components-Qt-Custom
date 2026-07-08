#include "treegroupingproxymodel.hpp"

#include <Components/ExtraClasses/Containers/TreeObject.h>
#include <Components/ExtraClasses/Containers/CommitableObject.h>
#include <Components/Logger/Logger.h>

#include <QPersistentModelIndex>

#include <unordered_map>

namespace QtCustom::Models
{

/**
 * @brief The ItemMetadata class Holds metadata of a node to handle tree
 */
class TreeGroupingProxyModel::ItemMetadata
{
public:
    TreeGroupingProxyModel::GroupKey_t getGroupKey() const {
        return m_selfKey;
    }
    void setGroupKey(const TreeGroupingProxyModel::GroupKey_t& k) {
        m_selfKey = k;
    }

    bool isSourceIndex() const {
        return std::holds_alternative<QPersistentModelIndex>(m_data);
    }
    void setSourceIndex(const QModelIndex& idx) {
        m_data = idx;
    }
    QPersistentModelIndex getSourceIndex() const {
        return std::get<QPersistentModelIndex>(m_data);
    }

    void setColumnCount(int colCount) {
        if (isSourceIndex()) {
            COMPLOG_WARNING_SYNC("Requested extend column count of invalid node");
            return;
        }
        auto& colsData = getColumns();
        colsData.resize(colCount);
    }
    void setIndexData(const QVariant& value, int col, int role) {
        if (isSourceIndex()) {
            COMPLOG_WARNING_SYNC("Requested set data of invalid or not group node");
            return;
        }
        auto& cols = getColumns();
        if (cols.size() <= col) {
            cols.resize(col + 1);
        }
        cols[col][role] = value;
    }
    QVariant getIndexData(int col, int role) const {
        if (isSourceIndex()) {
            COMPLOG_WARNING_SYNC("Requested get data of not group node");
            return {};
        }
        auto& cols = getColumns();
        if (cols.size() <= col) {
            return {};
        }
        auto& targetCol = cols[col];
        if (targetCol.count(role)) {
            return targetCol.at(role);
        }
        return {};
    }

private:
    const std::vector< std::map<int, QVariant> >& getColumns() const {
        return std::get<std::vector< std::map<int, QVariant> > >(m_data);
    }
    std::vector< std::map<int, QVariant> >& getColumns() {
        return std::get<std::vector< std::map<int, QVariant> > >(m_data);
    }

    TreeGroupingProxyModel::GroupKey_t m_selfKey;
    std::variant<std::vector< std::map<int, QVariant> >, QPersistentModelIndex> m_data;
};

struct TreeGroupingProxyModel::Impl
{
    Node_t::ptr_type    m_invisibleRootNode;
    QAbstractItemModel* m_sourceModel {nullptr};

    std::unordered_map<uint, Node_t::ptr_type> cache_nodes;         // Group hash
    std::unordered_map<uint, Node_t::ptr_type> cache_lowestLayer;   // Persistent index hash
};


TreeGroupingProxyModel::TreeGroupingProxyModel(QObject *parent)
    : QAbstractItemModel{parent},
    d {new Impl}
{
    d->m_invisibleRootNode = d->m_invisibleRootNode->create();
}

TreeGroupingProxyModel::~TreeGroupingProxyModel()
{

}

QModelIndex TreeGroupingProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    auto pTargetNode =
        (parent.isValid() && (nullptr != parent.internalPointer())) ?
            toNode(parent) :
            d->m_invisibleRootNode;
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
    auto pTargetNode = toNode(child);
    if (!pTargetNode || !pTargetNode->getParent()) {
        return {};
    }
    return toModelIndex(pTargetNode->getParent());
}

int TreeGroupingProxyModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return d->m_invisibleRootNode->getNodeCount();
    }
    auto pTargetNode = toNode(parent);
    if (!pTargetNode) {
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
    auto mappedIndex = mapToSource(index);
    if (mappedIndex.isValid()) {
        auto targetIdx = mappedIndex.sibling(mappedIndex.row(), index.column());
        return d->m_sourceModel->data(targetIdx, role);
    }
    auto pNode = toNode(index);
    if (!pNode) { return {}; }
    return getGroupData(pNode->getData().getGroupKey(), index.column(), role);
}

bool TreeGroupingProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) {
        return {};
    }
    auto mappedIndex = mapToSource(index);
    if (mappedIndex.isValid()) {
        auto targetIdx = mappedIndex.sibling(mappedIndex.row(), index.column());
        auto isDataChanged = d->m_sourceModel->setData(targetIdx, value, role);
        if (isDataChanged) {
            emit dataChanged(index, index, {role});
        }
        return isDataChanged;
    }
    return setGroupData(getGroup(index.row()), index.column(), value, role);
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
    if (d->m_sourceModel) {
        disconnect(d->m_sourceModel, nullptr, this, nullptr);
    }
    d->m_sourceModel = pModel;
    resetTree();
    if (d->m_sourceModel) {
        connect(pModel, &QAbstractItemModel::modelAboutToBeReset,
                this, &TreeGroupingProxyModel::resetTree);

        connect(pModel, &QAbstractItemModel::dataChanged,
                this, [this](const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles){
            for (int i = topLeft.row(); i < bottomRight.row() + 1; ++i) {
                auto changedIndex = d->m_sourceModel->index(i, 0, topLeft.parent());
                auto changedIndexSibling = d->m_sourceModel->index(i, d->m_sourceModel->columnCount(topLeft.parent()) - 1, topLeft.parent());
                updateNode(changedIndex);
                emit dataChanged(mapFromSource(changedIndex), mapFromSource(changedIndexSibling), roles);
            }
        });

        connect(pModel, &QAbstractItemModel::rowsInserted,
                this, [this](const QModelIndex &parent, int first, int last){
                    for (int i = first; i < last + 1; ++i) {
                        addNode(d->m_sourceModel->index(i, 0, parent));
                    }
                });

        connect(pModel, &QAbstractItemModel::rowsAboutToBeRemoved,
                this, [this](const QModelIndex &parent, int first, int last){
                    for (int i = first; i < last + 1; ++i) {
                        removeNode(d->m_sourceModel->index(i, 0, parent));
                    }
                });
    }

    endResetModel();
}

QAbstractItemModel *TreeGroupingProxyModel::sourceModel() const
{
    return d->m_sourceModel;
}

QModelIndex TreeGroupingProxyModel::mapToSource(const QModelIndex &idx) const
{
    if (idx.model() != this) {
        COMPLOG_WARNING_SYNC("Invalid index for mapToSource passed into TreeGroupingProxyModel");
        return {};
    }
    auto pTargetNode = toNode(idx.column() == 0 ? idx : idx.siblingAtColumn(0));
    if (pTargetNode && pTargetNode->getData().isSourceIndex()) {
        auto sourceIdx = pTargetNode->getData().getSourceIndex();
        return (idx.column() == 0 ? QModelIndex(sourceIdx) : d->m_sourceModel->sibling(sourceIdx.row(), idx.column(), sourceIdx));
    }
    return {};
}

QModelIndex TreeGroupingProxyModel::mapFromSource(const QModelIndex &idx) const
{
    if (idx.model() != d->m_sourceModel) {
        COMPLOG_WARNING_SYNC("Invalid index for mapFromSource passed into TreeGroupingProxyModel");
        return {};
    }
    auto targetNodeIt = d->cache_lowestLayer.find(getIndexHash(idx));
    if (d->cache_lowestLayer.end() == targetNodeIt) {
        return {};
    }
    return toModelIndex(targetNodeIt->second, idx.column());
}

uint TreeGroupingProxyModel::getGroupHash(const GroupKey_t &groupKey) const
{
    return qHash(groupKey.toString(), qGlobalQHashSeed());
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
    auto pTargetNode = d->cache_nodes[getGroupHash(groupKey)];
    if (!pTargetNode) {
        return {};
    }
    return pTargetNode->getData().getIndexData(column, role);
}

bool TreeGroupingProxyModel::setGroupData(GroupKey_t groupKey, int column, const QVariant &value, int role)
{
    auto pTargetNode = d->cache_nodes[getGroupHash(groupKey)];
    if (!pTargetNode) {
        return false;
    }
    pTargetNode->getData().setIndexData(value, column, role);
    auto changedIndex = toModelIndex(pTargetNode, column);
    emit dataChanged(changedIndex, changedIndex, {role});
    return true;
}

bool TreeGroupingProxyModel::canMergeGroups(const GroupKey_t &lgk, const GroupKey_t &rgk) const
{
    return lgk == rgk;
}

void TreeGroupingProxyModel::addNode(const QModelIndex &idx)
{
    // Setup node
    ItemMetadata nodeData {};
    nodeData.setSourceIndex(idx);
    nodeData.setGroupKey(getGroup(idx.row()));
    auto rowNode = d->m_invisibleRootNode->create(std::move(nodeData));

    // Go down and create branch if need
    auto rowNodeGroups = getBranchGroups(getParentGroup(rowNode->getData().getGroupKey()));
    d->cache_lowestLayer[getIndexHash(idx)] = rowNode;
    rowNode->setParent(setupMergableNode(rowNodeGroups));
}

void TreeGroupingProxyModel::updateNode(const QModelIndex &idx)
{
    // Go down and create branch if need
    Node_t::ptr_type rowNode;
    d->m_invisibleRootNode->callRecursive([&idx, &rowNode](auto pNode) -> bool {
        if (!pNode->getData().isSourceIndex()) {
            return false;
        }
        auto isTargetNode = idx == pNode->getData().getSourceIndex();
        if (isTargetNode) {
            rowNode = pNode;
        }
        return isTargetNode;
    });
    if (!rowNode) { return; }

    // Prepare to emit signal
    auto pPrevParent = (rowNode->getParent() ? rowNode->getParent() : d->m_invisibleRootNode);
    auto prevIndex = toModelIndex(pPrevParent);
    auto prevIndexRow = pPrevParent->getNodeRow(rowNode);
    auto newGroupKey = getGroup(idx.row());
    auto rowNodeGroups = getBranchGroups(getParentGroup(newGroupKey));

    // Get new pos
    auto pMergableNode = setupMergableNode(rowNodeGroups);
    auto& nodeData = rowNode->getData();
    d->cache_lowestLayer.erase(getIndexHash(nodeData.getSourceIndex()));
    nodeData.setSourceIndex(idx);
    nodeData.setGroupKey(newGroupKey);
    d->cache_lowestLayer[getIndexHash(idx)] = rowNode;

    // Update node position
    if (pMergableNode == pPrevParent) {
        return;
    }

    beginMoveRows(  prevIndex, prevIndexRow, prevIndexRow,
                  toModelIndex(pMergableNode), pMergableNode->getNodeCount());
    rowNode->setParent(pMergableNode);
    endMoveRows();

    prune(pPrevParent);
}

void TreeGroupingProxyModel::removeNode(const QModelIndex &idx)
{
    auto nodeIt = d->cache_lowestLayer.find(getIndexHash(idx));
    if (d->cache_lowestLayer.end() == nodeIt) {
        return;
    }
    auto pNode = nodeIt->second;

    auto pParent = pNode->getParent();
    if (!pParent) { pParent = d->m_invisibleRootNode; }
    auto nodeRow = pParent->getNodeRow(pNode);
    beginRemoveRows(toModelIndex(pParent), nodeRow, nodeRow);
    d->cache_lowestLayer.erase(nodeIt);
    pNode->setParent(nullptr);
    endRemoveRows();

    prune(pParent);
}

void TreeGroupingProxyModel::resetTree()
{
    d->cache_nodes.clear();
    d->cache_lowestLayer.clear();
    d->m_invisibleRootNode->clearNodes();
    if (d->m_sourceModel) {
        for (int row = 0; row < d->m_sourceModel->rowCount(); ++row) {
            addNode(d->m_sourceModel->index(row, 0));
        }
    }
}

void TreeGroupingProxyModel::prune(std::shared_ptr<Node_t> pBranchLeaf)
{
    if (!pBranchLeaf) { return; }
    while (pBranchLeaf->getParent()) {
        auto pParent = pBranchLeaf->getParent();
        if (pBranchLeaf->getNodeCount() > 0) {
            break;
        }
        auto nodeRow = pParent->getNodeRow(pBranchLeaf);
        beginRemoveRows(toModelIndex(pParent), nodeRow, nodeRow);
        d->cache_nodes.erase(getGroupHash(pBranchLeaf->getData().getGroupKey()));
        pBranchLeaf->setParent(nullptr);
        pBranchLeaf = pParent;
        endRemoveRows();
    }
}

uint TreeGroupingProxyModel::getIndexHash(const QModelIndex &sourceIndex) const
{
    return qHash(QPersistentModelIndex(sourceIndex), qGlobalQHashSeed());
}

std::shared_ptr<TreeGroupingProxyModel::Node_t> TreeGroupingProxyModel::toNode(const QModelIndex &idx) const
{
    if (!idx.isValid() ||
        !idx.internalPointer() ||
        idx.model() != this) {
        return {};
    }
    auto pTargetNode = static_cast<Node_t*>(idx.internalPointer());
    return pTargetNode->shared_from_this();
}

std::shared_ptr<TreeGroupingProxyModel::Node_t> TreeGroupingProxyModel::setupMergableNode(std::vector<GroupKey_t> groupBranch)
{
    auto pCurrentNode = d->m_invisibleRootNode;
    std::reverse(groupBranch.data(), groupBranch.data() + groupBranch.size());
    for (const auto& gKey : groupBranch) {
        bool foundMergable {false};
        for (int r = 0; r < pCurrentNode->getNodeCount(); ++r) {
            auto pNode = pCurrentNode->getNode(r);
            foundMergable = canMergeGroups(gKey, pNode->getData().getGroupKey());
            if (foundMergable) {
                pCurrentNode = pNode;
                break;
            }
        }
        if (foundMergable) {
            continue;
        }
        beginInsertRows(toModelIndex(pCurrentNode), pCurrentNode->getNodeCount() - 1, pCurrentNode->getNodeCount() - 1);
        ItemMetadata nodeData {};
        nodeData.setGroupKey(gKey);
        auto pSubnode = d->m_invisibleRootNode->create(std::move(nodeData));
        pSubnode->setParent(pCurrentNode);
        pCurrentNode = pSubnode;
        d->cache_nodes[getGroupHash(gKey)] = pSubnode;
        endInsertRows();
    }
    return pCurrentNode;
}

QModelIndex TreeGroupingProxyModel::toModelIndex(const std::shared_ptr<Node_t> &pNode, int column) const
{
    auto pParent = (pNode->getParent() ? pNode->getParent() : d->m_invisibleRootNode);
    return createIndex(pParent->getNodeRow(pNode), column, pNode.get());
}

std::vector<TreeGroupingProxyModel::GroupKey_t> TreeGroupingProxyModel::getBranchGroups(GroupKey_t rowNodeGroup) const
{
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
}

}