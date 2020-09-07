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
		mTimer.stop();
	}
	else {
		refreshSettings();

        mScene = Parse::Parser::parse(ui->scene->currentText().toStdString());
        mEngine = std::make_unique<Render::Engine>(*mScene);
		mEngine->setSettings(mSettings);

		updateFramebuffer();
		mEngine->startRender(this);
		ui->render->setText("Stop Rendering");
		mTimer.start();
	}
}

void MainWindow::on_timer()
{
	QPainter painter(&mPixmap);
	painter.drawImage(0, 0, mImage);
	ui->renderView->setPixmap(mPixmap);
}

void MainWindow::onRenderDone()
{
	ui->render->setText("Render");
    mEngine.reset();
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
	mImage = QImage(mEngine->framebuffer().bits(), mEngine->framebuffer().width(), mEngine->framebuffer().height(),
					mEngine->framebuffer().width() * 3, QImage::Format_RGB888);
	mPixmap = QPixmap(mEngine->framebuffer().width(), mEngine->framebuffer().height());
}

void MainWindow::on_save_clicked()
{
    QString filename = QFileDialog::getSaveFileName(nullptr, QString(), QString(), "PNG Files (*.png)");
    if(filename != "") {
        mPixmap.save(filename);
    }
}
