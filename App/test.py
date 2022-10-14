#! /usr/bin/env python

import sys
from PySide2 import QtCore, QtGui, QtWidgets, QtUiTools

app = QtWidgets.QApplication(sys.argv)
file = QtCore.QFile('App/MainWindow.ui')
file.open(QtCore.QFile.ReadOnly)
loader = QtUiTools.QUiLoader()
mainwindow = loader.load(file)

scenes = QtCore.QDir.current().entryList(['scene_*.txt'])
mainwindow.scene.addItems(scenes)

mainwindow.show()

app.exec_()