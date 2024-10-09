#include "mainwindow.h"
#include "colorhistogram.h"

#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), colorHistogram(nullptr) {

    // Open file actions for loading the image in
    QAction *openFileAct = new QAction("Open image file");
    connect(openFileAct, &QAction::triggered, this, &MainWindow::openFileSlot);
    openFileAct->setShortcut(Qt::CTRL | Qt::Key_O);

    // A menu for opening image file
    QMenu *fileMenu = new QMenu("&File");
    fileMenu->addAction(openFileAct);
    menuBar()->addMenu(fileMenu);

    // Persistent settings
    QSettings settings("JKW Systems", "Graphics1");
    dir = settings.value("dir", "").toString();
}

MainWindow::~MainWindow() {
    delete colorHistogram;
    QSettings settings("JKW Systems", "Graphics1");
    settings.setValue("dir", dir);
}

void MainWindow::openFileSlot(){
    QString fName = QFileDialog::getOpenFileName(this, "Select image file", dir, "Image files (*.png *.jpg *.jpeg *.bmp)");
    if (fName.isEmpty()) {
        return;
    }

    QImage image(fName);

    // check image is good. If not warn user and return.
    if (image.isNull()) {
        QMessageBox::information(this, "Invalid image file:", fName);
        return;
    }
    dir = QFileInfo(fName).absolutePath();

    // Set up our image viewer
    delete colorHistogram;
    colorHistogram = new ColorHistogram(image);
    connect(colorHistogram, &ColorHistogram::mouseMoved, this, &MainWindow::mouseMoved);
    setCentralWidget(colorHistogram);
}

void MainWindow::mouseMoved(QPoint pos, QColor color) {
    statusBar()->showMessage(QString("Position (%1,%2), Color (R,G,B) = (%3,%4,%5)").arg(pos.x()).arg(pos.y()).arg(color.red()).arg(color.green()).arg(color.blue()));
}
