#include "extendedcombobox.hpp"
#include "ui_extendedcombobox.h"

#include <QIdentityProxyModel>
#include "extendedcomboboxdialog.hpp"

#include <Components/Logger/Logger.h>

namespace QtCustom::Widgets {

class NullItemProxyModel : public QIdentityProxyModel
{
public:
    using QIdentityProxyModel::QIdentityProxyModel;

    void setRootIndex(const QModelIndex& idx) {
        m_cboxRootIndex = idx;
        m_cboxSourceRootIndex = QIdentityProxyModel::mapToSource(idx);
    }

    void setNullItemData(int role, const QVariant& val) {
        m_nullItemData[role] = val;
    }

    // QAbstractItemModel interface
    QModelIndex index(int row, int column, const QModelIndex &parent) const override {
        if (parent == m_cboxRootIndex) {
            if (row == 0) {
                return createIndex(0, column, 1);
            }
            auto idx = QIdentityProxyModel::index(row - 1, column, parent);
            return createIndex(row, column, idx.internalPointer());
        }
        return QIdentityProxyModel::index(row, column, parent);
    }

    int rowCount(const QModelIndex &parent) const override {
        if (parent == m_cboxRootIndex) {
            return QIdentityProxyModel::rowCount(parent) + 1;
        }
        return QIdentityProxyModel::rowCount(parent);
    }

    QVariant data(const QModelIndex &idx, int role) const override {
        if (idx.parent() == m_cboxRootIndex) {
            if (idx.row() == 0) {
                if (m_nullItemData.contains(role)) {
                    return m_nullItemData[role];
                }
                return {};
            }
            return QIdentityProxyModel::data(createIndex(idx.row() - 1, idx.column(), idx.internalPointer()), role);
        }
        return QIdentityProxyModel::data(idx, role);
    }

    Qt::ItemFlags flags(const QModelIndex &index) const override {
        if (index.isValid() && index.internalId() == 1) {
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        }
        return QIdentityProxyModel::flags(index);
    }

    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override {
        if (proxyIndex.isValid() && proxyIndex.internalId() == 1) {
            if (proxyIndex.row() == 0) {
                return {};
            }
            // return QIdentityProxyModel::mapToSource(createIndex(proxyIndex.row() - 1, proxyIndex.column(), 1));
        }
        return QIdentityProxyModel::mapToSource(proxyIndex);
    }
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override {
        if (sourceIndex.isValid() &&
            sourceIndex.parent() == m_cboxSourceRootIndex) {
            // return createIndex(sourceIndex.row() + 1, sourceIndex.column());
        }
        return QIdentityProxyModel::mapFromSource(sourceIndex);
    }

private:
    QMap<int, QVariant> m_nullItemData;
    QModelIndex m_cboxRootIndex;
    QModelIndex m_cboxSourceRootIndex;
};


ExtendedComboBox::ExtendedComboBox(QWidget *parent) :
    QWidget(parent),
    ui{new Ui::ExtendedComboBox}
{
    ui->setupUi(this);

    m_pDialog = new ExtendedComboBoxDialog(this);
    connect(ui->toolButtonSearch, &QToolButton::clicked,
            this, [this](){
        if (!m_pComboBoxModel->sourceModel()) {
            COMPLOG_WARNING("ExtendedComboBox: No model set");
            return;
        }
        auto selectedIndex = m_pComboBoxModel->index(0, ui->comboBox->currentIndex(), ui->comboBox->rootModelIndex());
        auto sourceSelectedIndex = m_pComboBoxModel->mapToSource(selectedIndex);
        m_pDialog->setSelectedIndex(sourceSelectedIndex);

        auto dialogExecResult = m_pDialog->exec();
        if (QDialog::Accepted == dialogExecResult) {
            sourceSelectedIndex = m_pDialog->getSelectedIndex();
            selectedIndex = m_pComboBoxModel->mapFromSource(sourceSelectedIndex);
            ui->comboBox->setCurrentIndex(selectedIndex.row());
        }
    });

    m_pComboBoxModel = new NullItemProxyModel(this);
    ui->comboBox->setModel(m_pComboBoxModel);
}

ExtendedComboBox::~ExtendedComboBox()
{
    delete ui;
}

void ExtendedComboBox::setSourceModel(QAbstractItemModel *pModel)
{
    m_pComboBoxModel->setSourceModel(pModel);
    m_pDialog->setSourceModel(pModel);
}

void ExtendedComboBox::setRootIndex(const QModelIndex &idx)
{
    ui->comboBox->setRootModelIndex(idx);
}

void ExtendedComboBox::setModelColumn(int col)
{
    ui->comboBox->setModelColumn(col);
}

void ExtendedComboBox::setNullIndexText(const QString &text)
{
    m_pComboBoxModel->setNullItemData(Qt::DisplayRole, text);
}

void ExtendedComboBox::setNullIndexData(const QMap<int, QVariant> &idxData)
{
    for (auto key : idxData.keys()) {
        m_pComboBoxModel->setNullItemData(key, idxData.value(key));
    }
}

void ExtendedComboBox::setDialogModelColumnHidden(int col, bool isColumnHidden)
{
    m_pDialog->setDialogModelColumnHidden(col, isColumnHidden);
}

void ExtendedComboBox::setHeaderHidden(bool isHeaderHidden)
{
    m_pDialog->setHeaderHidden(isHeaderHidden);
}

} // namespace QtCustom::Widgets
