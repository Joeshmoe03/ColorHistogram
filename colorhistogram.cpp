#include "colorhistogram.h"
#include "imageviewer.h"

#include <algorithm>
#include <cmath>
#include <QtWidgets>

ColorHistogram::ColorHistogram(const QImage &_image):image(_image), hist(1 << 24, 0), pixmapSlices(256) {

    // Set up layouts for ColorHistogram widget
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    QVBoxLayout *viewLayout = new QVBoxLayout;
    QVBoxLayout *histLayout = new QVBoxLayout;

    // Add side padding to histLayout and force it to hug center of window
    histLayout->setContentsMargins(20, 0, 20, 0);
    histLayout->setAlignment(Qt::AlignCenter);

    // Add our viewLayout on the left side of the main layout, and histLayout on the right
    mainLayout->addLayout(viewLayout);
    mainLayout->addLayout(histLayout);

    // Add our imageViewer to the left hand side of the screen in our viewLayout
    ImageViewer *imageViewer = new ImageViewer(image);
    imageViewer->setMinimumSize(384, 256);
    connect(imageViewer, &ImageViewer::mouseMoved, this, &ColorHistogram::mouseMoved);
    viewLayout->addWidget(imageViewer);

    // Add our ColorHistogram elements into the right hand side of screen (histLayout)
    histLabel = new QLabel;
    QPixmap histPixmap(256, 256);
    histPixmap.fill(Qt::transparent);
    histLabel->setPixmap(histPixmap);
    histLabel->setFixedSize(256, 256);
    histLayout->addWidget(histLabel);
    CountColors(); // populate our color count vector before generating slices

    // Slider label
    QLabel *valueLabel = new QLabel;
    valueLabel->setText("Color value: ");
    histLayout->addWidget(valueLabel);

    // Our color slider
    colorValSlider = new QSlider;
    colorValSlider->setOrientation(Qt::Horizontal);
    connect(colorValSlider, &QSlider::sliderMoved, this, &ColorHistogram::showSlice);
    histLayout->addWidget(colorValSlider);

    // Dropdown label
    QLabel *colorLabel = new QLabel;
    colorLabel->setText("Color slice: ");
    histLayout->addWidget(colorLabel);

    // Threshold widget for frequency based pixel intensity
    thresholder = new QComboBox;
    QStringList thresholdOptions = {"1", "2", "4", "8", "16", "32", "64", "128", "256", "512", "1024", "2048", "4096", "8192"};
    thresholder->addItems(thresholdOptions);
    connect(thresholder, &QComboBox::currentIndexChanged, this, &ColorHistogram::generateSlices);

    // our dropdown box for slice selection
    colorSelector = new QComboBox;
    QStringList colorOptions = {"Red", "Green", "Blue"};
    colorSelector->addItems(colorOptions);
    generateSlices(); // Generate slices on the first time as we default to first colorOption (red) on startup
    connect(colorSelector, &QComboBox::currentIndexChanged, this, &ColorHistogram::generateSlices); // Slices should oonly be changed afterwards by manual changes to QComboBox
    histLayout->addWidget(colorSelector);

    // Threshold label
    // Thresholding widget for adjusting pixel color intensity at bottom of all widgets
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

    // Color variables
    QRgb pixelColor;
    int red; int green; int blue;
    int colorIdx;

    // Image dimensions
    int width = image.width();
    int height = image.height();

    // iterate thru image and track with histFreq the possible 24-bit RGB colors of the image.
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
        histLabel->setPixmap(pixmapSlices[colorVal]);
    }
}

// Regenerate value slices on given color slice (R or G or B). Slot action is called on ComboBox change or on start.
void ColorHistogram::generateSlices() {
    // get current slice index 0, 1, 2 = R, G, B
    int sliceIdx = colorSelector->currentIndex();
    qDebug() << "Generating slices for color index: " << sliceIdx;

    // Given dimensions of pixmap and other color vars for populating pixmap
    // Indexing of hist is by BBBBBBBB GGGGGGGG RRRRRRRR (bits)
    int width = 256; int height = 256;
    int red; int green; int blue;
    int threshold = pow(2, thresholder->currentIndex());
    int colorIdx;
    int colorFreq;
    QColor color;

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
                if (sliceIdx == 0) {
                    red = colorVal; // we are iterating over red's colorVal when genr. pixmaps of "red"
                    green = y; // green will be along y axis when displayed histogram set on "red" ComboBox
                    blue = x; // blue will be along x axis when displayed histogram set on "red" ComboBox
                }

                // ComboBox idx is set to 1 AKA "green"
                else if (sliceIdx == 1) {
                    red = x;
                    green = colorVal;
                    blue = y;
                }

                // ComboBox idx is set to 2 AKA "blue"
                else if (sliceIdx == 2) {
                    red = x;
                    green = y;
                    blue = colorVal;
                }

                // Indexing of hist (frequencies colors) happens like so: BBBBBBBB GGGGGGGG RRRRRRRR (bits in 24 bit).
                // We want to get freq for given x, y, and colorVal from slider by the index.
                colorIdx = (blue << 16) | (green << 8) | red;
                colorFreq = hist[colorIdx];

                // Set color according to freq as it appears in photo black or white or in between threshold
                if (colorFreq < threshold) {
                    float intensity = 1 - ((threshold - colorFreq) / threshold);

                    // https://stackoverflow.com/questions/9323903/most-efficient-elegant-way-to-clip-a-number
                    intensity = std::clamp(intensity, 0.0f, 1.0f);
                    int pixelIntensity = (int)(intensity * 255);
                    color = QColor(pixelIntensity, pixelIntensity, pixelIntensity);
                } else {
                    color = Qt::white;
                }
                tempImg.setPixelColor(x, y, color);
            }
        }

        // Change original pixmap in corresponding pixmapSlices with populated pixmap
        slicePixmap = QPixmap::fromImage(tempImg);
    }
}
