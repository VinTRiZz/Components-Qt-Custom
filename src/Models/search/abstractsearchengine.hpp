#pragma once

#include <QObject>
#include <QString>

#include <vector>

namespace QtCustom::Models {

/**
 * @brief The AbstractSearchEngine class Search engine base for SearchModel
 * @note In every inherited class must emit filter changed signal to ask model for update
 */
class AbstractSearchEngine : public QObject
{
    Q_OBJECT
public:
    explicit AbstractSearchEngine(QObject *parent = nullptr);

    /**
     * @brief isWanted      Check if index is wanted by search engine
     * @param sourceIndex
     * @return
     */
    virtual bool isWanted(const QModelIndex& sourceIndex) const;

signals:
    // Ask model to refilter
    void sig_filterChanged();
};


/**
 * @brief The TextSearchEngine class Search engine that uses trigrams
 */
class TextSearchEngine : public AbstractSearchEngine
{
public:
    using AbstractSearchEngine::AbstractSearchEngine;

    virtual void setTarget(const QString& value);
    virtual void setThreshold(double searchThreshold); // Value is 0...1

    virtual bool isWanted(const QModelIndex& sourceIndex) const;

private:
    std::vector<QString> m_targetTrigrams;
    double m_searchThreshold {0.8};

    double m_indexDeltaPercent {0};

protected:
    std::vector<QString> createTrigrams(const QString &text) const;

    double getSamePercent(const std::vector<QString>& targetTrigram,
                          const std::vector<QString>& sourceTrigram) const;

    bool containTarget(const std::vector<QString>& targetText, const QString& sampleText, double threshold) const;
};

} // namespace QtCustom::Search