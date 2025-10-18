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
#include <QCloseEvent>

SettingsWindow::SettingsWindow(QWidget* parent) : QWidget(parent) {
    setupUI();
    createSettingsTree();
    setupConnections();

    loadSettings();
    connectSignalsForAutoSave();
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
    // Создаем корневой элемент (группа)
    rootItem = new SettingsItem(nullptr, "Settings", "root", "Application Settings");

    // Создаем группы (используем конструктор для групп)
    SettingsItem* mainGroup = new SettingsItem(rootItem, "Main Settings", "main_group", "General application settings");
    SettingsItem* templateGroup = new SettingsItem(rootItem, "Template Settings", "template_group", "File template settings");
    SettingsItem* colorGroup = new SettingsItem(rootItem, "Appearance", "appearance_group", "Visual appearance settings");

    // Добавляем настройки в группу Main (используем конструктор для настроек)
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

#include <QSettings>
#include <QDebug>

void SettingsWindow::loadSettings() {
    qDebug() << "=== LOADING SETTINGS ===";
    QSettings settings("TestLabs", "TestSettings");

    qDebug() << "Settings file:" << settings.fileName();

    // Собираем все элементы которые можно сохранять
    QList<SettingsItem*> allItems = rootItem->getAllChildren();
    allItems.prepend(rootItem);

    int loadedCount = 0;
    for (SettingsItem* item : allItems) {
        if (!item->isSavingEnabled() || item->isGroup()) continue;

        QString settingsPath = buildSettingsPath(item);
        QVariant defaultValue = item->getValue();
        QVariant savedValue = settings.value(settingsPath, defaultValue);

        qDebug() << "Loading:" << item->name() << "Path:" << settingsPath
                 << "Default:" << defaultValue << "Saved:" << savedValue;

        if (savedValue != defaultValue) {
            applyValueToWidget(item, savedValue);
            loadedCount++;
        }
    }

    qDebug() << "Loaded" << loadedCount << "settings";
}

void SettingsWindow::saveSettings() {
    qDebug() << "=== SAVING SETTINGS ===";
    QSettings settings("TestLabs", "TestSettings");

    // Собираем все элементы которые можно сохранять
    QList<SettingsItem*> allItems = rootItem->getAllChildren();
    allItems.prepend(rootItem);

    int savedCount = 0;
    for (SettingsItem* item : allItems) {
        if (!item->isSavingEnabled() || item->isGroup()) continue;

        QString settingsPath = buildSettingsPath(item);
        QVariant currentValue = item->getValue();

        qDebug() << "Saving:" << item->name() << "Path:" << settingsPath << "Value:" << currentValue;

        settings.setValue(settingsPath, currentValue);
        savedCount++;
    }

    settings.sync();
    qDebug() << "Saved" << savedCount << "settings";
}

QString SettingsWindow::buildSettingsPath(SettingsItem* item) const {
    QStringList pathParts;
    SettingsItem* current = item;

    // Собираем путь от корня до элемента
    while (current) {
        if (!current->id().isEmpty()) {
            pathParts.prepend(current->id());
        }
        current = current->parent();
    }

    return pathParts.join("/");
}

void SettingsWindow::applyValueToWidget(SettingsItem* item, const QVariant& value) {
    if (QComboBox* comboBox = item->comboBox()) {
        int index = comboBox->findText(value.toString());
        if (index >= 0) comboBox->setCurrentIndex(index);
    } else if (QCheckBox* checkBox = item->checkBox()) {
        checkBox->setChecked(value.toBool());
    } else if (QSpinBox* spinBox = item->spinBox()) {
        spinBox->setValue(value.toInt());
    } else if (QLineEdit* lineEdit = qobject_cast<QLineEdit*>(item->controlWidget())) {
        lineEdit->setText(value.toString());
    } else if (QWidget* compositeWidget = item->controlWidget()) {
        QLineEdit* lineEdit = compositeWidget->findChild<QLineEdit*>();
        if (lineEdit) lineEdit->setText(value.toString());
    }
}

void SettingsWindow::connectSignalsForAutoSave() {
    qDebug() << "=== CONNECTING AUTO-SAVE SIGNALS ===";

    // Собираем все элементы которые можно сохранять
    QList<SettingsItem*> allItems = rootItem->getAllChildren();
    allItems.prepend(rootItem);

    int connectedCount = 0;
    for (SettingsItem* item : allItems) {
        if (!item->isSavingEnabled() || item->isGroup()) continue;

        if (QComboBox* comboBox = item->comboBox()) {
            connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
                    this, &SettingsWindow::saveSettings);
            connectedCount++;
        } else if (QCheckBox* checkBox = item->checkBox()) {
            connect(checkBox, &QCheckBox::toggled, this, &SettingsWindow::saveSettings);
            connectedCount++;
        } else if (QSpinBox* spinBox = item->spinBox()) {
            connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsWindow::saveSettings);
            connectedCount++;
        } else if (QLineEdit* lineEdit = qobject_cast<QLineEdit*>(item->controlWidget())) {
            connect(lineEdit, &QLineEdit::textChanged, this, &SettingsWindow::saveSettings);
            connectedCount++;
        } else if (QWidget* compositeWidget = item->controlWidget()) {
            QLineEdit* lineEdit = compositeWidget->findChild<QLineEdit*>();
            if (lineEdit) {
                connect(lineEdit, &QLineEdit::textChanged, this, &SettingsWindow::saveSettings);
                connectedCount++;
            }
        }
    }

    qDebug() << "Connected" << connectedCount << "signals";
}

void SettingsWindow::closeEvent(QCloseEvent* event) {
    qDebug() << "=== WINDOW CLOSING - SAVING SETTINGS ===";
    saveSettings();
    event->accept();
}