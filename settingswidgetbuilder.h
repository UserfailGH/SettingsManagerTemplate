#ifndef SETTINGSWIDGETBUILDER_H
#define SETTINGSWIDGETBUILDER_H

#include <QObject>
#include <QHBoxLayout>
#include <QList>
#include <QMap>

class SettingsItem;
class QTreeWidget;
class QTreeWidgetItem;
class QStackedWidget;
class QPushButton;

class SettingsWidgetBuilder : public QObject
{
    Q_OBJECT

public:
    SettingsWidgetBuilder(QList<SettingsItem*> widgetList, QObject* parent = nullptr);
    ~SettingsWidgetBuilder();

    QWidget* getEmbeddedWidget() const;

private:
    void setupTreeUI();
    void buildSettingsTree(QTreeWidget* treeWidget, QStackedWidget* stackedWidget);
    void addTreeItem(QTreeWidget* treeWidget, QStackedWidget* stackedWidget,
                     SettingsItem* settingsItem, QTreeWidgetItem* parentTreeItem);
    void createGroupPage(QStackedWidget* stackedWidget, SettingsItem* groupItem);
    QString buildSettingsPath(SettingsItem* item) const;
    void loadSettings();
    void saveSettings();
    void applyValueToWidget(SettingsItem* item, const QVariant& value);
    void connectSignalsForAutoSave();
    void resetAllSettings();

private slots:
    void onTreeItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);

private:
    QList<SettingsItem*> widgetList_;
    QHBoxLayout* embedLayout_;
    QMap<SettingsItem*, QWidget*> groupPages_;
    QPushButton* resetAllButton_;
};

#endif