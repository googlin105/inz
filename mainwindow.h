#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void plot();
    void clearData();
    void save();

private slots:
    void on_btn_start_clicked();

    void on_btn_set_clicked();


    void killLoop() {
        killLoopFlag_ = true;
    }
    void setAxis();

    void on_btn_save_clicked();

public:
    Ui::MainWindow *ui;
    QVector<double> qv_tc,qv_fft,qv_x_tc,qv_x_fft,qv_fft2;
    double rate;
    double freq;
    double gain;
    double bw;
    size_t num_bins;
    std::string args;
    std::string subdev;
    std::string ant;
    std::string ref;
    bool killLoopFlag_;


};


#endif // MAINWINDOW_H
