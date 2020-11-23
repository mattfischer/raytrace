#include "MainWindow.hpp"
#include "ui_MainWindow.h"

#include "Parse/Parser.hpp"

#include <QPainter>
#include <QDir>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(&mTimer, &QTimer::timeout, this, &MainWindow::on_timer);

    QStringList scenes = QDir::current().entryList(QStringList() << "scene_*.txt");
    ui->scene->addItems(scenes);
}

MainWindow::~MainWindow()
{
    if(mEngine) {
        mEngine->stop();
    }

    delete ui;
}

void MainWindow::on_enableLighting_clicked(bool checked)
{
    ui->groupLighting->setEnabled(checked);
}

void MainWindow::on_indirectIrradianceCaching_clicked(bool checked)
{
    ui->groupIrradianceCaching->setEnabled(checked);
}

void MainWindow::on_render_clicked()
{
    if (mEngine && mEngine->rendering()) {
        mEngine->stop();
    }
    else {
        refreshSettings();

        mScene = Parse::Parser::parse(ui->scene->currentText().toStdString());
        mEngine = std::make_unique<Render::Engine>(*mScene);
        mEngine->setSettings(mSettings);

        updateFramebuffer();
        mEngine->startRender(this);
        ui->render->setText("Stop Rendering");
        mTimer.start(100);
    }
}

void MainWindow::on_timer()
{
    QPainter renderPainter(&mRenderPixmap);
    renderPainter.drawImage(0, 0, mRenderImage);
    ui->renderView->setPixmap(mRenderPixmap);

    QPainter sampleStatusPainter(&mSampleStatusPixmap);
    sampleStatusPainter.drawImage(0, 0, mSampleStatusImage);
    ui->sampleStatusView->setPixmap(mSampleStatusPixmap);

    if(mEngine && !mEngine->rendering()) {
        mTimer.stop();
    }
}

void MainWindow::onRenderDone()
{
    ui->render->setText("Render");
}

void MainWindow::onRenderStatus(const char *message)
{
    ui->statusbar->showMessage(message);
}

void MainWindow::refreshSettings()
{
    mSettings.width = ui->width->value();
    mSettings.height = ui->height->value();
    mSettings.lighting = ui->enableLighting->isChecked();
    mSettings.minSamples = ui->samplesMin->value();
    mSettings.maxSamples = ui->samplesMax->value();
    mSettings.sampleThreshold = ui->samplesThreshold->value();
    mSettings.lighterSettings.irradianceCaching = ui->indirectIrradianceCaching->isChecked();
    mSettings.lighterSettings.indirectSamples = ui->irradianceCachingSamples->value();
    mSettings.lighterSettings.irradianceCacheThreshold = ui->irradianceCachingThreshold->value();
}

void MainWindow::updateFramebuffer()
{
    ui->renderView->setMinimumSize(mSettings.width, mSettings.height);
    mRenderImage = QImage(mEngine->renderFramebuffer().bits(), mEngine->renderFramebuffer().width(), mEngine->renderFramebuffer().height(),
                    mEngine->renderFramebuffer().width() * 3, QImage::Format_RGB888);
    mRenderPixmap = QPixmap(mEngine->renderFramebuffer().width(), mEngine->renderFramebuffer().height());

    ui->sampleStatusView->setMinimumSize(mSettings.width, mSettings.height);
    mSampleStatusImage = QImage(mEngine->sampleStatusFramebuffer().bits(), mEngine->sampleStatusFramebuffer().width(), mEngine->sampleStatusFramebuffer().height(),
                    mEngine->sampleStatusFramebuffer().width() * 3, QImage::Format_RGB888);
    mSampleStatusPixmap = QPixmap(mEngine->sampleStatusFramebuffer().width(), mEngine->sampleStatusFramebuffer().height());
}

void MainWindow::on_save_clicked()
{
    QString filename = QFileDialog::getSaveFileName(nullptr, QString(), QString(), "PNG Files (*.png)");
    if(filename != "") {
        mRenderPixmap.save(filename);
    }
}
