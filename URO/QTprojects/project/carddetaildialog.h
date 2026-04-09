#ifndef CARDDETAILDIALOG_H
#define CARDDETAILDIALOG_H

#include "cardrecord.h"

#include <QDialog>

class QLabel;
class QTextEdit;

class CardDetailDialog : public QDialog
{
public:
    explicit CardDetailDialog(const CardRecord &record, QWidget *parent = nullptr);

private:
    QLabel *m_imageLabel = nullptr;
    QLabel *m_nameLabel = nullptr;
    QLabel *m_metaLabel = nullptr;
    QLabel *m_statsLabel = nullptr;
    QTextEdit *m_descriptionEdit = nullptr;
};

#endif // CARDDETAILDIALOG_H
