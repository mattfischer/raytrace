#include "MainWindow.hpp"
#include "ui_MainWindow.h"

#include "Parse/Parser.hpp"

#include <QPainter>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	QObject::connect(&mTimer, &QTimer::timeout, this, &MainWindow::on_timer);

	mScene = Parse::Parser::parse("scene.txt");
	mEngine = std::make_unique<Render::Engine>(*mScene);
}

MainWindow::~MainWindow()
{
	mEngine->stop();

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
	if (mEngine->rendering()) {
		mEngine->stop();
		mTimer.stop();
	}
	else {
		refreshSettings();
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
