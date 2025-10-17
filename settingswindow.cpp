#include "settingswindow.h"
#include "settingsitem.h"
#include "comboboxfactory.h"
#include "spinboxfactory.h"
#include "checkboxfactory.h"
#include "lineeditfactory.h"
#include "pushbuttonfactory.h"
#include "filebrowsefactory.h"
#include "colordialogfactory.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTreeWidget>
#include <QStackedWidget>
#include <QScrollArea>
#include <QFrame>
#include <QLabel>
#include <QTreeWidgetItem>

SettingsWindow::SettingsWindow(QWidget* parent) : QWidget(parent) {
    setupUI();
    createSettingsTree();
    setupConnections();
}

SettingsWindow::~SettingsWindow() {
    delete rootItem;
}

void SettingsWindow::setupUI() {
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Древовидная панель
    treeWidget = new QTreeWidget();
    treeWidget->setFixedWidth(250);
    treeWidget->setHeaderHidden(true);

    // Область содержимого
    stackedWidget = new QStackedWidget();

    mainLayout->addWidget(treeWidget);
    mainLayout->addWidget(stackedWidget, 1);

    setWindowTitle("Settings");
    setMinimumSize(900, 600);
}

void SettingsWindow::createSettingsTree() {
    // Создаем корневой элемент
    rootItem = new SettingsItem(nullptr, "Settings", "root", "Application Settings", nullptr, false );
    rootItem->setIsGroup(true);

    // Создаем группы
    SettingsItem* mainGroup = new SettingsItem(rootItem, "Main Settings", "main_group", "General application settings", nullptr, false);
    mainGroup->setIsGroup(true);

    SettingsItem* templateGroup = new SettingsItem(rootItem, "Template Settings", "template_group", "File template settings", nullptr, false);
    templateGroup->setIsGroup(true);

    SettingsItem* colorGroup = new SettingsItem(rootItem, "Appearance", "appearance_group", "Visual appearance settings", nullptr, false);
    colorGroup->setIsGroup(true);

    // Добавляем настройки в группу Main
    SettingsItem* languageItem = new SettingsItem(mainGroup, "Language", "1", "Select interface language",
                                                  new ComboBoxFactory("English", QStringList() << "English" << "Russian" << "Spanish"), true);

    SettingsItem* autostartItem = new SettingsItem(mainGroup, "Autostart", "2", "Run application on system startup",
                                                   new CheckBoxFactory(true), true);

    SettingsItem* timeoutItem = new SettingsItem(mainGroup, "Timeout", "3", "Request timeout in milliseconds",
                                                 new SpinBoxFactory(300, 100, 10000), true);

    // Добавляем настройки в группу Template
    SettingsItem* fileTemplateItem = new SettingsItem(templateGroup, "File Template", "4", "Template for file searching",
                                                      new LineEditFactory("*.png"), true);

    SettingsItem* storageItem = new SettingsItem(templateGroup, "Storage Path", "5", "Location where files will be stored",
                                                 new FileBrowseFactory(new LineEditFactory("D:/storage"), new PushButtonFactory("Browse...")), true);

    // Добавляем настройки в группу Appearance
    SettingsItem* themeItem = new SettingsItem(colorGroup, "Theme Color", "6", "Choose application theme color",
                                               new ColorDialogFactory(new LineEditFactory("#0078d4"), new PushButtonFactory("Choose Color")), true);

    SettingsItem* fontSizeItem = new SettingsItem(colorGroup, "Font Size", "7", "Application font size",
                                                  new SpinBoxFactory(12, 8, 24), true);

    // Строим дерево в QTreeWidget
    buildTreeWidget();

    // Создаем страницы для всех элементов (только для групп)
    createPagesForGroups();
}

void SettingsWindow::buildTreeWidget() {
    treeWidget->clear();

    // Рекурсивно добавляем элементы в QTreeWidget
    addItemToTreeWidget(rootItem, nullptr);

    // Разворачиваем корневые группы
    treeWidget->expandAll();
}

void SettingsWindow::addItemToTreeWidget(SettingsItem* settingsItem, QTreeWidgetItem* parentTreeItem) {
    QTreeWidgetItem* treeItem;
    if (parentTreeItem) {
        treeItem = new QTreeWidgetItem(parentTreeItem);
    } else {
        treeItem = new QTreeWidgetItem(treeWidget);
    }

    treeItem->setText(0, settingsItem->name());
    treeItem->setData(0, Qt::UserRole, QVariant::fromValue(settingsItem));

    // Для групп делаем жирный шрифт
    if (settingsItem->isGroup()) {
        QFont font = treeItem->font(0);
        font.setBold(true);
        treeItem->setFont(0, font);
    }

    // Рекурсивно добавляем детей
    for (int i = 0; i < settingsItem->childCount(); ++i) {
        addItemToTreeWidget(settingsItem->child(i), treeItem);
    }
}

void SettingsWindow::createPagesForGroups() {
    // Создаем страницы только для групповых элементов
    QList<SettingsItem*> allItems = rootItem->getAllChildren();
    allItems.prepend(rootItem);

    for (SettingsItem* item : allItems) {
        if (item->isGroup()) {
            createPageForGroup(item);
        }
    }
}

void SettingsWindow::createPageForGroup(SettingsItem* groupItem) {
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget* contentWidget = new QWidget();
    QVBoxLayout* contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setSpacing(15);
    contentLayout->setContentsMargins(25, 25, 25, 25);

    // Заголовок группы
    QLabel* titleLabel = new QLabel(groupItem->name());
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    contentLayout->addWidget(titleLabel);

    // Описание группы
    if (!groupItem->description().isEmpty()) {
        QLabel* descLabel = new QLabel(groupItem->description());
        descLabel->setWordWrap(true);
        contentLayout->addWidget(descLabel);
    }

    // Добавляем настройки этой группы (только непосредственные дети)
    bool hasSettings = false;
    for (int i = 0; i < groupItem->childCount(); ++i) {
        SettingsItem* child = groupItem->child(i);
        if (!child->isGroup()) {
            QHBoxLayout* itemLayout = child->createWidget();
            if (itemLayout) {
                // Добавляем разделитель между настройками
                if (hasSettings) {
                    QFrame* separator = new QFrame();
                    separator->setFrameShape(QFrame::HLine);
                    separator->setFrameShadow(QFrame::Sunken);
                    contentLayout->addWidget(separator);
                }

                contentLayout->addLayout(itemLayout);
                hasSettings = true;
            }
        }
    }

    // Если в группе нет настроек, показываем сообщение
    if (!hasSettings) {
        QLabel* emptyLabel = new QLabel("No settings available in this group");
        emptyLabel->setAlignment(Qt::AlignCenter);
        contentLayout->addWidget(emptyLabel);
    }

    contentLayout->addStretch();
    scrollArea->setWidget(contentWidget);

    // Сохраняем связь между SettingsItem и страницей
    groupPages[groupItem] = scrollArea;
    stackedWidget->addWidget(scrollArea);
}

void SettingsWindow::setupConnections() {
    connect(treeWidget, &QTreeWidget::currentItemChanged,
            this, &SettingsWindow::onTreeItemChanged);
}

void SettingsWindow::onTreeItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous) {
    if (!current) return;

    SettingsItem* settingsItem = current->data(0, Qt::UserRole).value<SettingsItem*>();
    if (settingsItem && settingsItem->isGroup()) {
        if (groupPages.contains(settingsItem)) {
            stackedWidget->setCurrentWidget(groupPages[settingsItem]);
        }
    }
}
