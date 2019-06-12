#include "secdialog.h"
#include "ui_secdialog.h"
#include <iostream>
#define mega 1000000

SecDialog::SecDialog(std::string *args,double *rate,double *gain,double *freq,double *bw,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SecDialog)
{
    ui->setupUi(this);
    ui->freqBox->setValue(*freq/mega);
    ui->gainBox->setValue(*gain);
    ui->bwBox->setValue(*bw/mega);
    ui->rateBox->setValue(*rate/mega);
    secBw=bw;
    secFreq=freq;
    secGain=gain;
    secRate=rate;

}

SecDialog::~SecDialog()
{
    delete ui;
}

void SecDialog::on_btn_set_clicked()
{
    *secBw=ui->bwBox->value()*mega;
    *secFreq=ui->freqBox->value()*mega;
    *secGain=ui->gainBox->value();
    *secRate=ui->rateBox->value()*mega;
    this->close();
}
