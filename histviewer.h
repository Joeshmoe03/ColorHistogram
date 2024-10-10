#ifndef HISTVIEWER_H
#define HISTVIEWER_H

#include <QGraphicsView>

class HistViewer : public QGraphicsView {

    Q_OBJECT

    QGraphicsScene scene;
    QGraphicsPixmapItem *giPixmap;
    QPixmap pixmap;

public:
    HistViewer(QPixmap &_pixmap);
    void updateHistogram(QPixmap &newHistogram);

protected:
    void mouseMoveEvent(QMouseEvent *evt) override;

signals:
    void mouseMoved(QPoint pos);
};

#endif // HISTVIEWER_H
