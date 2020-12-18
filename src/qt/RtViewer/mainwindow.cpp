#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <algorithm>

#include <QThread>
#include <QDateTime>
#include <QMessageBox>

#include "vec3.h"
#include "camera.h"
#include "world_example.h"

#include "bmp.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    int width = 600;
    int height = 600;

    ui->graphicsView->setScene(&scene);
    //ui->graphicsView->scale(1.0, -1.0);

    //point3 lookFrom(0.0, 0.0, 0.0);
    //point3 lookAt(0.0, 0.0, 1.0);
    point3 lookUp(0.0, 1.0, 0.0);

    point3 lookFrom(0.0, 75.0, -400.0);
    point3 lookAt(0.0, 50.0, 0.0);

    double vFovDegree = 50.0;

    double aspect = width / (double)height;
    double aperture = 0.0; // pinhole cam
    double focus_dist = 1.0; // pinhole -> irrelevant
    Camera cam(
                lookFrom, lookAt, lookUp,
                vFovDegree, aspect,
                aperture, focus_dist
                );
    auto exampleScene = createExampleWorld();
    renderer = std::make_shared<Renderer>(width, height, cam);
    renderer->scene = std::make_shared<Scene>();
    renderer->scene->background = color(0.0, 0.0, 0.0);
    renderer->max_depth = 1; // ray bounce limit
    renderer->samples_per_pixel = 1;
    renderer->scene = std::make_shared<Scene>(exampleScene);

    connect(this, SIGNAL(renderCompletion()), this, SLOT(on_renderComplete()), Qt::QueuedConnection);
    connect(this, SIGNAL(signal_updateImage()), this, SLOT(on_updateImageSignal()), Qt::QueuedConnection);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startButton_clicked()
{
    setCamFromUi();
    renderer->image_width = ui->resX->value();
    renderer->image_height = ui->resY->value();

    if(renderThread.joinable())
        renderThread.join();
    renderThread = std::thread([=]() {
        auto renderThreads = renderer->renderMultiThreaded(4, true);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        while(renderer->isRenderInProgress()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(400));
            emit signal_updateImage();
        }
        for(auto& curThread : renderThreads) {
            curThread.join();
        }
        emit renderCompletion();
    });
}

void MainWindow::on_renderComplete() {
    on_updateImageSignal();
    if(renderThread.joinable())
        renderThread.join();
}

void MainWindow::on_updateImageSignal() {
    {
        auto& renderResult = renderer->getImage();
        auto imageLock = renderer->lockImage();
        lastImage = renderResult;

    }
    updateGraphicsScene();
}

void MainWindow::on_stopButton_clicked()
{
    renderer->requestRenderStop();
}

void MainWindow::updateCamUi() {

}

void MainWindow::setCamFromUi() {
    vec3 lookFrom(ui->camX->value(), ui->camY->value(), ui->camZ->value());
    vec3 lookAt(ui->lookX->value(), ui->lookY->value(), ui->lookZ->value());
    vec3 lookUp(0.0, 1.0, 0.0);
    double vFov = ui->vFov->value();
    double aspect = ui->resX->value() / (double) ui->resY->value();
    double focusDist = 10.0;

    renderer->max_depth = ui->maxRayBounce->value();

    renderer->cam.update(
                lookFrom,
                lookAt,
                lookUp,
                vFov,
                aspect,
                focusDist
                );
}

QImage MainWindow::getCurrentImage() {
    std::vector<unsigned char> bitmapData;
    //bitmapData = BMP::mapToBytePerChannelNormalize(renderResult);
    int brightnessVal = ui->brightnessSlider->value() - ui->brightnessSlider->maximum() / 2;
    double brightnessScale = 1000 * std::pow(2.0, brightnessVal/(double)20.0);
    bitmapData = BMP::mapToBytePerChannel(lastImage, brightnessScale);
    QImage image(renderer->image_width, renderer->image_height, QImage::Format_RGB888);
    //std::cerr << "image size: " << std::to_string(image.sizeInBytes()) << "\n";
    std::size_t imageSize = 3 * renderer->image_width * renderer->image_height;
    memcpy(image.bits(), bitmapData.data(), std::min(imageSize, bitmapData.size()));
    return image;
}

void MainWindow::updateGraphicsScene() {
    scene.clear();
    std::vector<unsigned char> bitmapData;
    QImage image = getCurrentImage();
    QPixmap pm = QPixmap::fromImage(image);
    scene.addPixmap(pm);
}

void MainWindow::on_brightnessSlider_valueChanged(int value)
{
    updateGraphicsScene();
}

void MainWindow::on_saveImageBtn_clicked()
{
    //if(renderer->isRenderInProgress()) return;
    QImage curImage = getCurrentImage();
    auto now = QDateTime::currentDateTime();
    QString dateStr = QString("%1%2%3_%4%5%6")
            .arg(now.date().year())
            .arg(now.date().month(), 2, 10, QChar('0'))
            .arg(now.date().day(), 2, 10, QChar('0'))
            .arg(now.time().hour(), 2, 10, QChar('0'))
            .arg(now.time().minute(), 2, 10, QChar('0'))
            .arg(now.time().second(), 2, 10, QChar('0'))
            ;
    QString filename = QString("image_%1.png").arg(dateStr);
    if(curImage.save(filename, "PNG")) {
        QMessageBox::information(this, "Image Saved", QString("Image Saved As %1").arg(filename));
    } else {
        QMessageBox::critical(this, "Couldn't Save Image", QString("Couldn't Save Image As %1").arg(filename));
    }
}
