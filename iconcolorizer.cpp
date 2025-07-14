#include "iconcolorizer.h"
#include <algorithm>
#include "QDebug"

QColor IconColorizer::_dominantColor=QColor(71,158,245);

void IconColorizer::setDominantColor(QColor color)
{
    _dominantColor=color;
}

QColor IconColorizer::dominantColor()
{
    return _dominantColor;
}

QPixmap IconColorizer::recolor(const QPixmap &src, QColor color) {
    QImage image = src.toImage().convertToFormat(QImage::Format_ARGB32);
    applyColor(image, color);
    return QPixmap::fromImage(image);
}

QPixmap IconColorizer::recolor(const QString &filePath, QColor color) {
    QPixmap pixmap(filePath);
    return recolor(pixmap, color);
}

QPixmap IconColorizer::recolor(const QString &filePath, const QLinearGradient &gradient)
{
    QPixmap pixmap(filePath);
    if(pixmap.isNull()) return QPixmap();

    return recolor(pixmap,gradient);
}

QPixmap IconColorizer::recolor(const QPixmap &src, const QLinearGradient &gradient) {
    QImage image = src.toImage().convertToFormat(QImage::Format_ARGB32);
    QImage gradientImage(image.size(), QImage::Format_ARGB32);

    QPainter gp(&gradientImage);
    gp.fillRect(image.rect(), gradient);
    gp.end();

    for (int y = 0; y < image.height(); ++y) {
        QRgb *imgLine = reinterpret_cast<QRgb *>(image.scanLine(y));
        QRgb *gradLine = reinterpret_cast<QRgb *>(gradientImage.scanLine(y));
        for (int x = 0; x < image.width(); ++x) {
            int alpha = qAlpha(imgLine[x]);
            if (alpha > 0) {
                imgLine[x] = qRgba(qRed(gradLine[x]), qGreen(gradLine[x]), qBlue(gradLine[x]), alpha);
            }
        }
    }
    return QPixmap::fromImage(image);
}

QIcon IconColorizer::recolor(const QIcon &src, QColor color) {
    QIcon result;
    //qDebug() <<"size="<< src.availableSizes();
    for (const QSize &size : src.availableSizes()) {
        QPixmap pixmap = src.pixmap(size);
        result.addPixmap(recolor(pixmap, color));
    }
    return result;
}

QIcon IconColorizer::recolor(const QIcon &src, const QLinearGradient &gradient) {
    QIcon result;
    for (const QSize &size : src.availableSizes()) {
        QPixmap pixmap = src.pixmap(size);
        result.addPixmap(recolor(pixmap, gradient));
    }
    return result;
}

bool IconColorizer::recolorAndSave(const QPixmap &src, QColor color, const QString &savePath) {
    QPixmap result = recolor(src, color);
    return result.save(savePath);
}

bool IconColorizer::recolorAndSave(const QString &filePath, const QColor &color, const QString &savePath) {
    QPixmap result = recolor(filePath, color);
    return result.save(savePath);
}

bool IconColorizer::recolorAndSave(const QPixmap &src, const QLinearGradient &gradient, const QString &savePath) {
    QPixmap result = recolor(src, gradient);
    return result.save(savePath);
}

bool IconColorizer::recolorAndSave(const QIcon &src, QColor color, const QString &savePath) {
    QPixmap pixmap = src.pixmap(256); // Use largest available size or default to 256
    return recolorAndSave(pixmap, color, savePath);
}

bool IconColorizer::recolorAndSave(const QIcon &src, const QLinearGradient &gradient, const QString &savePath) {
    QPixmap pixmap = src.pixmap(256); // Use largest available size or default to 256
    return recolorAndSave(pixmap, gradient, savePath);
}


void IconColorizer::applyGrayscale(QImage &image) {
    for (int y = 0; y < image.height(); ++y) {
        QRgb *line = reinterpret_cast<QRgb *>(image.scanLine(y));
        for (int x = 0; x < image.width(); ++x) {
            int gray = qGray(line[x]);
            int alpha = qAlpha(line[x]);
            line[x] = qRgba(gray, gray, gray, alpha);
        }
    }
}

void IconColorizer::applySepia(QImage &image) {
    for (int y = 0; y < image.height(); ++y) {
        QRgb *line = reinterpret_cast<QRgb *>(image.scanLine(y));
        for (int x = 0; x < image.width(); ++x) {
            int r = qRed(line[x]);
            int g = qGreen(line[x]);
            int b = qBlue(line[x]);
            int alpha = qAlpha(line[x]);

            int tr = std::min(255, int(0.393 * r + 0.769 * g + 0.189 * b));
            int tg = std::min(255, int(0.349 * r + 0.686 * g + 0.168 * b));
            int tb = std::min(255, int(0.272 * r + 0.534 * g + 0.131 * b));

            line[x] = qRgba(tr, tg, tb, alpha);
        }
    }
}

void IconColorizer::applyColor(QImage &image, QColor color) {
    for (int y = 0; y < image.height(); ++y) {
        QRgb *line = reinterpret_cast<QRgb *>(image.scanLine(y));
        for (int x = 0; x < image.width(); ++x) {
            int alpha = qAlpha(line[x]);
            if (alpha > 0) {
                line[x] = qRgba(color.red(), color.green(), color.blue(), alpha);
            }
        }
    }
}

QIcon IconColorizer::colorize(const QIcon &icon, const QColor &color, const QSize &size) {
    if (icon.isNull()) {
        return QIcon();
    }

    QPixmap originalPixmap = icon.pixmap(size);
    if (originalPixmap.isNull()) {
        return QIcon();
    }

    QImage img = originalPixmap.toImage().convertToFormat(QImage::Format_ARGB32);
    for (int y = 0; y < img.height(); ++y) {
        QRgb *line = reinterpret_cast<QRgb *>(img.scanLine(y));
        for (int x = 0; x < img.width(); ++x) {
            QColor pixelColor = QColor::fromRgba(line[x]);
            int alpha = pixelColor.alpha();
            line[x] = qRgba(color.red(), color.green(), color.blue(), alpha);
        }
    }

    QPixmap colorizedPixmap = QPixmap::fromImage(img);
    return QIcon(colorizedPixmap);
}


QIcon IconColorizer::colorize(const QIcon &icon, const QLinearGradient &gradient, const QSize &size) {
    if (icon.isNull()) {
        return QIcon();
    }

    QPixmap originalPixmap = icon.pixmap(size);
    if (originalPixmap.isNull()) {
        return QIcon();
    }

    QPixmap colorizedPixmap = recolor(originalPixmap,gradient);
    return QIcon(colorizedPixmap);
}
