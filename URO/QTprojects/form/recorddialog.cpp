#include "recorddialog.h"
#include "ui_recorddialog.h"

RecordDialog::RecordDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RecordDialog)
{
    ui->setupUi(this);
}

RecordDialog::~RecordDialog()
{
    delete ui;
}

PersonRecord RecordDialog::record() const
{
    return {
        ui->editName->text().trimmed(),
        ui->editRole->text().trimmed(),
        ui->editEmail->text().trimmed(),
        ui->editNote->toPlainText().trimmed()
    };
}

void RecordDialog::setRecord(const PersonRecord &record)
{
    ui->editName->setText(record.name);
    ui->editRole->setText(record.role);
    ui->editEmail->setText(record.email);
    ui->editNote->setPlainText(record.note);
}
