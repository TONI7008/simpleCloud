#ifndef ICONCOLORIZER_H
#define ICONCOLORIZER_H

#include <QIcon>
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QLinearGradient>
#include <QString>
#include <QColor>

class IconColorizer {
public:
    // Recolor QPixmap variants
    static QColor _dominantColor;
    static void setDominantColor(QColor color);
    static QColor dominantColor();

    static QPixmap recolor(const QPixmap &src, QColor color=_dominantColor);
    static QPixmap recolor(const QPixmap &src, const QLinearGradient &gradient);

    // Recolor QIcon variants
    static QIcon recolor(const QIcon &src, QColor color=_dominantColor);
    static QIcon recolor(const QIcon &src, const QLinearGradient &gradient);

    // File-based operations
    static QPixmap recolor(const QString &filePath, QColor color=_dominantColor);
    static QPixmap recolor(const QString &filePath, const QLinearGradient &gradient);

    // Save operations
    static bool recolorAndSave(const QPixmap &src, QColor color, const QString &savePath);
    static bool recolorAndSave(const QPixmap &src, const QLinearGradient &gradient, const QString &savePath);
    static bool recolorAndSave(const QIcon &src, QColor color, const QString &savePath);
    static bool recolorAndSave(const QIcon &src, const QLinearGradient &gradient, const QString &savePath);
    static bool recolorAndSave(const QString &src, const QColor &color, const QString &savePath);

    // Image effects
    static void applyGrayscale(QImage &image);
    static void applySepia(QImage &image);
    static void applyColor(QImage &image, QColor color=_dominantColor);
    //static void applyGradient(QImage &image, const QLinearGradient &gradient);

    static QIcon colorize(const QIcon &icon, const QColor &color=_dominantColor, const QSize &size=QSize(64,64));
    static QIcon colorize(const QIcon &icon, const QLinearGradient &gradient, const QSize &size=QSize(64,64));
};

#endif // ICONCOLORIZER_H
