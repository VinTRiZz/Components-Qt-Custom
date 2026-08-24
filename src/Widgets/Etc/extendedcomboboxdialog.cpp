#include "extendedcomboboxdialog.hpp"
#include "ui_extendedcomboboxdialog.h"


namespace QtCustom::Widgets {

ExtendedComboBoxDialog::ExtendedComboBoxDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ExtendedComboBoxDialog)
{
    ui->setupUi(this);

    connect(ui->tableView, &QTableView::doubleClicked,
            this, &QDialog::accept);

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->hide();
}

ExtendedComboBoxDialog::~ExtendedComboBoxDialog()
{
    delete ui;
}

void ExtendedComboBoxDialog::setSourceModel(QAbstractItemModel *pModel)
{
    // TODO: search -> setSourceModel
    ui->tableView->setModel(pModel);
}

void ExtendedComboBoxDialog::setSelectedIndex(const QModelIndex &idx)
{
    // TODO: search -> mapFromSource
    ui->tableView->setCurrentIndex(idx);
}

QModelIndex ExtendedComboBoxDialog::getSelectedIndex() const
{
    // TODO: search -> mapToSource
    return ui->tableView->currentIndex();
}

void ExtendedComboBoxDialog::setDialogModelColumnHidden(int col, bool isColumnHidden)
{
    ui->tableView->setColumnHidden(col, isColumnHidden);
}

void ExtendedComboBoxDialog::setHeaderHidden(bool isHeaderHidden)
{
    ui->tableView->horizontalHeader()->setHidden(isHeaderHidden);
}

}