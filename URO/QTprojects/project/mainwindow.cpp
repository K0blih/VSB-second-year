#include "mainwindow.h"
#include "carddetaildialog.h"
#include "carddialog.h"
#include "ui_mainwindow.h"

#include <QAction>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QStatusBar>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupCentralArea();
    setupMenus();
    setupSampleData();
    refreshTable();
    updatePreview();
    updateActionState();
    statusBar()->showMessage("Loaded 4 sample cards.", 4000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupCentralArea()
{
    setWindowTitle("Card Catalog");
    resize(1080, 640);

    auto *rootLayout = new QHBoxLayout(ui->centralwidget);
    auto *leftLayout = new QVBoxLayout;
    auto *buttonLayout = new QHBoxLayout;
    auto *previewLayout = new QVBoxLayout;

    m_addButton = new QPushButton("Add Card", this);
    m_editButton = new QPushButton("Edit Card", this);
    m_deleteButton = new QPushButton("Delete Card", this);
    m_detailButton = new QPushButton("View Detail", this);

    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_editButton);
    buttonLayout->addWidget(m_deleteButton);
    buttonLayout->addWidget(m_detailButton);
    buttonLayout->addStretch();

    m_table = new QTableWidget(this);
    m_table->setColumnCount(5);
    m_table->setHorizontalHeaderLabels({"Name", "Category", "Rarity", "Attack", "Defense"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setStretchLastSection(false);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_table->verticalHeader()->setVisible(false);

    leftLayout->addLayout(buttonLayout);
    leftLayout->addWidget(m_table);

    auto *previewTitle = new QLabel("<b>Selected Card Preview</b>", this);
    m_previewImageLabel = new QLabel(this);
    m_previewImageLabel->setAlignment(Qt::AlignCenter);
    m_previewImageLabel->setMinimumSize(240, 240);
    m_previewImageLabel->setStyleSheet("background: #f3f4f6; border: 1px solid #c7ccd1;");

    m_previewTitleLabel = new QLabel(this);
    m_previewMetaLabel = new QLabel(this);
    m_previewStatsLabel = new QLabel(this);
    m_previewDescriptionLabel = new QLabel(this);
    m_previewDescriptionLabel->setWordWrap(true);

    previewLayout->addWidget(previewTitle);
    previewLayout->addWidget(m_previewImageLabel);
    previewLayout->addWidget(m_previewTitleLabel);
    previewLayout->addWidget(m_previewMetaLabel);
    previewLayout->addWidget(m_previewStatsLabel);
    previewLayout->addWidget(m_previewDescriptionLabel);
    previewLayout->addStretch();

    rootLayout->addLayout(leftLayout, 3);
    rootLayout->addLayout(previewLayout, 2);

    connect(m_addButton, &QPushButton::clicked, this, &MainWindow::addCard);
    connect(m_editButton, &QPushButton::clicked, this, &MainWindow::editCurrentCard);
    connect(m_deleteButton, &QPushButton::clicked, this, &MainWindow::deleteCurrentCard);
    connect(m_detailButton, &QPushButton::clicked, this, &MainWindow::showCurrentCardDetail);
    connect(m_table, &QTableWidget::itemSelectionChanged, this, [this]() {
        updatePreview();
        updateActionState();
        if (currentRow() >= 0) {
            statusBar()->showMessage(QString("Selected: %1").arg(currentCard().name), 2500);
        }
    });
    connect(m_table, &QTableWidget::itemDoubleClicked, this, [this]() {
        showCurrentCardDetail();
    });
}

void MainWindow::setupMenus()
{
    QMenu *fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction("New Card", this, &MainWindow::addCard);
    fileMenu->addSeparator();
    fileMenu->addAction("Exit", this, &QWidget::close);

    QMenu *editMenu = menuBar()->addMenu("Edit");
    m_editAction = editMenu->addAction("Edit Selected", this, &MainWindow::editCurrentCard);
    m_deleteAction = editMenu->addAction("Delete Selected", this, &MainWindow::deleteCurrentCard);

    QMenu *viewMenu = menuBar()->addMenu("View");
    m_detailAction = viewMenu->addAction("Card Detail", this, &MainWindow::showCurrentCardDetail);

    QMenu *helpMenu = menuBar()->addMenu("Help");
    helpMenu->addAction("About", this, [this]() {
        QMessageBox::about(this,
                           "About Card Catalog",
                           "Card Catalog is a Qt Widgets demo project with static sample data.\n\n"
                           "It demonstrates a table view, dialogs, images, menu actions and a status bar.\n\n"
                           "Richard Chovanec CHO0289");
    });
}

void MainWindow::setupSampleData()
{
    m_cards = {
        {"Flame Dragon", "Creature", "Legendary", 92, 68,
         "An ancient dragon that overwhelms the arena with firestorms and pressure.", ":/images/flame_dragon.xpm"},
        {"Forest Guardian", "Creature", "Epic", 61, 88,
         "A sentinel of the old woods that protects allied cards with layered shields.", ":/images/forest_guardian.xpm"},
        {"Wind Rider", "Creature", "Rare", 57, 46,
         "A swift sky duelist that gains momentum from every gust and repositioning move.", ":/images/wind_rider.xpm"},
        {"Earth Titan", "Creature", "Epic", 83, 94,
         "A massive stone guardian that anchors the battlefield and absorbs direct attacks.", ":/images/earth_titan.xpm"},
        {"Storm Relic", "Artifact", "Rare", 74, 54,
         "A charged relic that stores lightning and boosts the next offensive combo.", ":/images/storm_relic.xpm"},
        {"Ash Scout", "Creature", "Common", 38, 31,
         "A fast reconnaissance unit used to reveal weak points before the main attack.", ":/images/flame_dragon.xpm"}
    };
}

void MainWindow::refreshTable()
{
    m_table->setRowCount(m_cards.size());

    for (int row = 0; row < m_cards.size(); ++row) {
        const CardRecord &card = m_cards.at(row);
        auto *nameItem = new QTableWidgetItem(card.name);
        nameItem->setIcon(QIcon(card.imagePath));
        m_table->setItem(row, 0, nameItem);
        m_table->setItem(row, 1, new QTableWidgetItem(card.category));
        m_table->setItem(row, 2, new QTableWidgetItem(card.rarity));
        m_table->setItem(row, 3, new QTableWidgetItem(QString::number(card.attack)));
        m_table->setItem(row, 4, new QTableWidgetItem(QString::number(card.defense)));
    }

    if (!m_cards.isEmpty()) {
        const int row = qBound(0, currentRow(), m_cards.size() - 1);
        m_table->selectRow(row);
    }
}

void MainWindow::updatePreview()
{
    const int row = currentRow();
    if (row < 0 || row >= m_cards.size()) {
        m_previewImageLabel->setText("No card selected");
        m_previewImageLabel->setPixmap(QPixmap());
        m_previewTitleLabel->setText("Select a row in the table.");
        m_previewMetaLabel->clear();
        m_previewStatsLabel->clear();
        m_previewDescriptionLabel->clear();
        return;
    }

    const CardRecord &card = m_cards.at(row);
    m_previewImageLabel->setText(QString());
    m_previewImageLabel->setPixmap(QPixmap(card.imagePath).scaled(240, 240, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_previewTitleLabel->setText(QString("<b>%1</b>").arg(card.name));
    m_previewMetaLabel->setText(QString("%1 | %2").arg(card.category, card.rarity));
    m_previewStatsLabel->setText(QString("Attack: %1    Defense: %2").arg(card.attack).arg(card.defense));
    m_previewDescriptionLabel->setText(card.description);
}

void MainWindow::updateActionState()
{
    const bool hasSelection = currentRow() >= 0;
    m_editButton->setEnabled(hasSelection);
    m_deleteButton->setEnabled(hasSelection);
    m_detailButton->setEnabled(hasSelection);
    if (m_editAction) {
        m_editAction->setEnabled(hasSelection);
    }
    if (m_deleteAction) {
        m_deleteAction->setEnabled(hasSelection);
    }
    if (m_detailAction) {
        m_detailAction->setEnabled(hasSelection);
    }
}

int MainWindow::currentRow() const
{
    const QModelIndexList selection = m_table->selectionModel()->selectedRows();
    return selection.isEmpty() ? -1 : selection.first().row();
}

CardRecord MainWindow::currentCard() const
{
    const int row = currentRow();
    return row >= 0 ? m_cards.at(row) : CardRecord{};
}

void MainWindow::addCard()
{
    CardDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted) {
        statusBar()->showMessage("New card creation canceled.", 2500);
        return;
    }

    m_cards.append(dialog.record());
    refreshTable();
    m_table->selectRow(m_cards.size() - 1);
    statusBar()->showMessage(QString("Added card: %1").arg(m_cards.last().name), 3000);
}

void MainWindow::editCurrentCard()
{
    const int row = currentRow();
    if (row < 0) {
        return;
    }

    CardDialog dialog(m_cards.at(row), this);
    if (dialog.exec() != QDialog::Accepted) {
        statusBar()->showMessage("Edit canceled.", 2500);
        return;
    }

    m_cards[row] = dialog.record();
    refreshTable();
    m_table->selectRow(row);
    statusBar()->showMessage(QString("Updated card: %1").arg(m_cards.at(row).name), 3000);
}

void MainWindow::deleteCurrentCard()
{
    const int row = currentRow();
    if (row < 0) {
        return;
    }

    const CardRecord card = m_cards.at(row);
    const auto answer = QMessageBox::question(this,
                                              "Delete card",
                                              QString("Delete '%1' from the catalog?").arg(card.name));
    if (answer != QMessageBox::Yes) {
        statusBar()->showMessage("Delete canceled.", 2500);
        return;
    }

    m_cards.removeAt(row);
    refreshTable();
    updatePreview();
    updateActionState();
    statusBar()->showMessage(QString("Deleted card: %1").arg(card.name), 3000);
}

void MainWindow::showCurrentCardDetail()
{
    const int row = currentRow();
    if (row < 0) {
        return;
    }

    CardDetailDialog dialog(m_cards.at(row), this);
    dialog.exec();
    statusBar()->showMessage(QString("Viewed detail: %1").arg(m_cards.at(row).name), 2500);
}
