#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
     QString srcDirName, dstDirName;
     QFile* logFile;

private slots:
    void on_source_toolButton_clicked();

    void on_dst_toolButton_clicked();

    void on_copy_pushButton_clicked();

    //пересчитать географические координаты (B, L) в пиксели слоя, номер тайла и номер пикселя данного тайла.
    void GEOLatLonToXY(double B, double L, int z, int &tilesX, int &tilesY, double &picX, double &picY, double &flatX, double &flatY);

    //получить из номера тайла для данного слоя имя файла тайла в формате qrst
    QString xyzToTsqr(int tilesX, int tilesY, int z);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
