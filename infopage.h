#ifndef INFOPAGE_H
#define INFOPAGE_H

#include "tfileinfo.h"
#include "tframe.h"
#include <QObject>
#include <QGridLayout>
#include <QMouseEvent>
#include <QFormLayout>
#include <QToolButton>
#include <QLabel>
#include <QPushButton>

class HoverButton;
class TLabel;

class InfoPage : public TFrame {
    Q_OBJECT
public:
    void changeInfo(TFileInfo info);
    static InfoPage* m_infoPage;
    static InfoPage* instance();
    static void Init(QWidget* parent = nullptr);
    static void cleanUp();

    void showForFile(const TFileInfo& info);
    void showForFolder(const TFileInfo &info);
private:
    explicit InfoPage(QWidget* parent = nullptr); // Make constructor private
    ~InfoPage();

    void initUI();
    TFileInfo info;
    QPixmap backgroundImage = QPixmap(":/pictures/image2.png");
    TLabel *filePath;
    QLabel *fileSize;
    QLabel *date;
    QLabel *uploadDate;
    HoverButton *closeButton;
    QPushButton* iconButton;
    bool m_folder;
    QString buttonStyle = R"(
QToolButton{
border-radius:10px;
padding-left:10px;
padding-right:10px;
color: rgb(255, 255, 255);
background:transparent;
}
/*QToolButton:hover{
background: qLineargradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgba(13, 112, 210, 0.5), stop: 1.0 rgba(0, 79, 204, 0.5));
}*/)";

    bool dragging = false;
    QPoint dragPosition;
    void updateInfo();
    QIcon getIcon();
    QString name();
};


#endif // INFOPAGE_H
