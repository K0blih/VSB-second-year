#ifndef RECORDDIALOG_H
#define RECORDDIALOG_H

#include <QDialog>

#include "mainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class RecordDialog;
}
QT_END_NAMESPACE

class RecordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RecordDialog(QWidget *parent = nullptr);
    ~RecordDialog() override;

    PersonRecord record() const;
    void setRecord(const PersonRecord &record);

private:
    Ui::RecordDialog *ui;
};

#endif // RECORDDIALOG_H
