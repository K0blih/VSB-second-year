#include "carddetaildialog.h"

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QTextEdit>
#include <QVBoxLayout>

CardDetailDialog::CardDetailDialog(const CardRecord &record, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Card Detail");
    resize(520, 340);

    auto *mainLayout = new QHBoxLayout(this);
    auto *infoLayout = new QVBoxLayout;

    m_imageLabel = new QLabel(this);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setMinimumSize(220, 220);
    m_imageLabel->setPixmap(QPixmap(record.imagePath).scaled(220, 220, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    m_nameLabel = new QLabel(QString("<h2>%1</h2>").arg(record.name), this);
    m_metaLabel = new QLabel(QString("%1 | %2").arg(record.category, record.rarity), this);
    m_statsLabel = new QLabel(QString("Attack: %1    Defense: %2").arg(record.attack).arg(record.defense), this);

    m_descriptionEdit = new QTextEdit(this);
    m_descriptionEdit->setReadOnly(true);
    m_descriptionEdit->setPlainText(record.description);

    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &CardDetailDialog::reject);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &CardDetailDialog::accept);

    infoLayout->addWidget(m_nameLabel);
    infoLayout->addWidget(m_metaLabel);
    infoLayout->addWidget(m_statsLabel);
    infoLayout->addWidget(m_descriptionEdit);
    infoLayout->addWidget(buttonBox);

    mainLayout->addWidget(m_imageLabel);
    mainLayout->addLayout(infoLayout);
}
