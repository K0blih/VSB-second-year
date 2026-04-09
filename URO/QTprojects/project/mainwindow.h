#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "cardrecord.h"

#include <QList>
#include <QMainWindow>

class QAction;
class QLabel;
class QPushButton;
class QTableWidget;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    void setupCentralArea();
    void setupMenus();
    void setupSampleData();
    void refreshTable();
    void updatePreview();
    void updateActionState();
    int currentRow() const;
    CardRecord currentCard() const;
    void addCard();
    void editCurrentCard();
    void deleteCurrentCard();
    void showCurrentCardDetail();

    Ui::MainWindow *ui;
    QTableWidget *m_table = nullptr;
    QLabel *m_previewImageLabel = nullptr;
    QLabel *m_previewTitleLabel = nullptr;
    QLabel *m_previewMetaLabel = nullptr;
    QLabel *m_previewStatsLabel = nullptr;
    QLabel *m_previewDescriptionLabel = nullptr;
    QPushButton *m_addButton = nullptr;
    QPushButton *m_editButton = nullptr;
    QPushButton *m_deleteButton = nullptr;
    QPushButton *m_detailButton = nullptr;
    QAction *m_editAction = nullptr;
    QAction *m_deleteAction = nullptr;
    QAction *m_detailAction = nullptr;
    QList<CardRecord> m_cards;
};
#endif // MAINWINDOW_H
