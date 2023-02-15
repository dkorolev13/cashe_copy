#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDebug>
#include <QtMath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_source_toolButton_clicked()
{
   this->srcDirName = QFileDialog::getExistingDirectory(this, "Choose source directory", "/home/user/my_projects/");
   ui->src_path_lineEdit->setText(srcDirName);
}

void MainWindow::on_dst_toolButton_clicked()
{
    this->dstDirName = QFileDialog::getExistingDirectory(this, "Choose destanation directory", "/home/user/my_projects/");
    ui->dst_path_lineEdit->setText(dstDirName);

    logFile= new QFile(this->dstDirName + "/logFile.txt");
    logFile->open(QFile::WriteOnly | QFile::Append);
}

void MainWindow::on_copy_pushButton_clicked()
{
    double top_left_B = ui->top_left_N_doubleSpinBox->value();
    double top_left_L = ui->top_left_E_doubleSpinBox->value();

    double bottom_right_B = ui->bottom_right_N_doubleSpinBox->value();
    double bottom_right_L = ui->bottom_right_E_doubleSpinBox->value();

    int z = ui->spinBox->value();

    int topLeftTilesX, topLeftTilesY;
    topLeftTilesX = topLeftTilesY = NULL;
    int bottomRightTilesX, bottomRightTilesY;
    bottomRightTilesX = bottomRightTilesY = NULL;
    double flatX, flatY, picX, picY;
    flatX = flatY = picX = picY = NULL;

    int progressBarMaxValue = 0;

    this->GEOLatLonToXY(top_left_B, top_left_L, z, topLeftTilesX, topLeftTilesY, picX, picY, flatX, flatY);
    this->GEOLatLonToXY(bottom_right_B, bottom_right_L, z, bottomRightTilesX, bottomRightTilesY, picX, picY, flatX, flatY);

//    progressBarMaxValue = (bottomRightTilesX - topLeftTilesX + 1) * (bottomRightTilesY - topLeftTilesY + 1);

    int pBCounter = 0;
    ui->progressBar->setMaximum(ui->to_spinBox->value() - ui->from_spinBox->value() + 1);
    ui->progressBar->setValue(pBCounter);

    QStringList strLst;
    for (int i = topLeftTilesX; i <= bottomRightTilesX; i++) {
        for (int j = topLeftTilesY; j <= bottomRightTilesY; j++) {
            strLst << this->xyzToTsqr(i, j, z);
        }
    }

    qDebug() << strLst.at(0) << strLst.size();


    for (int i = ui->from_spinBox->value(); i <= ui->to_spinBox->value(); i++) {

        QString currentFileName = srcDirName + "/" + strLst.at(i) + ".jpg";
        QFile file(currentFileName);
        if(file.exists()) {
            file.copy(dstDirName + "/" + strLst.at(i) + ".jpg");

        } else {
            QTextStream logTextStream(logFile);
            logTextStream << currentFileName << "\n";
        }
        pBCounter += 1;
        ui->progressBar->setValue(pBCounter);

    }
//    for (int i = topLeftTilesX; i <= bottomRightTilesX; i++) {
//        for (int j = topLeftTilesY; j <= bottomRightTilesY; j++) {
//            QString currentFileName = srcDirName + "/" + this->xyzToTsqr(i, j, z) + ".jpg";
//            QFile file(currentFileName);
//            if(file.exists()) {
//                file.copy(dstDirName + "/" + this->xyzToTsqr(i, j, z) + ".jpg");
//                pBCounter += 1;
//                ui->progressBar->setValue(pBCounter);
//            } else {
//                QTextStream logTextStream(logFile);
//                logTextStream << currentFileName << "\n";
//            }
//        }
//    }
    logFile->close();
}

void MainWindow::GEOLatLonToXY(double B, double L, int z, int &tilesX, int &tilesY, double &picX, double &picY, double &flatX, double &flatY)
{
    double bm0 = 256 * qPow(2, z-1) / 2;

    double latrad = qDegreesToRadians(B);
    double lonrad = qDegreesToRadians(L);

    flatX = bm0 * (1 + lonrad / M_PI);
    flatY = bm0 * (1 - 0.5 * qLn((1 + qSin(latrad)) / (1 - qSin(latrad))) / M_PI);

    tilesX = qFloor(flatX) / 256;
    tilesY = qFloor(flatY) / 256;

    picX = flatX - tilesX * 256;
    picY = flatY - tilesY * 256;
}

QString MainWindow::xyzToTsqr(int tilesX, int tilesY, int z)
{
    if(z<1 || z>32)
        return QString();

    QString str = "t";
    int mask = 1 << (z - 1);
    tilesX %= mask;
    if(tilesX < 0)
        tilesX += mask;
    for(int i=2; i<=z; i++)
    {
        mask >>= 1;
        if(tilesX & mask)
        {
            if(tilesY & mask)
                str += "s";
            else
                str += "r";
        }
        else
        {
            if(tilesY & mask)
                str += "t";
            else
                str += "q";
        }
    }
    return str;
}
