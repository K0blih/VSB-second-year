#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "recorddialog.h"

#include <QHeaderView>
#include <QTableWidgetItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tableRecords->horizontalHeader()->setStretchLastSection(true);
    ui->tableRecords->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableRecords->verticalHeader()->setVisible(false);
    ui->tableRecords->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableRecords->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableRecords->setEditTriggers(QAbstractItemView::NoEditTriggers);

    loadSampleData();
    refreshTable();

    connect(ui->buttonAdd, &QPushButton::clicked, this, &MainWindow::openCreateDialog);
    connect(ui->buttonEdit, &QPushButton::clicked, this, [this]() {
        openEditDialog(ui->tableRecords->currentRow());
    });
    connect(ui->tableRecords, &QTableWidget::cellDoubleClicked, this, [this](int row, int) {
        openEditDialog(row);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadSampleData()
{
    m_records = {
        {"Jana Novakova", "Projektova manazerka", "jana.novakova@example.com", "Koordinuje terminy a zadani."},
        {"Petr Svoboda", "Vyvojar", "petr.svoboda@example.com", "Stara se o implementaci desktop casti."},
        {"Lucie Dvorakova", "Testerka", "lucie.dvorakova@example.com", "Pripravuje testovaci scenare a kontrolu formulare."}
    };
}

void MainWindow::refreshTable()
{
    ui->tableRecords->setRowCount(m_records.size());

    for (int row = 0; row < m_records.size(); ++row) {
        const PersonRecord &record = m_records.at(row);
        ui->tableRecords->setItem(row, 0, new QTableWidgetItem(record.name));
        ui->tableRecords->setItem(row, 1, new QTableWidgetItem(record.role));
        ui->tableRecords->setItem(row, 2, new QTableWidgetItem(record.email));
        ui->tableRecords->setItem(row, 3, new QTableWidgetItem(record.note));
    }

    const bool hasRows = !m_records.isEmpty();
    ui->buttonEdit->setEnabled(hasRows);
    if (hasRows && ui->tableRecords->currentRow() < 0) {
        ui->tableRecords->selectRow(0);
    }
}

void MainWindow::openCreateDialog()
{
    RecordDialog dialog(this);
    dialog.setWindowTitle("Novy zaznam");

    if (dialog.exec() == QDialog::Accepted) {
        m_records.append(dialog.record());
        refreshTable();
        ui->statusbar->showMessage("Zaznam byl vytvoren.", 3000);
    }
}

void MainWindow::openEditDialog(int row)
{
    if (row < 0 || row >= m_records.size()) {
        ui->statusbar->showMessage("Vyberte zaznam k uprave.", 3000);
        return;
    }

    RecordDialog dialog(this);
    dialog.setWindowTitle("Uprava zaznamu");
    dialog.setRecord(m_records.at(row));

    if (dialog.exec() == QDialog::Accepted) {
        m_records[row] = dialog.record();
        refreshTable();
        ui->tableRecords->selectRow(row);
        ui->statusbar->showMessage("Zaznam byl upraven.", 3000);
    }
}
