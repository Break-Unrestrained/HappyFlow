#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>  // show timer
#include <QTime>  // show time

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    // ---- Add Pomodoro Colock related members ----
    QTimer *timer;  // Timer object
    int currentSeconds; // Current countdown in seconds
    bool isPaused;  // Paused state

    // ---- Add Todo List function ----
    void loadTodoItems();
    void saveTodoItems();

private slots:
    // ---- Add Pomodoro Clock slot function declarations ----
    void updateTimer();  // Update timer display
    void onStartButtonClicked();  // Start button click event
    void onPauseButtonClicked();  // Pause button click event
    void onResetButtonClicked();  // Reset button click event

    void onAddTodoButtonClicked();
    void onDeleteTodoButtonClicked();
};
#endif // MAINWINDOW_H
