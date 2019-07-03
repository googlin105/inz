#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "secdialog.h"

#include <uhd/utils/thread.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/exception.hpp>
#include <uhd/types/tune_request.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>

#include <fftw3.h>

#define REAL 0
#define IMAG 1
#define MEGA 1000000

double max=0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    rate=1e6;
    freq=1037e5;
    gain=30;
    bw=1e6;
    num_bins=1024;

    ui->setupUi(this);
    setAxis();
    ui->plot1->xAxis->setLabel("czas [us]");
    ui->plot2->xAxis->setLabel("czestotliwosc [MHz]");
    ui->btn_stop->setEnabled(false);
    ui->plot1->setInteraction(QCP::iRangeZoom);
    ui->plot2->setInteraction(QCP::iRangeZoom);
    ui->plot1->axisRect()->setRangeZoom(Qt::Vertical);
    ui->plot2->axisRect()->setRangeZoom(Qt::Vertical);


    connect( ui->btn_stop, SIGNAL( clicked() ),
             this, SLOT( killLoop() ) );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::plot()
{
    ui->plot1->graph(0)->setData(qv_x_tc,qv_tc);
    ui->plot1->replot();
    ui->plot1->update();
    ui->plot2->graph(0)->setData(qv_x_fft,qv_fft);
    ui->plot2->replot();
    ui->plot2->update();
}

void MainWindow::clearData()
{
    qv_x_tc.clear();
    qv_x_fft.clear();
    qv_tc.clear();
    qv_fft.clear();
}

void MainWindow::save()
{
}

