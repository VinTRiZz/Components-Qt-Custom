#pragma once

#include <QIdentityProxyModel>

namespace QtCustom::Models {

class AbstractSearchEngine;

/**
 * @brief The SearchMode enum Working mode of an search model
 */
enum class SearchMode {
    Signal = 0, // Emit signal after found next
    Filter,     // Hide all unwanted indexes
};

class SearchProxyModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    explicit SearchProxyModel(QObject *parent = nullptr);

    void setEngine(AbstractSearchEngine* pEngine);
    AbstractSearchEngine* getEngine() const;

    // Index by index search
    QModelIndex getNextInclusion(const QModelIndex& offsetIndex) const;
    QModelIndex getPrevInclusion(const QModelIndex& offsetIndex) const;

    // QIdentityProxyModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;
    QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

signals:
    void sig_foundNext(const QModelIndex& sourceIndex);

private:
    AbstractSearchEngine* m_search_engine = nullptr;
    SearchMode m_mode = SearchMode::Signal;

    bool isIndexWanted(const QModelIndex& idx);
};

} // namespace QtCustom::Search
