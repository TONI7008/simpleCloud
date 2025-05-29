#ifndef IMAGEWORKER_H
#define IMAGEWORKER_H

#include <QObject>
#include <QColor>
#include <QImage>
#include <QLinearGradient>
#include <QPainter>
#include <QDir>
#include <QBitmap>
#include <QFileInfo>
#include <QDebug>
#include <QRandomGenerator>
#include <QThread>


inline int clamp(int value, int min, int max) {
    return std::max(min, std::min(value, max));
}

class ImageWorker : public QObject
{
    Q_OBJECT
public:
    explicit ImageWorker(QObject *parent = nullptr);
    ImageWorker(QString input_File, QString outputFolder, QObject *parent=nullptr);
    ImageWorker(QColor, QColor,QColor,QString,QObject *parent=nullptr);
    ImageWorker(QColor,QString,QObject *parent=nullptr);
    enum WorkToDo{
        gaussianblur,
        createS,
        createSL,
        createSG,
        processimage,
        notSet
    };

    void run();
    void setWorkToDO(WorkToDo work);
    void setRadius(short);
    void setColor(QColor);
    void setColor2(QColor);
    void setColor3(QColor);
    void setInputFile(QString);
    void setOutputFolder(QString);

signals:
    void pixmapReady(const QPixmap);
    void imageReady(const QString);
    void imageColorReady(const QString);

private:
    bool stop=false;
    WorkToDo m_work=WorkToDo::notSet;
    QString m_iFile="";
    QString m_oFolder=".";
    QColor m_color=QColor(71, 158, 245);
    QColor m_color2;
    QColor m_color3;
    short radius=0;


    void addNoise(QImage &image, int noiseLevel);
    void createGaussianKernel(int radius, QVector<float> &kernel);
    void gaussianBlur(QImage &image, int radius);
    bool createAndSaveRadialGradientImage(const QColor &color1, const QColor &color2, const QColor &color3, const QString &fileName);
    bool createAndSaveImage(const QColor &color, const QString &fileName="background_color.png");
    bool createAndSaveLinearGradientImage(const QColor &color1, const QColor &color2, const QColor &color3, const QString &fileName);
    QString resizeImage(QString cheminImageOriginale, QString cheminImageReduite);

    QString convertToPng(const QString &filename);
    void processImage(QString imagePath);
};

#endif // IMAGEWORKER_H
