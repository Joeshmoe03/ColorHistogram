#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QGraphicsView>

class ImageViewer : public QGraphicsView
{
    Q_OBJECT

    QGraphicsScene scene;
    QGraphicsPixmapItem *giPixmap;
    QImage image;
    double scale;


public:
    ImageViewer(const QImage &_image);

protected: // virtual methods can be overridden
    void keyPressEvent(QKeyEvent *evt) override;
    void mouseMoveEvent(QMouseEvent *evt) override;

signals:
    void mouseMoved(QPoint pos, QColor color);
};

#endif // IMAGEVIEWER_H
