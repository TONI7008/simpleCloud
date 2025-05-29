#include "imageworker.h"
#include "tcloud.h"


ImageWorker::ImageWorker(QObject *parent)
    : QObject{parent}, stop(false), m_work(WorkToDo::notSet)
{

}

ImageWorker::ImageWorker(QString input_File, QString outputFolder, QObject *parent)
    : QObject{parent}, stop(false), m_work(WorkToDo::notSet), m_iFile(input_File), m_oFolder(outputFolder)
{

}

ImageWorker::ImageWorker(QColor color1, QColor color2, QColor color3,QString outputFolder, QObject *parent)
    : QObject{parent}, stop(false), m_work(WorkToDo::notSet), m_color(color1), m_color2(color2), m_color3(color3)
{
    m_oFolder=outputFolder;
}

ImageWorker::ImageWorker(QColor color,QString outputFolder,QObject *parent)
    : QObject{parent}, stop(false), m_work(WorkToDo::notSet), m_color(color)
{
    m_oFolder=outputFolder;
}

bool ImageWorker::createAndSaveImage(const QColor& color, const QString& fileName) {
    QImage image(1980, 1200, QImage::Format_ARGB32);
    image.fill(color);

    QString fullPath = m_oFolder + QDir::separator() + fileName;
    if(image.save(fullPath)){
        emit imageColorReady(fullPath);
    }
    return false;
}

bool ImageWorker::createAndSaveRadialGradientImage(const QColor& color1, const QColor& color2, const QColor& color3, const QString& fileName) {
    QImage image(1980, 1200, QImage::Format_ARGB32);
    QPainter painter(&image);

    QRadialGradient gradient(QPointF(990, 600), 600);
    gradient.setColorAt(0.0, color1);
    gradient.setColorAt(0.5, color2);
    gradient.setColorAt(1.0, color3);

    painter.fillRect(image.rect(), gradient);

    QString fullPath = m_oFolder + QDir::separator() + fileName;

    return image.save(fullPath);
}

bool ImageWorker::createAndSaveLinearGradientImage(const QColor& color1, const QColor& color2, const QColor& color3, const QString& fileName) {
    QImage image(1980, 1200, QImage::Format_ARGB32);
    QPainter painter(&image);

    QLinearGradient gradient(0, 0, 1980, 1200);
    gradient.setColorAt(0.0, color1);
    gradient.setColorAt(0.5, color2);
    gradient.setColorAt(1.0, color3);

    painter.fillRect(image.rect(), gradient);

    QString fullPath = m_oFolder + QDir::separator() + fileName;

    return image.save(fullPath);
}

QString ImageWorker::resizeImage(QString cheminImageOriginale, QString cheminImageReduite) {
    QImage image(cheminImageOriginale);
    if (image.isNull()) {
        qWarning() << "Failed to load image from:" << cheminImageOriginale;
        return QString();
    }

    image = image.convertToFormat(QImage::Format_ARGB32);

    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            if (image.pixelColor(x, y) == Qt::color0) {
                image.setPixelColor(x, y, QColor(0, 0, 0, 0));
            }
        }
    }

    int side = qMin(image.width(), image.height());
    int centerX = (image.width() - side) / 2;
    int centerY = (image.height() - side) / 2;

    QImage imageReduite(side, side, QImage::Format_ARGB32);
    QPainter painter(&imageReduite);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.drawImage(QRect(0, 0, side, side), image, QRect(centerX, centerY, side, side));

    if (!imageReduite.save(cheminImageReduite)) {
        qWarning() << "Failed to save reduced image to:" << cheminImageReduite;
        return QString();
    }

    return cheminImageReduite;
}

void ImageWorker::processImage(QString imagePath) {
    QFileInfo info(imagePath);
    QString path=imagePath;
    if(info.suffix()!="png"){
        path=convertToPng(imagePath);
    }
    QPixmap pixmap(path);
    if (pixmap.isNull()) {
        qWarning() << "Failed to load pixmap from:" << path;
        return;
    }

    QBitmap mask(pixmap.size());
    mask.fill(Qt::color0);
    QPainter painter(&mask);
    painter.setBrush(Qt::color1);

    int side = qMin(pixmap.width(), pixmap.height());
    int centerX = (pixmap.width() - side) / 2;
    int centerY = (pixmap.height() - side) / 2;
    painter.drawEllipse(centerX, centerY, side, side);

    pixmap.setMask(mask);

    QString savePath = m_oFolder + QDir::separator() + "background_picture." + QFileInfo(path).suffix();
    if (!pixmap.save(savePath)) {
        qWarning() << "Failed to save processed pixmap to:" << savePath;
        return;
    }

    QPixmap pmap = QPixmap(resizeImage(savePath, savePath));
    if (pmap.isNull()) {
        qWarning() << "Failed to resize pixmap.";
        return;
    }

    emit pixmapReady(pmap);
}

QString ImageWorker::convertToPng(const QString &filename) {
    QImage image(filename);
    if (image.isNull()) {
        qWarning() << "Failed to load image from file:" << filename;
        return QString();
    }

    QFileInfo fileInfo(filename);
    QString newFilename = fileInfo.path() + "/" + fileInfo.baseName() + ".png";

    if (!image.save(newFilename, "PNG")) {
        qWarning() << "Failed to save image to file:" << newFilename;
        return QString();
    }
    return newFilename;
}

