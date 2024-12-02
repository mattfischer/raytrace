import raytrace

import sys
import math
from PySide2 import QtCore, QtGui, QtWidgets, QtUiTools
from PySide2.QtCore import Slot

class App(QtWidgets.QApplication):
    def __init__(self, main_window_ui, args):
        super(App, self).__init__(args)

        ba = QtCore.QByteArray(main_window_ui.encode())
        file = QtCore.QBuffer(ba)
        file.open(QtCore.QIODevice.ReadOnly)
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

        self.mainwindow.renderMethodIrradianceCaching.toggled.connect(self.on_renderMethodIrradianceCaching_toggled)
        self.mainwindow.renderMethodRestir.toggled.connect(self.on_renderMethodRestir_toggled)
        self.mainwindow.renderButton.clicked.connect(self.on_renderButton_clicked)
        self.mainwindow.saveButton.clicked.connect(self.on_saveButton_clicked)
        self.mainwindow.renderView.installEventFilter(self)

        self.lightprobe = LightProbeDialog(self.mainwindow)

        self.mainwindow.show()

    @Slot()
    def on_timer(self):
        '''renderPainter = QtGui.QPainter(self.renderPixmap)
        renderPainter.drawImage(0, 0, self.renderImage)
        self.mainwindow.renderView.setPixmap(self.renderPixmap)

        if self.engine and not self.engine.rendering():
            self.timer.stop()
        '''

    @Slot()
    def on_renderMethodIrradianceCaching_toggled(self, checked):
        self.mainwindow.groupIrradianceCaching.setEnabled(checked)

    @Slot()
    def on_renderMethodRestir_toggled(self, checked):
        self.mainwindow.groupRestir.setEnabled(checked)

    @Slot()
    def on_renderButton_clicked(self):
        if self.engine and self.engine.rendering():
            self.engine.stop()
            self.mainwindow.renderButton.setText('Render')
        else:
            self.refreshSettings()

            scene = raytrace.Scene(self.mainwindow.scene.currentText())
            self.engine = raytrace.Engine(scene, self.settings)

            self.updateFramebuffer()
            self.engine.start_render(self)
            self.mainwindow.statusbar.showMessage('')
            self.mainwindow.renderButton.setText('Stop Rendering')
            self.timer.start(100)

    @Slot()
    def on_saveButton_clicked(self):
        (filename, _) = QtWidgets.QFileDialog.getSaveFileName(None, '', '', 'PNG Files (*.png)')
        if filename != '':
            self.renderPixmap.save(filename)

    @Slot()
    def eventFilter(self, watched, event):
        if event.type() == QtCore.QEvent.MouseButtonRelease:
            self.lightprobe.show()
            dpr = self.mainwindow.renderView.devicePixelRatio()
            self.lightprobe.renderProbe(self.engine, event.x() * dpr, event.y() * dpr)

        return False

    def refreshSettings(self):
        self.settings.width = self.mainwindow.widthBox.value()
        self.settings.height = self.mainwindow.heightBox.value()
        self.settings.samples = self.mainwindow.samplesBox.value()

        render_methods = [
            (self.mainwindow.renderMethodNoLighting, 'noLighting'),
            (self.mainwindow.renderMethodDirectLighting, 'directLighting'),
            (self.mainwindow.renderMethodPathTracingCpu, 'pathTracingCpu'),
            (self.mainwindow.renderMethodPathTracingGpu, 'pathTracingGpu'),
            (self.mainwindow.renderMethodRestir, 'restir'),
            (self.mainwindow.renderMethodIrradianceCaching, 'irradianceCaching')
        ]
        for (widget, name) in render_methods:
            if widget.isChecked():
                self.settings.render_method = name
                break

        self.settings.irradiance_cache_samples = self.mainwindow.irradianceCachingSamples.value()
        self.settings.irradiance_cache_threshold = self.mainwindow.irradianceCachingThreshold.value()

        self.settings.restir_indirect_samples = self.mainwindow.restirIndirectSamples.value()
        self.settings.restir_radius = self.mainwindow.restirRadius.value()
        self.settings.restir_candidates = self.mainwindow.restirCandidates.value()

    def updateFramebuffer(self):
        '''dpr = self.mainwindow.renderView.devicePixelRatio()
        self.mainwindow.renderView.setMinimumSize(self.settings.width / dpr, self.settings.height / dpr)
        self.renderImage = QtGui.QImage(self.engine.render_framebuffer, self.engine.render_framebuffer.width, self.engine.render_framebuffer.height,
                        self.engine.render_framebuffer.width * 3, QtGui.QImage.Format_RGB888)
        self.renderImage.setDevicePixelRatio(dpr)
        self.renderPixmap = QtGui.QPixmap(self.engine.render_framebuffer.width, self.engine.render_framebuffer.height)
        self.renderPixmap.setDevicePixelRatio(dpr)
        '''

    def on_render_done(self, total_time_seconds):
        seconds = total_time_seconds
        hours = int(seconds / 3600)
        seconds -= hours * 3600
        minutes = int(seconds / 60)
        seconds -= minutes * 60
        if hours > 0:
            message = 'Render time: %ih %im %is' % (hours, minutes, seconds)
        elif minutes > 0:
            message = 'Render time: %im %is' % (minutes, seconds)
        else:
            message = 'Render time: %.03fs' % seconds
        self.mainwindow.statusbar.showMessage(message)

        self.mainwindow.renderButton.setText('Render')

class LightProbeDialog(QtWidgets.QDialog):
    def __init__(self, parent):
        super(LightProbeDialog, self).__init__(parent, QtCore.Qt.WindowTitleHint | QtCore.Qt.WindowCloseButtonHint)
        self.setWindowTitle('Light Probe')
        self.setFixedSize(400, 400)
        self.samples = []

    def paintEvent(self, event):
        size = self.width() - 20
        painter = QtGui.QPainter(self)
        for sample in self.samples:
            (color, azimuth, elevation) = sample
            radius = math.cos(elevation) * size / 2
            x = math.cos(azimuth) * radius + size / 2 + 10
            y = math.sin(azimuth) * radius + size / 2 + 10
            painter.setBrush(QtGui.QColor(color[0] * 255, color[1] * 255, color[2] * 255))
            painter.drawEllipse(x, y, 20, 20)
        painter.end()

    def renderProbe(self, engine, x, y):
        self.samples = engine.render_probe(x, y)
        self.update()

def run_app(main_window_ui):
    app = App(main_window_ui, sys.argv)
    app.exec_()
