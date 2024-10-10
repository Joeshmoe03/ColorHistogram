#include "colorhistogram.h"
#include "imageviewer.h"
#include "histviewer.h"

#include <algorithm>
#include <cmath>
#include <QtWidgets>

ColorHistogram::ColorHistogram(const QImage &_image):image(_image), hist(1 << 24, 0), pixmapSlices(256) {

    // Set up layouts for ColorHistogram widget
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    QVBoxLayout *viewLayout = new QVBoxLayout;
    QVBoxLayout *histLayout = new QVBoxLayout;

    statusBarView = new QStatusBar;
    statusBarHist = new QStatusBar;

    // Add side padding to histLayout and force it to hug center of window
    histLayout->setContentsMargins(20, 0, 20, 0);
    histLayout->setAlignment(Qt::AlignCenter);

    // Add our viewLayout on the left side of the main layout, and histLayout on the right
    mainLayout->addLayout(viewLayout);
    mainLayout->addLayout(histLayout);

    // Add our imageViewer to the left hand side of the screen in our viewLayout
    ImageViewer *imageViewer = new ImageViewer(image);
    imageViewer->setMinimumSize(384, 256);
    connect(imageViewer, &ImageViewer::mouseMoved, this, &ColorHistogram::mouseMoveView);
    viewLayout->addWidget(imageViewer);
    viewLayout->addWidget(statusBarView);

    // Add our ColorHistogram elements into the right hand side of screen (histLayout)
    QPixmap histPixmap(256, 256);
    histPixmap.fill(Qt::transparent);
    histViewer = new HistViewer(histPixmap);
    connect(histViewer, &HistViewer::mouseMoved, this, &ColorHistogram::mouseMoveHist);
    histViewer->setMinimumSize(258, 258);
    histViewer->updateHistogram(histPixmap);
    histLayout->addWidget(histViewer);
    histLayout->addWidget(statusBarHist);

    // populate our color count vector before generating slices
    CountColors();

    // Slider label
    QLabel *valueLabel = new QLabel;
    valueLabel->setText("Color value: ");
    histLayout->addWidget(valueLabel);

    // Our color slider
    colorValSlider = new QSlider;
    colorValSlider->setOrientation(Qt::Horizontal);
    colorValSlider->setMaximum(255);
    connect(colorValSlider, &QSlider::sliderMoved, this, &ColorHistogram::showSlice);
    histLayout->addWidget(colorValSlider);

    // Dropdown label
    QLabel *colorLabel = new QLabel;
    colorLabel->setText("Color slice: ");
    histLayout->addWidget(colorLabel);

    // Threshold widget for frequency based pixel intensity
    thresholder = new QSlider;
    thresholder->setRange(0, 255);
    thresholder->setOrientation(Qt::Horizontal);
    thresholder->setValue(255);
    connect(thresholder, &QSlider::sliderReleased, this, &ColorHistogram::generateSlices);

    // our dropdown box for slice selection
    colorSelector = new QComboBox;
    QStringList colorOptions = {"Red", "Green", "Blue"};
    colorSelector->addItems(colorOptions);
    generateSlices(); // Generate slices on the first time as we default to first colorOption (red) on startup
    connect(colorSelector, &QComboBox::currentIndexChanged, this, &ColorHistogram::generateSlices); // Slices should oonly be changed afterwards by manual changes to QComboBox
    histLayout->addWidget(colorSelector);

    // Thresholding widget for adjusting pixel color intensity at bottom of all widgets + label
    QLabel *thresholderLabel = new QLabel;
    thresholderLabel->setText("Threshold: ");
    histLayout->addWidget(thresholderLabel);
    histLayout->addWidget(thresholder);
}

ColorHistogram::~ColorHistogram() {}

// Populate hist, which is vector tracking color frequencies
void ColorHistogram::CountColors() {

    // Reset histogram frequency on new image
    hist.assign(1 << 24, 0);

    // Color variables and img properties stuff
    QRgb pixelColor;
    int red; int green; int blue;
    int colorIdx;
    int width = image.width();
    int height = image.height();

    // iterate thru image and track with hist the freq.
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            pixelColor = image.pixel(x, y);

            // individual colors of RGB
            red = qRed(pixelColor);
            green = qGreen(pixelColor);
            blue = qBlue(pixelColor);

            // bit shift where we have BBBBBBBB GGGGGGGG RRRRRRRR for our 24 bit index for our frequency array. Increment the array at corresponding color.
            colorIdx = (blue << 16) | (green << 8) | red;
            ++hist[colorIdx];
        }
    }
}

// Update the UI to show the slice based on the slider
void ColorHistogram::showSlice(int colorVal) {
    if (colorVal >= 0 && colorVal < pixmapSlices.size()) {
        histViewer->updateHistogram(pixmapSlices[colorVal]);
    }
}

