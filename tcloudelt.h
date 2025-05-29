#ifndef TCLOUDELT_H
#define TCLOUDELT_H

#include <QGridLayout>
#include <QIcon>

#include "tframe.h"
#include "tfileinfo.h"

class TPushButton;
class TMenu;
class TLabel;

class eltCore {
public:
    // Constructors
    eltCore() = default;  // Default constructor
    eltCore(const QString& _name, const QString& _path, const TFileInfo& _info)
        : name(_name), path(_path), info(_info) {}

    // Rule of Five (for complete value semantics)
    eltCore(const eltCore&) = default;
    eltCore(eltCore&&) = default;
    eltCore& operator=(const eltCore&) = default;
    eltCore& operator=(eltCore&&) = default;
    ~eltCore() = default;

    // Member variables
    QString name;
    QString path;
    TFileInfo info;

    // Optional: Add equality comparison
    bool operator==(const eltCore& other) const {
        return name == other.name &&
               path == other.path &&
               info.filepath == other.info.filepath;  // Add more fields if needed
    }
};

class TCloudElt : public TFrame
{
    Q_OBJECT
public:
    explicit TCloudElt(QWidget *parent = nullptr);
    enum Type{
        File,
        Folder,
        Shorcut,
        None
    };
    enum View{
        Linear,
        Grid
    };

    QString name() const;
    qint64 size() const;
    TFileInfo info() const;

    Type type() const;
    void setSelected(bool);
    bool isSelected() const;
    void setView(View);

    virtual QWidget* originParent();

    TMenu *fMenu() const;
    void setFMenu(TMenu *newFMenu);

    bool multiSelection() const;
    void setMultiSelection(bool newMultiSelection);

    virtual eltCore core();
    virtual void copy();
    virtual void cut();


signals:
    void selectedChanged(bool selected);
    void viewChanged(const View&);

protected:
    QString m_name;
    QIcon m_icon;
    QSize iconSize;
    qint64 m_size;
    bool m_selected;
    View m_view;
    bool m_multiSelection;
    Type m_type;

    TFileInfo m_info;
    TLabel* m_labelName=nullptr;
    TPushButton* m_iconButton=nullptr;
    QGridLayout* m_displayLayout=nullptr;
    TMenu* m_fMenu=nullptr;
    TMenu* m_deletedMenu=nullptr;


};

#endif // TCLOUDELT_H
