import raytrace

import sys
from PySide2 import QtCore, QtGui, QtWidgets, QtUiTools
from PySide2.QtCore import Slot

class App(QtWidgets.QApplication):
    def __init__(self, args):
        super(App, self).__init__(args)

        file = QtCore.QFile('App/MainWindow.ui')
        file.open(QtCore.QFile.ReadOnly)
        loader = QtUiTools.QUiLoader()
        self.mainwindow = loader.load(file)
        self.mainwindow.setFont(QtGui.QFont(self.mainwindow.font().family(), self.mainwindow.font().pointSize() * self.devicePixelRatio()))

        scenes = QtCore.QDir.current().entryList(['scene_*.txt'])
        self.mainwindow.scene.addItems(scenes)

        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.on_timer)

        self.engine = None
        self.scene = None
        self.settings = raytrace.Settings()
        self.renderImage = None
        self.renderPixmap = None
        self.sampleStatusImage = None
        self.sampleStatusPixmap = None

        self.mainwindow.enableLightingBox.clicked.connect(self.on_enableLighting_clicked)
        self.mainwindow.indirectIrradianceCaching.clicked.connect(self.on_indirectIrradianceCaching_clicked)
        self.mainwindow.renderButton.clicked.connect(self.on_renderButton_clicked)
        self.mainwindow.saveButton.clicked.connect(self.on_saveButton_clicked)

        self.mainwindow.show()

    @Slot()
    def on_timer(self):
        renderPainter = QtGui.QPainter(self.renderPixmap)
        renderPainter.drawImage(0, 0, self.renderImage)
        self.mainwindow.renderView.setPixmap(self.renderPixmap)

        sampleStatusPainter = QtGui.QPainter(self.sampleStatusPixmap)
        sampleStatusPainter.drawImage(0, 0, self.sampleStatusImage)
        self.mainwindow.sampleStatusView.setPixmap(self.sampleStatusPixmap)

        if self.engine and not self.engine.rendering():
            self.timer.stop()

    @Slot()
    def on_enableLighting_clicked(self, checked):
        self.mainwindow.groupLighting.setEnabled(checked)

    @Slot()
    def on_indirectIrradianceCaching_clicked(self, checked):
        self.mainwindow.groupIrradianceCaching.setEnabled(checked)

    @Slot()
    def on_renderButton_clicked(self):
        if self.engine and self.engine.rendering():
            self.engine.stop()
        else:
            self.refreshSettings()

            scene = raytrace.Scene(self.mainwindow.scene.currentText())
            self.engine = raytrace.Engine(scene)
            self.engine.set_settings(self.settings)

            self.updateFramebuffer()
            self.engine.start_render(self)
            self.mainwindow.renderButton.setText('Stop Rendering')
            self.timer.start(100)

    @Slot()
    def on_saveButton_clicked(self):
        filename = QtCore.QFileDialog.getSaveFileName(None, '', '', 'PNG Files (*.png)')
        if filename != '':
            self.renderPixmap.save(filename)

    def refreshSettings(self):
        self.settings.width = self.mainwindow.widthBox.value()
        self.settings.height = self.mainwindow.heightBox.value()
        self.settings.lighting = self.mainwindow.enableLightingBox.isChecked()
        self.settings.min_samples = self.mainwindow.samplesMinBox.value()
        self.settings.max_samples = self.mainwindow.samplesMaxBox.value()
        self.settings.sample_threshold = self.mainwindow.samplesThresholdBox.value()
        self.settings.irradiance_caching = self.mainwindow.indirectIrradianceCaching.isChecked()
        self.settings.indirect_samples = self.mainwindow.irradianceCachingSamples.value()
        self.settings.irradiance_cache_threshold = self.mainwindow.irradianceCachingThreshold.value()

    def updateFramebuffer(self):
        dpr = self.mainwindow.renderView.devicePixelRatio()
        self.mainwindow.renderView.setMinimumSize(self.settings.width / dpr, self.settings.height / dpr)
        self.renderImage = QtGui.QImage(self.engine.render_framebuffer, self.engine.render_framebuffer.width, self.engine.render_framebuffer.height,
                        self.engine.render_framebuffer.width * 3, QtGui.QImage.Format_RGB888)
        self.renderImage.setDevicePixelRatio(dpr)
        self.renderPixmap = QtGui.QPixmap(self.engine.render_framebuffer.width, self.engine.render_framebuffer.height)
        self.renderPixmap.setDevicePixelRatio(dpr)

        self.mainwindow.sampleStatusView.setMinimumSize(self.settings.width / dpr, self.settings.height / dpr)
        self.sampleStatusImage = QtGui.QImage(self.engine.sample_status_framebuffer, self.engine.sample_status_framebuffer.width, self.engine.sample_status_framebuffer.height,
                        self.engine.sample_status_framebuffer.width * 3, QtGui.QImage.Format_RGB888)
        self.sampleStatusImage.setDevicePixelRatio(dpr)
        self.sampleStatusPixmap = QtGui.QPixmap(self.engine.sample_status_framebuffer.width, self.engine.sample_status_framebuffer.height)
        self.sampleStatusPixmap.setDevicePixelRatio(dpr)

    def on_render_done(self):
        self.mainwindow.renderButton.setText('Render')

    def on_render_status(self, message):
        self.mainwindow.statusbar.showMessage(message)

if __name__ == "__main__":
    QtWidgets.QApplication.setAttribute(QtCore.Qt.AA_EnableHighDpiScaling)

    app = App(sys.argv)
    app.exec_()
