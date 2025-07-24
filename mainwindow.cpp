#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // ---- Add css ----
    ui->timerLabel->setStyleSheet("font-size: 80px; color: #E74C3C; font-weight: bold;");
    QString buttonStyle = "QPushButton { background-color: #3498DB; color: white; border-radius: 5px; padding: 10px 20px; }"
                          "QPushButton:hover { background-color: #2980B9; }"
                          "QPushButton:pressed { background-color: #1F618D; }";
    ui->startButton->setStyleSheet(buttonStyle);
    ui->pauseButton->setStyleSheet(buttonStyle);
    ui->resetButton->setStyleSheet(buttonStyle);
    ui->addTodoButton->setStyleSheet(buttonStyle);
    ui->deleteTodoButton->setStyleSheet(buttonStyle);

    // 为输入框和列表设置风格
    ui->todoLineEdit->setStyleSheet("QLineEdit { border: 2px solid #3498DB; border-radius: 5px; padding: 5px; }"
                                    "QLineEdit:focus { border: 2px solid #2980B9; }");
    ui->todoListWidget->setStyleSheet("QListWidget { border: 2px solid #BDC3C7; border-radius: 5px; background-color: #ECF0F1; }"
                                      "QListWidget::item { padding: 5px; }"
                                      "QListWidget::item:selected { background-color: #85C1E9; color: black; }");

    // 设置主窗口背景色 (可选)
    this->setStyleSheet("QMainWindow { background-color: #F8F9F9; }");
    // ... （其他初始化代码）


    // ---- Init Pomodoro related members ----
    timer = new QTimer(this);  // Create timer object, with this as parent
    currentSeconds = 25 * 60;  // Initial 25 minutes, converted to seconds
    isPaused = true;  // Initial statue is paused

    // Initialize display
    QTime time(0, currentSeconds / 60, currentSeconds % 60);
    ui->timerLabel->setText(time.toString("mm:ss"));
    // Ensure pause/reset button are disabled at the start, or start is the only clickable one
    ui->startButton->setEnabled(true);
    ui->pauseButton->setEnabled(false);  // Can enable/disable here or when start button is pressed
    ui->resetButton->setEnabled(false);

    // ---- connect signals and slots ----
    // Connect the timer's timeout() signal to updateTime() slot function
    connect(timer, &QTimer::timeout, this, &MainWindow::updateTimer);

    // Connect the clicked() signal for UI buttons to their corresponding slot function
    // This form of connection can directly find the corresponding control by objectName
    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onStartButtonClicked);
    connect(ui->pauseButton, &QPushButton::clicked, this, &MainWindow::onPauseButtonClicked);
    connect(ui->resetButton, &QPushButton::clicked, this, &MainWindow::onResetButtonClicked);

    connect(ui->addTodoButton, &QPushButton::clicked, this, &MainWindow::onAddTodoButtonClicked);
    connect(ui->deleteTodoButton, &QPushButton::clicked, this, &MainWindow::onDeleteTodoButtonClicked);


    loadTodoItems();
}

MainWindow::~MainWindow()
{
    saveTodoItems();
    delete ui;
}

// ---
void MainWindow::updateTimer()
{
    if (currentSeconds > 0) {
        currentSeconds--;
        QTime time(0, currentSeconds / 60, currentSeconds % 60);
        ui->timerLabel->setText(time.toString("mm:ss"));
    } else {
        timer->stop();  //
        isPaused = true;
        //
        QMessageBox::information(this, "Pomodoro Timer", "Time Over!");
        ui->startButton->setEnabled(true);  //
        ui->pauseButton->setEnabled(false);  //
        ui->resetButton->setEnabled(true);  //
    }
}


void MainWindow::onStartButtonClicked()
{
    if (isPaused) {
        timer->start(1000);
        isPaused = false;
        ui->startButton->setText("Doing");
        ui->startButton->setEnabled(false);
        ui->pauseButton->setEnabled(true);
        ui->resetButton->setEnabled(true);
    }
    //
}

void MainWindow::onPauseButtonClicked()
{
    if (!isPaused) {
        timer->stop();  //
        isPaused = true;
        ui->startButton->setText("Resume");
        ui->startButton->setEnabled(true);
        ui->pauseButton->setEnabled(false);
        ui->resetButton->setEnabled(true);
    }
    //
}

void MainWindow::onResetButtonClicked()
{
    // if (isPaused) {
        timer->stop();
        currentSeconds = 25 * 60;
        isPaused = true;
        QTime time(0, currentSeconds / 60, currentSeconds % 60);
        ui->timerLabel->setText(time.toString("mm:ss"));
        ui->startButton->setText("Start");
        ui->startButton->setEnabled(true);
        ui->pauseButton->setEnabled(false);
        ui->resetButton->setEnabled(false);
    // }
    //
}

void MainWindow::onAddTodoButtonClicked()
{
    QString todoText = ui->todoLineEdit->text().trimmed();
    if (!todoText.isEmpty()) {
        ui->todoListWidget->addItem(todoText);
        ui->todoLineEdit->clear();
    } else {
        QMessageBox::warning(this, "Add Todo List", "Todo List can't be empty");
    }
}

void MainWindow::onDeleteTodoButtonClicked()
{
    //
    QListWidgetItem *selectedItem = ui->todoListWidget->currentItem();
    if (selectedItem) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Delete Todo List", "Are you sure delete this item?", QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            delete selectedItem;
        }
    } else  {
        QMessageBox::information(this, "Delete Todo List", "Please select a item.");
    }

}

void MainWindow::loadTodoItems()
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString filePath = dataDir + "/todo_items.txt";

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open todo_items.txt for reading, or file doesn't exist.";
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        ui->todoListWidget->addItem(line.trimmed());
    }
    file.close();
}

void MainWindow::saveTodoItems()
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString filePath = dataDir + "/todo_items.txt";

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        qDebug() << "Failed to open todo_items.txt for writing.";
        return;
    }

    QTextStream out(&file);
    for (int i = 0; i < ui->todoListWidget->count(); ++i) {
        QListWidgetItem *item = ui->todoListWidget->item(i);
        out << item->text() << "\n";
    }
    file.close();
}
