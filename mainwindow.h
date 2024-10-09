#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "colorhistogram.h"
#include <QMainWindow>
#include <QGraphicsScene>

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QString dir;
    ColorHistogram *colorHistogram;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void openFileSlot();
    void mouseMoved(QPoint pos, QColor color);
};
#endif // MAINWINDOW_H