void ImageWorker::setWorkToDO(WorkToDo work) {
    m_work = work;
}

void ImageWorker::setRadius(short r)
{
    radius=r;
}

void ImageWorker::setColor(QColor color)
{
    m_color=color;
}

void ImageWorker::setColor2(QColor color)
{
    m_color2=color;
}

void ImageWorker::setColor3(QColor color)
{
    m_color3=color;
}

void ImageWorker::setInputFile(QString in)
{
    m_iFile=in;
}

void ImageWorker::setOutputFolder(QString out)
{
    m_oFolder=out;
}


void ImageWorker::addNoise(QImage &image, int noiseLevel) {
    for (int y = 0; y < image.height(); y++) {
        for (int x = 0; x < image.width(); x++) {
            QRgb pixel = image.pixel(x, y);
            int r = qRed(pixel) + QRandomGenerator::global()->bounded(-noiseLevel, noiseLevel);
            int g = qGreen(pixel) + QRandomGenerator::global()->bounded(-noiseLevel, noiseLevel);
            int b = qBlue(pixel) + QRandomGenerator::global()->bounded(-noiseLevel, noiseLevel);
            image.setPixel(x, y, qRgb(clamp(r, 0, 255), clamp(g, 0, 255), clamp(b, 0, 255)));
        }
    }
}

// Helper function to create a Gaussian kernel
void ImageWorker::createGaussianKernel(int radius, QVector<float>& kernel) {
    int size = radius * 2 + 1;
    kernel.resize(size);
    float sigma = radius / 2.0f;
    float sum = 0.0f;

    for (int i = 0; i < size; i++) {
        int x = i - radius;
        kernel[i] = std::exp(-0.5f * x * x / (sigma * sigma));
        sum += kernel[i];
    }

    // Normalize the kernel
    for (int i = 0; i < size; i++) {
        kernel[i] /= sum;
    }
}

// Function to apply Gaussian blur to an image
void ImageWorker::gaussianBlur(QImage &image, int radius) {
    if (radius < 1) return;

    QVector<float> kernel;
    createGaussianKernel(radius, kernel);

    QImage temp = image;

    int w = image.width();
    int h = image.height();

    // Horizontal blur
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float rSum = 0, gSum = 0, bSum = 0;
            for (int k = -radius; k <= radius; k++) {
                int idx = clamp(x + k, 0, w - 1);
                QRgb pixel = temp.pixel(idx, y);
                float weight = kernel[k + radius];
                rSum += qRed(pixel) * weight;
                gSum += qGreen(pixel) * weight;
                bSum += qBlue(pixel) * weight;
            }
            image.setPixel(x, y, qRgb(clamp(static_cast<int>(rSum), 0, 255),
                                      clamp(static_cast<int>(gSum), 0, 255),
                                      clamp(static_cast<int>(bSum), 0, 255)));
        }
    }

    // Vertical blur
    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            float rSum = 0, gSum = 0, bSum = 0;
            for (int k = -radius; k <= radius; k++) {
                int idx = clamp(y + k, 0, h - 1);
                QRgb pixel = image.pixel(x, idx);
                float weight = kernel[k + radius];
                rSum += qRed(pixel) * weight;
                gSum += qGreen(pixel) * weight;
                bSum += qBlue(pixel) * weight;
            }
            temp.setPixel(x, y, qRgb(clamp(static_cast<int>(rSum), 0, 255),
                                     clamp(static_cast<int>(gSum), 0, 255),
                                     clamp(static_cast<int>(bSum), 0, 255)));
        }
    }

    image = temp;
}

void ImageWorker::run() {
    while (!stop) {
        // Handle each work task using bitwise checks
        if (m_work == WorkToDo::gaussianblur) {
            QImage image(m_iFile);
            if(radius!=0){
                gaussianBlur(image, radius); // Example radius
            }

            QString baseName = "background_image";
            QString extension = ".png";
            QString filePath = m_oFolder + QDir::separator() + baseName + extension;
            int counter = 1;

            while (QFile::exists(filePath)) {
                filePath = m_oFolder + QDir::separator() + baseName + "_" + QString::number(counter) + extension;
                counter++;
            }
            // Save the image with the new or original filename
            image.save(filePath);
            emit imageReady(filePath);
            // Stop after processing the tasks
            stop = true;
        }


        if (m_work==WorkToDo::createS) {
            createAndSaveImage(m_color, "background_color.png");
            stop = true;
        }

        if (m_work==WorkToDo::createSL) {
            createAndSaveLinearGradientImage(m_color, m_color2, m_color3, "background_color.png");
            stop = true;
        }

        if (m_work==WorkToDo::createSG) {
            createAndSaveRadialGradientImage(m_color, m_color2, m_color3, "background_color.png");
            stop = true;
        }

        if (m_work==WorkToDo::processimage) {
            processImage(m_iFile);
            stop = true;
        }

        TCLOUD::Wait(150);  // Small sleep to prevent high CPU usage in the loop
    }
    thread()->quit();
}

