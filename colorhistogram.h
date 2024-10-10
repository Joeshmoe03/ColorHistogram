#ifndef COLORHISTOGRAM_H
#define COLORHISTOGRAM_H

#include "histviewer.h"
#include <QtWidgets>
#include <QLabel>

class ColorHistogram : public QWidget
{
    Q_OBJECT
    QImage image;
    QVector<int> hist;
    QVector<QPixmap> pixmapSlices;

    QStatusBar *statusBarView;
    QStatusBar *statusBarHist;

    //QLabel *histLabel;

    HistViewer *histViewer;
    QSlider *thresholder;
    QSlider *colorValSlider;
    QComboBox *colorSelector;

public:
    ColorHistogram(const QImage &_image);
    ~ColorHistogram();

    void CountColors();

public slots:
    void generateSlices();
    void showSlice(int colorVal);
    void mouseMoveView(QPoint pos, QColor color);
    void mouseMoveHist(QPoint pos);
};

#endif // COLORHISTOGRAM_H
