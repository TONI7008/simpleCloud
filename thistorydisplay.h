#ifndef TTHistoryDisplay_H
#define TTHistoryDisplay_H

#include <QWidget>
#include<QIcon>
#include <QPushButton>
#include <QHBoxLayout>

class THistoryDisplay : public QWidget
{
    Q_OBJECT
public:
    enum View {
        Modern,
        Normal,
        Minimal,
        Breadcrumb
    };

    explicit THistoryDisplay(QWidget* parent = nullptr);
    ~THistoryDisplay();

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
    void middleClicked(const QString& path);  // New signal for middle path

private:
    void updateView();
    void createModernView();
    void createNormalView();
    void createMinimalView();
    void createBreadcrumbView();
    void clearLayout();
    bool needsEllipsis(const QStringList& parts);  // New helper function

    QString buildPartialPath(int index) const;

    QString m_path;
    View m_view = Modern;
    QPushButton* m_homeButton = nullptr;
    QHBoxLayout* m_layout = nullptr;
    QIcon m_homeIcon;
    QIcon m_separatorIcon;
    QString m_buttonStyle;

    static const QString DEFAULT_STYLE;
    QPushButton *createSeparator();
    void addPathButton(const QString &text, const QString &fullPath);
};

#endif // TTHistoryDisplay_H