// Regenerate value slices on given color slice (R or G or B). Slot action is called on ComboBox change or on start.
void ColorHistogram::generateSlices() {
    // get current slice index 0, 1, 2 = R, G, B
    int sliceIdx = colorSelector->currentIndex();

    // Given dimensions of pixmap and other color vars for populating pixmap
    // Indexing of hist is by BBBBBBBB GGGGGGGG RRRRRRRR (bits)
    int width = 256; int height = 256;
    int red; int green; int blue;
    int threshold = thresholder->sliderPosition();
    int colorIdx;
    int colorFreq;

    // Iteratr thru all 255 pixmaps based on color value for given color slice R, G, or B
    for (int colorVal = 0; colorVal < 256; ++colorVal) {

        // Modify coorresponding pixmap of pixmapSlices to given color value + set default stuff initially
        QPixmap &slicePixmap = pixmapSlices[colorVal];
        slicePixmap = QPixmap(256, 256);
        slicePixmap.fill(Qt::transparent);
        QImage tempImg = slicePixmap.toImage();

        // Itrate x,y of Pixmap and set value of pix
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {

                // ComboBox idx is set to 0 AKA "red"
                switch(sliceIdx) {
                case 0:
                    red = colorVal; // we are iterating over red's colorVal when genr. pixmaps of "red"
                    green = y; // green will be along y axis when displayed histogram set on "red" ComboBox
                    blue = x; // blue will be along x axis when displayed histogram set on "red" ComboBox
                    break;

                // ComboBox idx is set to 1 AKA "green"
                case 1:
                    red = x;
                    green = colorVal;
                    blue = y;
                    break;

                // ComboBox idx is set to 2 AKA "blue"
                case 2:
                    red = x;
                    green = y;
                    blue = colorVal;
                    break;
                }

                // Indexing of hist (frequencies colors) happens like so: BBBBBBBB GGGGGGGG RRRRRRRR (bits in 24 bit).
                // We want to get freq for given x, y, and colorVal from slider by the index.
                colorIdx = (blue << 16) | (green << 8) | red;
                colorFreq = hist[colorIdx];

                // Set color according to freq as it appears in photo black or white or in between
                int intensity = colorFreq * threshold;

                // https://stackoverflow.com/questions/9323903/most-efficient-elegant-way-to-clip-a-number
                intensity = std::clamp(intensity, 0, 255);
                tempImg.setPixelColor(x, y, qRgb(intensity, intensity, intensity));
            }
        }
        // Change original pixmap in corresponding pixmapSlices with populated pixmap
        slicePixmap = QPixmap::fromImage(tempImg);
    }
    // Display current slice on new regeneration or for first time
    showSlice(colorValSlider->sliderPosition());
}

void ColorHistogram::mouseMoveView(QPoint pos, QColor color) {
    statusBarView->showMessage(QString("Position (%1,%2), Color (R,G,B) = (%3,%4,%5)").arg(pos.x()).arg(pos.y()).arg(color.red()).arg(color.green()).arg(color.blue()));
}

// HMMM... Tried to implement histviewer just like imageviewer but realized that while pos
// info can be extracted from QPixmap, I still need current position of colorValSlider +
// sliceIdx in colorhistogram.cpp to determine which color corresponds to what position and value.
// QUESTION FOR PROFESSOR: is there a more elegant way of doing this so I am not splitting
// source of origin of the statusBar data to display (meaning pos comes from histViewer's signal, and
// color info comes from this class when updating statusBar)? I am also not a fan of the switch I
// use to get the right color. Alternative?
void ColorHistogram::mouseMoveHist(QPoint pos) {
    int sliceIdx = colorSelector->currentIndex();
    int colorVal = colorValSlider->sliderPosition();
    int red; int green; int blue;

    switch(sliceIdx) {
    case 0:
        red = colorVal; // we are iterating over red's colorVal when genr. pixmaps of "red"
        green = pos.y(); // green will be along y axis when displayed histogram set on "red" ComboBox
        blue = pos.x(); // blue will be along x axis when displayed histogram set on "red" ComboBox
        break;

        // ComboBox idx is set to 1 AKA "green"
    case 1:
        red = pos.x();
        green = colorVal;
        blue = pos.y();
        break;

        // ComboBox idx is set to 2 AKA "blue"
    case 2:
        red = pos.x();
        green = pos.y();
        blue = colorVal;
        break;
    }
    statusBarHist->showMessage(QString("Position (%1,%2), Color (R,G,B) = (%3,%4,%5)").arg(pos.x()).arg(pos.y()).arg(red).arg(green).arg(blue));
}
