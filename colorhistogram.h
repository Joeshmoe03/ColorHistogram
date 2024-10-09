#ifndef COLORHISTOGRAM_H
#define COLORHISTOGRAM_H

#include <QtWidgets>
#include <QLabel>

class ColorHistogram : public QWidget
{
    Q_OBJECT
    QImage image;
    QVector<int> hist;
    QVector<QPixmap> pixmapSlices;

    QLabel *histLabel;
    QComboBox *thresholder;
    QSlider *colorValSlider;
    QComboBox *colorSelector;

public:
    ColorHistogram(const QImage &_image);
    ~ColorHistogram();

    void CountColors();

public slots:
    void generateSlices();
    void showSlice(int colorVal);

signals:
    void mouseMoved(QPoint pos, QColor color);
};

#endif // COLORHISTOGRAM_H
