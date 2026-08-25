#include "searchproxymodel.hpp"

namespace QtCustom::Models {

SearchProxyModel::SearchProxyModel(QObject *parent)
    : QIdentityProxyModel{parent}
{

}

void SearchProxyModel::setEngine(AbstractSearchEngine *pEngine)
{

}

AbstractSearchEngine *SearchProxyModel::getEngine() const
{
    return {};
}

QModelIndex SearchProxyModel::getNextInclusion(const QModelIndex &offsetIndex) const
{
    return {};
}

QModelIndex SearchProxyModel::getPrevInclusion(const QModelIndex &offsetIndex) const
{
    return {};
}

int SearchProxyModel::rowCount(const QModelIndex &parent) const
{
    return {};
}

QModelIndex SearchProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    return {};
}

QModelIndex SearchProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    return {};
}

QModelIndex SearchProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    return {};
}

bool SearchProxyModel::isIndexWanted(const QModelIndex &idx)
{
    return {};
}

} // namespace QtCustom::Search
