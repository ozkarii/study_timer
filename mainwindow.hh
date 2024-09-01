#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>
#include <QTimer>
#include <QFile>
#include <QTime>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_startButton_clicked();
    void on_stopButton_clicked();
    void on_resetButton_clicked();
    void on_fileToolButton_clicked();

private:
    Ui::MainWindow *ui;
    QTimer* timer;
    QString current_filename;
    unsigned long elapsed_seconds;
    QTime break_start_time;
    QTime session_start_time;

    bool first_start = true;

    void on_timeout();
    void reset_timer();
    void update_lcd();

    bool add_stop_entry();
    bool add_start_entry();
    bool add_reset_entry();

    std::string to_time_string(unsigned long secs);

};

#endif // MAINWINDOW_HH
