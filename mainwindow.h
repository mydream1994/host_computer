#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QLabel>
#include "qextserialbase.h"
#include "win_qextserialport.h"
#include <iostream>
#include <QMessageBox>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool saveFile(const QString &fileName);

private slots:
    void on_clear_1_clicked();

    void on_clear_2_clicked();

    void on_uart_btn_clicked();

    void Read_Data();

    void on_save_1_clicked();

    void on_save_2_clicked();

    void on_start_btn_clicked();

    void on_stop_btn_clicked();

private:
    Ui::MainWindow *ui;
    Win_QextSerialPort* myCom;
};

#endif // MAINWINDOW_H
