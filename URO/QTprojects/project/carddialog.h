#ifndef CARDDIALOG_H
#define CARDDIALOG_H

#include "cardrecord.h"

#include <QDialog>

class QComboBox;
class QLineEdit;
class QSpinBox;
class QTextEdit;

class CardDialog : public QDialog
{
public:
    explicit CardDialog(QWidget *parent = nullptr);
    explicit CardDialog(const CardRecord &record, QWidget *parent = nullptr);

    CardRecord record() const;

protected:
    void accept() override;

private:
    void setupUi();
    void populateImageChoices();
    void setRecord(const CardRecord &record);

    QLineEdit *m_nameEdit = nullptr;
    QLineEdit *m_categoryEdit = nullptr;
    QComboBox *m_rarityCombo = nullptr;
    QSpinBox *m_attackSpin = nullptr;
    QSpinBox *m_defenseSpin = nullptr;
    QTextEdit *m_descriptionEdit = nullptr;
    QComboBox *m_imageCombo = nullptr;
};

#endif // CARDDIALOG_H