void MainWindow::on_btn_start_clicked()
{
    uhd::set_thread_priority_safe();


    killLoopFlag_ = false;

    double fft_left = freq/MEGA-rate/(MEGA*2);
    double fft_right = freq/MEGA+rate/(MEGA*2);
    double fft_band = fft_right-fft_left;
    double fft_step = fft_band/num_bins;
    double m=0;

    setAxis();
    ui->btn_stop->setEnabled(true);
    ui->btn_start->setEnabled(false);
    ui->btn_set->setEnabled(false);
    ui->btn_save->setEnabled(false);

    fftw_complex x[num_bins];
    fftw_complex y[num_bins];

    args="addr=192.168.101.4";
    subdev="A:0";
    ant="TX/RX";
    ref="internal";

    std::cout << std::endl;
    std::cout << boost::format("Creating the usrp device with: %s...") % args << std::endl;
    usrp = uhd::usrp::multi_usrp::make(args);

    std::cout << boost::format("Lock mboard clocks: %f") % ref << std::endl;
    usrp->set_clock_source(ref);

    //always select the subdevice first, the channel mapping affects the other settings
    std::cout << boost::format("subdev set to: %f") % subdev << std::endl;
    usrp->set_rx_subdev_spec(subdev);
    std::cout << boost::format("Using Device: %s") % usrp->get_pp_string() << std::endl;

    //set the sample rate
    if (rate <= 0.0) {
        std::cerr << "Please specify a valid sample rate" << std::endl;
        //return ~0;
    }

    // set sample rate
    std::cout << boost::format("Setting RX Rate: %f Msps...") % (rate / 1e6) << std::endl;
    usrp->set_rx_rate(rate);
    std::cout << boost::format("Actual RX Rate: %f Msps...") % (usrp->get_rx_rate() / 1e6) << std::endl << std::endl;

    // set freq
    std::cout << boost::format("Setting RX Freq: %f MHz...") % (freq / 1e6) << std::endl;
    uhd::tune_request_t tune_request(freq);
    usrp->set_rx_freq(tune_request);
    std::cout << boost::format("Actual RX Freq: %f MHz...") % (usrp->get_rx_freq() / 1e6) << std::endl << std::endl;

    // set the rf gain
    std::cout << boost::format("Setting RX Gain: %f dB...") % gain << std::endl;
    usrp->set_rx_gain(gain);
    std::cout << boost::format("Actual RX Gain: %f dB...") % usrp->get_rx_gain() << std::endl << std::endl;

    // set the IF filter bandwidth
    std::cout << boost::format("Setting RX Bandwidth: %f MHz...") % (bw / 1e6) << std::endl;
    usrp->set_rx_bandwidth(bw);
    std::cout << boost::format("Actual RX Bandwidth: %f MHz...") % (usrp->get_rx_bandwidth() / 1e6) << std::endl << std::endl;

    // set the antenna
    std::cout << boost::format("Setting RX Antenna: %s") % ant << std::endl;
    usrp->set_rx_antenna(ant);
    std::cout << boost::format("Actual RX Antenna: %s") % usrp->get_rx_antenna() << std::endl << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::vector<std::string> sensor_names;
    sensor_names = usrp->get_rx_sensor_names(0);
    if (std::find(sensor_names.begin(), sensor_names.end(), "lo_locked")
        != sensor_names.end()) {
        uhd::sensor_value_t lo_locked = usrp->get_rx_sensor("lo_locked", 0);
        std::cout << boost::format("Checking RX: %s ...") % lo_locked.to_pp_string()
                  << std::endl;
        UHD_ASSERT_THROW(lo_locked.to_bool());
    }

    uhd::stream_args_t stream_args("fc32");
    uhd::rx_streamer::sptr rx_stream = usrp->get_rx_stream(stream_args);

    uhd::rx_metadata_t md;
    std::vector<std::complex<float>> buff(num_bins);
    rx_stream->issue_stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);

    auto next_refresh = std::chrono::high_resolution_clock::now();

    while(1){
        size_t num_rx_samps = rx_stream->recv(&buff.front(), buff.size(), md);
        if (num_rx_samps != buff.size())
            continue;

        if (std::chrono::high_resolution_clock::now() < next_refresh) {
            continue;
        }
        next_refresh = std::chrono::high_resolution_clock::now()
                       + std::chrono::microseconds(int64_t(1e6 /3 ));

        for(int i=0;i<int(num_bins);i++){
            qv_tc.push_back(double(buff[i].real()));
            qv_x_tc.push_back(i*(1/rate)*MEGA);
            qv_x_fft.push_back(fft_left+i*fft_step);
            //qv_x_fft.push_back(i);
            x[i][REAL] = double(buff[i].real());
            x[i][IMAG] = double(buff[i].imag());

            m=m+qv_tc[i]*qv_tc[i];
        }
        std::cout<<20*log10(m/num_bins)<<std::endl;


        fftw_plan plan = fftw_plan_dft_1d(int(num_bins),x,y,FFTW_FORWARD,FFTW_ESTIMATE);
        fftw_execute(plan);
        fftw_destroy_plan(plan);
        fftw_cleanup();

        for(int i=int(num_bins/2);i<int(num_bins);i++){
            qv_fft.push_back(sqrt(y[i][REAL]*y[i][REAL]+y[i][IMAG]*y[i][IMAG]));
        }


        for(int i=0;i<int(num_bins/2);i++){
            qv_fft.push_back(sqrt(y[i][REAL]*y[i][REAL]+y[i][IMAG]*y[i][IMAG]));
        }


        plot();
        clearData();
        m=0;



        QApplication::processEvents();
            if ( killLoopFlag_ ) {
                ui->btn_start->setEnabled(true);
                ui->btn_set->setEnabled(true);
                ui->btn_save->setEnabled(true);
                break;
            }

    }


}

void MainWindow::on_btn_set_clicked()
{
    SecDialog secDialog(&args,&rate,&gain,&freq,&bw);
    secDialog.setModal(true);
    secDialog.exec();
    setAxis();
}


void MainWindow::setAxis()
{
    ui->plot1->addGraph();
    ui->plot2->addGraph();
    ui->plot1->xAxis->setRange(0,(1/rate)*num_bins*MEGA);
    //ui->plot1->xAxis->setRange(0,1024);
    ui->plot1->yAxis->setRange(-1, 1);
    ui->plot2->xAxis->setRange((freq-(rate/2))/MEGA,(freq+(rate/2))/MEGA);
    //ui->plot2->xAxis->setRange(0,1024);
    ui->plot2->yAxis->setRange(0,1);
    ui->plot1->replot();
    ui->plot1->update();
    ui->plot2->replot();
    ui->plot2->update();

}

void MainWindow::on_btn_save_clicked()
{
    save();
}

void MainWindow::init()
{

}
