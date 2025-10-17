#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>
#include <QMap>

class SettingsItem;
class QTreeWidget;
class QTreeWidgetItem;
class QStackedWidget;
class QScrollArea;

class SettingsWindow : public QWidget
{
    Q_OBJECT

public:
    SettingsWindow(QWidget* parent = nullptr);
    ~SettingsWindow();

private:
    void setupUI();
    void createSettingsTree();
    void buildTreeWidget();
    void addItemToTreeWidget(SettingsItem* settingsItem, QTreeWidgetItem* parentTreeItem);
    void createPagesForGroups();
    void createPageForGroup(SettingsItem* groupItem);
    void setupConnections();

private slots:
    void onTreeItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);

private:
    QTreeWidget* treeWidget;
    QStackedWidget* stackedWidget;
    SettingsItem* rootItem;
    QMap<SettingsItem*, QScrollArea*> groupPages;
};

#endif // SETTINGSWINDOW_H
