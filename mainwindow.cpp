#include "mainwindow.hh"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDate>
#include <string>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, &QTimer::timeout, this, &MainWindow::on_timeout);
    elapsed_seconds = 0;
    current_filename = "sessions/test.csv";
    connect(ui->fileLineEdit, &QLineEdit::textChanged, this,
            [this](){this->current_filename = this->ui->fileLineEdit->text();
                     this->ui->errorLabel->hide();});
    ui->errorLabel->setStyleSheet("color: red;");
    ui->errorLabel->hide();
    this->setWindowTitle("Study Timer");
    this->setFixedSize(310, 270);
    ui->stopButton->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete timer;
}

void MainWindow::on_startButton_clicked()
{
    if (timer->isActive()) {
        return;
    }

    if (not first_start) {
        ui->breakLabel->setText(QString::fromStdString(to_time_string(break_start_time.secsTo(QTime::currentTime()))));
    }
    else {
        session_start_time = QTime::currentTime();
    }

    if (not add_start_entry()) {
        ui->errorLabel->show();
        return;
    }
    first_start = false;
    timer->start();
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);

}

void MainWindow::on_stopButton_clicked()
{
    if (timer->isActive()) {
        timer->stop();
        break_start_time = QTime::currentTime();
        if(not add_stop_entry()) {
            ui->errorLabel->show();
        }
        ui->startButton->setEnabled(true);
        ui->stopButton->setEnabled(false);
    }
}

void MainWindow::on_timeout()
{
    elapsed_seconds += 1;
    update_lcd();
}

void MainWindow::update_lcd()
{
    ui->lcdNumberSec->display(int(elapsed_seconds % 60));
    ui->lcdNumberMin->display(int((elapsed_seconds / 60) % 60));
    ui->lcdNumberHour->display(int(elapsed_seconds / 60 / 60));
}

bool MainWindow::add_stop_entry()
{
    QFile file(current_filename, this);
    if (not file.open(QIODevice::Append | QIODevice::WriteOnly)) {
        return false;
    }

    std::string date = QDate::currentDate().toString(Qt::DateFormat::ISODate).toStdString();
    std::string time = QTime::currentTime().toString().toStdString();
    std::string secs = std::to_string(elapsed_seconds);
    std::string mins = std::to_string(elapsed_seconds / 60);
    std::string hours = std::to_string(elapsed_seconds / 60 / 60);

    std::string line = "Stop;" + date + ';' + time + ';' + hours + ';' + mins + ';' + secs + '\n';

    if (file.write(line.c_str()) == -1) {
        return false;
    }

    file.close();

    return true;
}

bool MainWindow::add_start_entry()
{
    QFile file(current_filename, this);
    if (not file.open(QIODevice::Append | QIODevice::WriteOnly)) {
        return false;
    }

    std::string first_line;

    if (not first_start) {
        first_line = "--- Break duration: " + to_time_string(break_start_time.secsTo(QTime::currentTime())) + '\n';
    }
    else {
        first_line = "--- Session start at: "
                     + QDate::currentDate().toString(Qt::DateFormat::ISODate).toStdString() + " "
                     + session_start_time.toString().toStdString() + '\n';
    }

    std::string date = QDate::currentDate().toString(Qt::DateFormat::ISODate).toStdString();
    std::string time = QTime::currentTime().toString().toStdString();
    std::string secs = std::to_string(elapsed_seconds);
    std::string mins = std::to_string(elapsed_seconds / 60);
    std::string hours = std::to_string(elapsed_seconds / 60 / 60);

    std::string second_line = "Start;" + date + ';' + time + ';' + hours + ';' + mins + ';' + secs + '\n';

    if (file.write(first_line.c_str()) == -1 || file.write(second_line.c_str()) == -1) {
        return false;
    }

    file.close();

    return true;
}

bool MainWindow::add_reset_entry()
{
    if (first_start) {
        return false;
    }
    QFile file(current_filename, this);
    if (not file.open(QIODevice::Append | QIODevice::WriteOnly)) {
        return false;
    }

    std::string first_line;

    first_line = "--- Session reset, studying duration: " + to_time_string(session_start_time.secsTo(QTime::currentTime())) + '\n';

    std::string date = QDate::currentDate().toString(Qt::DateFormat::ISODate).toStdString();
    std::string time = QTime::currentTime().toString().toStdString();
    std::string secs = std::to_string(elapsed_seconds);
    std::string mins = std::to_string(elapsed_seconds / 60);
    std::string hours = std::to_string(elapsed_seconds / 60 / 60);

    std::string second_line = "Reset;" + date + ';' + time + ';' + hours + ';' + mins + ';' + secs + '\n';

    if (file.write(first_line.c_str()) == -1 || file.write(second_line.c_str()) == -1) {
        return false;
    }

    file.close();

    return true;
}

std::string MainWindow::to_time_string(unsigned long secs)
{
    return std::to_string(secs / 60 / 60)
           + ":" + std::to_string((secs / 60) % 60)
           + ":" + std::to_string(secs % 60);
}

void MainWindow::reset_timer()
{
    timer->stop();
    elapsed_seconds = 0;
    update_lcd();
}

void MainWindow::on_resetButton_clicked()
{
    add_reset_entry();
    reset_timer();
    ui->stopButton->setEnabled(false);
    ui->startButton->setEnabled(true);
    first_start = true;
}


void MainWindow::on_fileToolButton_clicked()
{
    current_filename = QFileDialog::getOpenFileName(this);
    ui->fileLineEdit->setText(current_filename);

}

