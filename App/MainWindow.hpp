#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QTimer>

#include "Object/Scene.hpp"
#include "Render/Engine.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public Render::Engine::Listener
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void on_lightingIndirect_clicked(bool checked);
	void on_enableLighting_clicked(bool checked);
	void on_lightingSpecular_clicked(bool checked);
	void on_indirectIrradianceCaching_clicked(bool checked);
	void on_render_clicked();
	void on_timer();

	void refreshSettings();
	void updateFramebuffer();

	void onRenderDone();
	void onRenderStatus(const char *message);

private:
	Ui::MainWindow *ui;
	std::unique_ptr<Object::Scene> mScene;
	std::unique_ptr<Render::Engine> mEngine;
	Render::Settings mSettings;
	QImage mImage;
	QPixmap mPixmap;
	QTimer mTimer;
};

#endif // MAINWINDOW_HPP
