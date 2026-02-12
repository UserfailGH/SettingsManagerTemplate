#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QtWidgets/QWidget>
#include <QMap>

class SettingsItem;
class QTreeWidget;
class QTreeWidgetItem;
class QStackedWidget;
class QScrollArea;
class QPushButton;

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
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onTreeItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
    void onResetAllClicked();
    void onResetGroupClicked();

private:
    QTreeWidget* treeWidget;
    QStackedWidget* stackedWidget;
    SettingsItem* rootItem;
    QMap<SettingsItem*, QScrollArea*> groupPages;
    QPushButton* resetAllButton;
    QPushButton* resetGroupButton;

    void loadSettings();
    void saveSettings();
    void connectSignalsForAutoSave();
    QString buildSettingsPath(SettingsItem* item) const;
    void applyValueToWidget(SettingsItem* item, const QVariant& value);
    void resetItemToDefault(SettingsItem* item);
};

#endif // SETTINGSWINDOW_H