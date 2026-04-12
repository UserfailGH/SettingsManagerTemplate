#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>
#include <QTreeWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QMap>

class SettingsItem;

class SettingsWindow : public QWidget {
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget* parent = nullptr);
    ~SettingsWindow();

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onTreeItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
    void onResetAllClicked();
    void onResetGroupClicked();

private:
    void setupUI();
    void createSettingsTree();
    void buildTreeWidget();
    void addItemToTreeWidget(SettingsItem* item, QTreeWidgetItem* parent);
    void createPagesForGroups();
    void createPageForGroup(SettingsItem* group);
    void setupConnections();
    void loadSettings();
    void saveSettings();
    void applyValueToWidget(SettingsItem* item, const QVariant& value);
    void connectSignalsForAutoSave();

    QTreeWidget* treeWidget = nullptr;
    QStackedWidget* stackedWidget = nullptr;
    QPushButton* resetAllButton = nullptr;
    QPushButton* resetGroupButton = nullptr;
    QMap<SettingsItem*, QWidget*> groupPages;

    SettingsItem* rootItem = nullptr;
};

#endif // SETTINGSWINDOW_H