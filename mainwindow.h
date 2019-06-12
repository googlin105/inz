#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <uhd/utils/thread.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/exception.hpp>
#include <uhd/types/tune_request.hpp>

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

    void init();

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
    uhd::usrp::multi_usrp::sptr usrp;


};


#endif // MAINWINDOW_H
