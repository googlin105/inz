#ifndef SECDIALOG_H
#define SECDIALOG_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class SecDialog;
}

class SecDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SecDialog(std::string *args,double *rate,double *gain,double *freq,double *bw,QWidget *parent = nullptr);
    ~SecDialog();

private slots:
    void on_btn_set_clicked();

private:
    Ui::SecDialog *ui;
    double* secFreq,*secRate,*secBw,*secGain;


};

#endif // SECDIALOG_H
