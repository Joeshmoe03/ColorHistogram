#include "histviewer.h"

#include <QtWidgets>

HistViewer::HistViewer(QPixmap &_pixmap) : pixmap(_pixmap) {
    setScene(&scene);
    giPixmap = scene.addPixmap(pixmap);

    setSceneRect(QRectF(QPointF(0,0), pixmap.size()));
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setMouseTracking(true);
}

/* React to mouse movement event for the ImageViewer */
void HistViewer::mouseMoveEvent(QMouseEvent *evt) {
    QPoint pos = (giPixmap->mapFromScene(mapToScene(evt->pos()))-QPointF(0.5, 0.5)).toPoint();
    QImage pixmapImg = pixmap.toImage();

    if (pixmapImg.valid(pos))
        emit mouseMoved(pos);
}

/* Updatre the histogram */
void HistViewer::updateHistogram(const QPixmap &newHistogram) {
    giPixmap->setPixmap(newHistogram);
}


