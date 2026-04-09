#include "carddialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPixmap>
#include <QSpinBox>
#include <QTextEdit>
#include <QVBoxLayout>

CardDialog::CardDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
}

CardDialog::CardDialog(const CardRecord &record, QWidget *parent)
    : QDialog(parent)
{
    setupUi();
    setRecord(record);
}

CardRecord CardDialog::record() const
{
    return {
        m_nameEdit->text().trimmed(),
        m_categoryEdit->text().trimmed(),
        m_rarityCombo->currentText(),
        m_attackSpin->value(),
        m_defenseSpin->value(),
        m_descriptionEdit->toPlainText().trimmed(),
        m_imageCombo->currentData().toString()
    };
}

void CardDialog::accept()
{
    if (m_nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Missing name", "Card name is required.");
        m_nameEdit->setFocus();
        return;
    }

    if (m_descriptionEdit->toPlainText().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Missing description", "Card description is required.");
        m_descriptionEdit->setFocus();
        return;
    }

    QDialog::accept();
}

void CardDialog::setupUi()
{
    setWindowTitle("Card Editor");
    resize(440, 420);

    auto *rootLayout = new QVBoxLayout(this);
    auto *formLayout = new QFormLayout;

    m_nameEdit = new QLineEdit(this);
    m_categoryEdit = new QLineEdit(this);

    m_rarityCombo = new QComboBox(this);
    m_rarityCombo->addItems({"Common", "Rare", "Epic", "Legendary"});

    m_attackSpin = new QSpinBox(this);
    m_attackSpin->setRange(0, 999);

    m_defenseSpin = new QSpinBox(this);
    m_defenseSpin->setRange(0, 999);

    m_descriptionEdit = new QTextEdit(this);
    m_descriptionEdit->setPlaceholderText("Short lore or effect text...");

    m_imageCombo = new QComboBox(this);
    populateImageChoices();

    formLayout->addRow("Name:", m_nameEdit);
    formLayout->addRow("Category:", m_categoryEdit);
    formLayout->addRow("Rarity:", m_rarityCombo);
    formLayout->addRow("Attack:", m_attackSpin);
    formLayout->addRow("Defense:", m_defenseSpin);
    formLayout->addRow("Image:", m_imageCombo);
    formLayout->addRow("Description:", m_descriptionEdit);

    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &CardDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &CardDialog::reject);

    rootLayout->addLayout(formLayout);
    rootLayout->addWidget(buttonBox);
}

void CardDialog::populateImageChoices()
{
    const struct ImageChoice {
        const char *label;
        const char *path;
    } choices[] = {
        {"Earth Titan", ":/images/earth_titan.xpm"},
        {"Flame Dragon", ":/images/flame_dragon.xpm"},
        {"Forest Guardian", ":/images/forest_guardian.xpm"},
        {"Storm Relic", ":/images/storm_relic.xpm"},
        {"Wind Rider", ":/images/wind_rider.xpm"}
    };

    for (const ImageChoice &choice : choices) {
        QPixmap pixmap(QString::fromLatin1(choice.path));
        m_imageCombo->addItem(QIcon(pixmap.scaled(32, 32)),
                              QString::fromLatin1(choice.label),
                              QString::fromLatin1(choice.path));
    }
}

void CardDialog::setRecord(const CardRecord &record)
{
    m_nameEdit->setText(record.name);
    m_categoryEdit->setText(record.category);
    m_rarityCombo->setCurrentText(record.rarity);
    m_attackSpin->setValue(record.attack);
    m_defenseSpin->setValue(record.defense);
    m_descriptionEdit->setPlainText(record.description);

    const int imageIndex = m_imageCombo->findData(record.imagePath);
    if (imageIndex >= 0) {
        m_imageCombo->setCurrentIndex(imageIndex);
    }
}
