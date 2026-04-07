#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QTableWidgetItem;

struct PersonRecord
{
    QString name;
    QString role;
    QString email;
    QString note;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    void loadSampleData();
    void refreshTable();
    void openCreateDialog();
    void openEditDialog(int row);

    Ui::MainWindow *ui;
    QVector<PersonRecord> m_records;
};
#endif // MAINWINDOW_H
