#ifndef TDYNAMICFRAME_H
#define TDYNAMICFRAME_H

#include "popoutframe.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QIcon>
#include <QString>
#include <QLabel>

class TStackedWidget;

class HistoryDisplay : public QWidget
{
    Q_OBJECT
public:
    enum View {
        Modern,
        Normal,
        Minimal,
        Breadcrumb
    };

    explicit HistoryDisplay(QWidget* parent = nullptr);
    ~HistoryDisplay();

    void setPath(const QString& newPath);
    QString path() const;

    View view() const;
    void setView(View newView);

    void setHomeIcon(const QIcon& icon);
    void setSeparatorIcon(const QIcon& icon);
    void setButtonStyle(const QString& style);

signals:
    void pathClicked(const QString& path);
    void homeClicked();

private:
    void updateView();
    void createModernView();
    void createNormalView();
    void createMinimalView();
    void createBreadcrumbView();
    void clearLayout();

    QString buildPartialPath(int index) const;

    QString m_path;
    View m_view = Modern;
    QPushButton* m_homeButton = nullptr;
    QHBoxLayout* m_layout = nullptr;
    QIcon m_homeIcon;
    QIcon m_separatorIcon;
    QString m_buttonStyle;

    static const QString DEFAULT_STYLE;
};

class MessageWidget : public QWidget
{
    Q_OBJECT
public:
    MessageWidget(QWidget* parent=nullptr);
    ~MessageWidget();

    void setMessage(const QString&);
private:
    QLabel* m_label=nullptr;
    QHBoxLayout* m_layout = nullptr;
};





class TDynamicFrame : public PopoutFrame
{
    Q_OBJECT
public:
    explicit TDynamicFrame(QWidget* parent = nullptr);
    ~TDynamicFrame();

    TStackedWidget* stackedWidget() const;
    void setAnimationDuration(int duration);
    void setPath(const QString&);
    void setMaxwidth(int width);
    void setMessage(const QString&);

signals:
    void pathClicked(const QString& path);

private:
    TStackedWidget* m_stack = nullptr;
    QHBoxLayout* m_layout = nullptr;
    HistoryDisplay* m_hDisplay=nullptr;
    MessageWidget* m_mWidget=nullptr;

};

#endif // TDYNAMICFRAME_H
