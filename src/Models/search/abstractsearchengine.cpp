#include "abstractsearchengine.hpp"

#include <QModelIndex>

namespace QtCustom::Models {

AbstractSearchEngine::AbstractSearchEngine(QObject *parent)
    : QObject{parent}
{

}

bool AbstractSearchEngine::isWanted([[maybe_unused]] const QModelIndex &sourceIndex) const {
    return true;
}


void TextSearchEngine::setTarget(const QString &value)
{
    m_targetTrigrams = createTrigrams(value);
    m_indexDeltaPercent = double(value.size()) / 3.0;
    emit sig_filterChanged();
}

void TextSearchEngine::setThreshold(double searchThreshold)
{
    m_searchThreshold = searchThreshold;
    emit sig_filterChanged();
}

bool TextSearchEngine::isWanted(const QModelIndex &sourceIndex) const
{
    return containTarget(m_targetTrigrams, sourceIndex.data(Qt::DisplayRole).toString(), m_searchThreshold);
}

std::vector<QString> TextSearchEngine::createTrigrams(const QString &text) const {
    if (text.isEmpty())
        return {};

    std::vector<QString> trigrams;
    trigrams.reserve(text.size() / 3);

    QString currentTrigram;
    for (const auto& ch : text) {
        if (currentTrigram.size() < 3) {
            currentTrigram.push_back(ch);
            continue;
        }
        std::sort(currentTrigram.begin(), currentTrigram.end());
        trigrams.emplace_back(std::move(currentTrigram));
        currentTrigram.reserve(3);
        currentTrigram.push_back(ch);
    }

    if (!currentTrigram.isEmpty()) {
        std::sort(currentTrigram.begin(), currentTrigram.end());
        std::fill_n(std::back_inserter(currentTrigram), 3 - currentTrigram.size(), ' ');
        trigrams.emplace_back(std::move(currentTrigram));
    }
    return trigrams;
}

double TextSearchEngine::getSamePercent(const std::vector<QString> &targetTrigram, const std::vector<QString> &sourceTrigram) const {
    double samePercent {};

    auto targetSize = targetTrigram.size();
    double deltaSamePercent = 1.0 / double(targetSize) / 3.0;

    auto sourceSize = sourceTrigram.size();

    std::size_t targetIdx {};
    for (; targetIdx < targetSize && targetIdx < sourceSize; ++targetIdx) {
        auto& targetTrig = targetTrigram[targetIdx];
        auto& sourceTrig = sourceTrigram[targetIdx];

        samePercent += (targetTrig[0] == sourceTrig[0]) * deltaSamePercent;
        samePercent += (targetTrig[1] == sourceTrig[1]) * deltaSamePercent;
        samePercent += (targetTrig[2] == sourceTrig[2]) * deltaSamePercent;
    }

    return samePercent;
}

bool TextSearchEngine::containTarget(const std::vector<QString> &targetTrigrams, const QString &sampleText, double threshold) const {
    double maxPercent {};
    for (const auto& tok : sampleText.split(' ')) {
        auto samePercent = getSamePercent(targetTrigrams, createTrigrams(tok));
        maxPercent = std::max(maxPercent, samePercent);
        if (maxPercent > threshold) {
            break;
        }
    }
    return (maxPercent > threshold);
}

} // namespace QtCustom::Search
