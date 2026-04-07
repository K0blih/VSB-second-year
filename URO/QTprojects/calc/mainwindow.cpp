#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QJSEngine>
#include <QJSValue>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    const QList<QPushButton *> inputButtons = {
        ui->zeroButton,
        ui->oneButton,
        ui->twoButton,
        ui->threeButton,
        ui->fourButton,
        ui->fiveButton,
        ui->sixButton,
        ui->sevenButton,
        ui->eightButton,
        ui->nineButton,
        ui->dotButton,
        ui->plusButton,
        ui->minusButton,
        ui->multiplyButton,
        ui->divideButton,
        ui->openParenButton,
        ui->closeParenButton
    };

    for (QPushButton *button : inputButtons) {
        connect(button, &QPushButton::clicked, this, [this, button]() {
            appendText(button->text());
        });
    }

    connect(ui->clearButton, &QPushButton::clicked, this, &MainWindow::clearDisplay);
    connect(ui->equalsButton, &QPushButton::clicked, this, &MainWindow::evaluateExpression);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::appendText(const QString &text)
{
    if (ui->display->text() == "0") {
        ui->display->setText(text);
        return;
    }

    if (ui->display->text() == "Error") {
        ui->display->setText(text);
        return;
    }

    ui->display->setText(ui->display->text() + text);
}

void MainWindow::evaluateExpression()
{
    QJSEngine engine;
    const QJSValue result = engine.evaluate(ui->display->text());

    if (result.isError()) {
        ui->display->setText("Error");
        return;
    }

    ui->display->setText(result.toString());
}

void MainWindow::clearDisplay()
{
    ui->display->setText("0");
}
