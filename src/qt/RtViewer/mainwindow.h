#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <thread>
#include <chrono>
#include <vector>

#include "../../MyRt/render/renderer.h"
#include "../../MyRt/hittables/film.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void updateCamUi();
    void setCamFromUi();

    void updateGraphicsScene();

    QImage getCurrentImage();
    QImage getCurrentFilmImage();

    void processFilm();

signals:
    void renderCompletion();
    void signal_updateImage();

private slots:
    void on_startButton_clicked();

    void on_stopButton_clicked();

    void on_renderComplete();
    void on_updateImageSignal();

    void on_brightnessSlider_valueChanged(int value);

    void on_saveImageBtn_clicked();

    void on_toggleViewBtn_clicked();

    void on_calcFilmBtn_clicked();

    void on_scaleSlider_valueChanged(int value);

private:
    Ui::MainWindow *ui;

    std::shared_ptr<myrt::film> pFilm;
    bool bShowFilm = false;
    std::shared_ptr<Renderer> renderer;
    std::thread renderThread;

    QGraphicsScene scene;
    std::vector<double> lastImage;
};
#endif // MAINWINDOW_H
