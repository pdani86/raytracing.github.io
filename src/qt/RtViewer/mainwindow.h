#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <thread>
#include <chrono>
#include <vector>

#include "../../MyRt/render/renderer.h"

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

signals:
    void renderCompletion();
    void signal_updateImage();

private slots:
    void on_startButton_clicked();

    void on_stopButton_clicked();

    void on_renderComplete();
    void on_updateImageSignal();

private:
    Ui::MainWindow *ui;

    std::shared_ptr<Renderer> renderer;
    std::thread renderThread;

    QGraphicsScene scene;
};
#endif // MAINWINDOW_H
