#pragma once

#include <QAbstractItemModel>

#include <memory>

namespace ExtraClasses
{
template <typename DataT> class TreeNode;
}

namespace QtCustom::Models
{

/**
 * @brief The TreeGroupingProxyModel class  Model, creating tree based on source table rows
 * @note    Can work not only with columns, but with objects. Use qvariant_cast for it and inherit proxy
 *          By default, model builds tree using column data from right to left.
 *          Tree is built using rule one row - one group
 */
class TreeGroupingProxyModel : public QAbstractItemModel
{
    Q_OBJECT

    struct ItemMetadata;
    using Node_t = ExtraClasses::TreeNode<ItemMetadata>;
public:
    explicit TreeGroupingProxyModel(QObject *parent = nullptr);
    ~TreeGroupingProxyModel();

    // QAbstractItemModel interface
    QModelIndex index(int row, int column, const QModelIndex &parent = {}) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    // Proxy model equ functional
    void setSourceModel(QAbstractItemModel* pModel);
    QAbstractItemModel* sourceModel() const;
    QModelIndex mapToSource(const QModelIndex& idx) const;
    QModelIndex mapFromSource(const QModelIndex& idx) const;

    // Grouping functionality
    using GroupKey_t = QVariant;
protected:
    /**
     * @brief getGroup          Define first level of a tree
     * @param sourceModelRow
     * @return
     */
    virtual GroupKey_t  getGroup(int sourceModelRow) const;

    /**
     * @brief getParentGroup    Define every upper than first level of a tree
     * @param groupKey
     * @return                  Must return invalid GroupKey_t if level is highest
     */
    virtual GroupKey_t  getParentGroup(GroupKey_t groupKey) const;

    // Used to get and set data of a group indexes
    virtual bool        setGroupData(GroupKey_t groupKey, int column, const QVariant& value, int role);
    virtual QVariant    getGroupData(GroupKey_t groupKey, int column, int role) const;

    /**
     * @brief canMergeGroups Used to merge groups. Both arguments are in same level
     * @param lgk
     * @param rgk
     * @return  true if groups can be merged
     */
    virtual bool canMergeGroups(const GroupKey_t& lgk, const GroupKey_t& rgk) const;

private:
    struct Impl;
    std::unique_ptr<Impl> d;

    // Branch building
    void addNode(const QModelIndex& idx);
    void removeNode(const QModelIndex& idx);

    QModelIndex toModelIndex(const std::shared_ptr<Node_t>& pNode, int column = 0) const;
};

}