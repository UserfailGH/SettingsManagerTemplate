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
#include <QPushButton>
#include <QSettings>
#include <QMessageBox>
#include <QDebug>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QColorDialog>
#include <QFileDialog>

#include <QStringList>
#include <QVariant>

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
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    resetAllButton = new QPushButton("Reset All to Default");
    resetGroupButton = new QPushButton("Reset Current Group");
    buttonLayout->addWidget(resetAllButton);
    buttonLayout->addWidget(resetGroupButton);
    buttonLayout->addStretch();

    QHBoxLayout* contentLayout = new QHBoxLayout();
    treeWidget = new QTreeWidget();
    treeWidget->setFixedWidth(250);
    treeWidget->setHeaderHidden(true);

    stackedWidget = new QStackedWidget();

    contentLayout->addWidget(treeWidget);
    contentLayout->addWidget(stackedWidget, 1);

    mainLayout->addLayout(buttonLayout);
    mainLayout->addLayout(contentLayout);

    setWindowTitle("Settings");
    setMinimumSize(900, 600);
}

void SettingsWindow::createSettingsTree() {
    rootItem = new SettingsItem(nullptr, "Settings", "root", "Application Settings");

    SettingsItem* mainGroup = new SettingsItem(rootItem, "Main Settings", "main_group", "General application settings");
    SettingsItem* templateGroup = new SettingsItem(rootItem, "Template Settings", "template_group", "File template settings");
    SettingsItem* colorGroup = new SettingsItem(rootItem, "Appearance", "appearance_group", "Visual appearance settings");

    auto* languageItem = new SettingsItem(mainGroup, "Language", "1", "Select interface language");
    languageItem->setFactory(new ComboBoxFactory("English", {"English", "Russian", "Spanish"}), true);

    auto* autostartItem = new SettingsItem(mainGroup, "Autostart", "2", "Run application on system startup");
    autostartItem->setFactory(new CheckBoxFactory(true), true);

    auto* timeoutItem = new SettingsItem(mainGroup, "Timeout", "3", "Request timeout in milliseconds");
    timeoutItem->setFactory(new SpinBoxFactory(300, 100, 10000), true);

    auto* fileTemplateItem = new SettingsItem(templateGroup, "File Template", "4", "Template for file searching");
    fileTemplateItem->setFactory(new LineEditFactory("*.png"), true);

    auto* storageItem = new SettingsItem(templateGroup, "Storage Path", "5", "Location where files will be stored");
    storageItem->setFactory(new FileBrowseFactory(new LineEditFactory("D:/storage"), new PushButtonFactory("Browse...")), true);

    auto* themeItem = new SettingsItem(colorGroup, "Theme Color", "6", "Choose application theme color");
    themeItem->setFactory(new ColorDialogFactory(new LineEditFactory("#0078d4"), new PushButtonFactory("Choose Color")), true);

    auto* fontSizeItem = new SettingsItem(colorGroup, "Font Size", "7", "Application font size");
    fontSizeItem->setFactory(new SpinBoxFactory(12, 8, 24), true);

    buildTreeWidget();
    createPagesForGroups();
}

void SettingsWindow::buildTreeWidget() {
    treeWidget->clear();
    addItemToTreeWidget(rootItem, nullptr);
    treeWidget->expandAll();
}

void SettingsWindow::addItemToTreeWidget(SettingsItem* item, QTreeWidgetItem* parent) {
    auto* treeItem = parent ? new QTreeWidgetItem(parent) : new QTreeWidgetItem(treeWidget);
    treeItem->setText(0, item->name());
    treeItem->setData(0, Qt::UserRole, QVariant::fromValue(item));

    if (item->isGroup()) {
        QFont font = treeItem->font(0);
        font.setBold(true);
        treeItem->setFont(0, font);
    }

    for (int i = 0; i < item->childCount(); ++i) {
        addItemToTreeWidget(item->child(i), treeItem);
    }
}

void SettingsWindow::createPagesForGroups() {
    for (SettingsItem* item : rootItem->getAllChildren()) {
        if (item->isGroup()) {
            createPageForGroup(item);
        }
    }
}

void SettingsWindow::createPageForGroup(SettingsItem* group) {
    auto* scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    auto* content = new QWidget();
    auto* layout = new QVBoxLayout(content);
    layout->setSpacing(15);
    layout->setContentsMargins(25, 25, 25, 25);

    auto* title = new QLabel(group->name());
    QFont f = title->font();
    f.setPointSize(16);
    f.setBold(true);
    title->setFont(f);
    layout->addWidget(title);

    if (!group->description().isEmpty()) {
        auto* desc = new QLabel(group->description());
        desc->setWordWrap(true);
        layout->addWidget(desc);
    }

    bool hasSettings = false;
    for (int i = 0; i < group->childCount(); ++i) {
        SettingsItem* child = group->child(i);
        if (!child->isGroup()) {
            QHBoxLayout* row = child->createWidget();
            if (row) {
                if (hasSettings) {
                    auto* sep = new QFrame();
                    sep->setFrameShape(QFrame::HLine);
                    sep->setFrameShadow(QFrame::Sunken);
                    layout->addWidget(sep);
                }
                layout->addLayout(row);
                hasSettings = true;
            }
        }
    }

    if (!hasSettings) {
        auto* empty = new QLabel("No settings available in this group");
        empty->setAlignment(Qt::AlignCenter);
        layout->addWidget(empty);
    }

    layout->addStretch();
    scroll->setWidget(content);

    groupPages[group] = scroll;
    stackedWidget->addWidget(scroll);
}

void SettingsWindow::setupConnections() {
    connect(treeWidget, &QTreeWidget::currentItemChanged, this, &SettingsWindow::onTreeItemChanged);
    connect(resetAllButton, &QPushButton::clicked, this, &SettingsWindow::onResetAllClicked);
    connect(resetGroupButton, &QPushButton::clicked, this, &SettingsWindow::onResetGroupClicked);
}

void SettingsWindow::onTreeItemChanged(QTreeWidgetItem* current, QTreeWidgetItem*) {
    if (!current) return;
    auto* item = current->data(0, Qt::UserRole).value<SettingsItem*>();
    if (item && item->isGroup() && groupPages.contains(item)) {
        stackedWidget->setCurrentWidget(groupPages[item]);
    }
}

void SettingsWindow::onResetAllClicked() {
    if (QMessageBox::question(this, "Reset All", "Reset ALL settings to default?",
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        for (SettingsItem* item : rootItem->getAllChildren()) {
            if (!item->isGroup() && item->isSavingEnabled()) {
                item->resetToDefault();
            }
        }
        saveSettings();
        QMessageBox::information(this, "Reset", "All settings reset to default.");
    }
}

void SettingsWindow::onResetGroupClicked() {
    auto* current = treeWidget->currentItem();
    if (!current) {
        QMessageBox::warning(this, "Error", "Please select a group.");
        return;
    }

    auto* group = current->data(0, Qt::UserRole).value<SettingsItem*>();
    if (!group || !group->isGroup()) {
        QMessageBox::warning(this, "Error", "Please select a valid group.");
        return;
    }

    if (QMessageBox::question(this, "Reset Group", QString("Reset '%1'?").arg(group->name()),
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        for (int i = 0; i < group->childCount(); ++i) {
            auto* child = group->child(i);
            if (!child->isGroup() && child->isSavingEnabled()) {
                child->resetToDefault();  // ← Сохраняет форматирование
            }
        }
        saveSettings();
        QMessageBox::information(this, "Reset", QString("'%1' reset.").arg(group->name()));
    }
}

void SettingsWindow::loadSettings() {
    QSettings s("TestLabs", "TestSettings");
    for (SettingsItem* item : rootItem->getAllChildren()) {
        if (!item->isSavingEnabled() || item->isGroup()) continue;
        QString path = buildSettingsPath(item);
        QVariant saved = s.value(path, item->defaultValue());
        if (saved != item->defaultValue()) {
            applyValueToWidget(item, saved);
        }
    }
}

void SettingsWindow::saveSettings() {
    QSettings s("TestLabs", "TestSettings");
    for (SettingsItem* item : rootItem->getAllChildren()) {
        if (!item->isSavingEnabled() || item->isGroup()) continue;
        s.setValue(buildSettingsPath(item), item->getValue());
    }
    s.sync();
}

QString SettingsWindow::buildSettingsPath(SettingsItem* item) const {
    QStringList parts;
    SettingsItem* cur = item;
    while (cur && !cur->id().isEmpty()) {
        parts.prepend(cur->id());
        cur = cur->parent();
    }
    return parts.join("/");
}

void SettingsWindow::applyValueToWidget(SettingsItem* item, const QVariant& value) {
    if (auto* cb = item->comboBox()) {
        int idx = cb->findText(value.toString());
        if (idx >= 0) cb->setCurrentIndex(idx);
    } else if (auto* cb = item->checkBox()) {
        cb->setChecked(value.toBool());
    } else if (auto* sb = item->spinBox()) {
        sb->setValue(value.toInt());
    } else if (auto* le = qobject_cast<QLineEdit*>(item->controlWidget())) {
        le->setText(value.toString());
    } else if (auto* wrapper = item->controlWidget()) {
        if (auto* le = wrapper->findChild<QLineEdit*>()) {
            le->setText(value.toString());
        }
    }
}

void SettingsWindow::connectSignalsForAutoSave() {
    for (SettingsItem* item : rootItem->getAllChildren()) {
        if (!item->isSavingEnabled() || item->isGroup()) continue;

        if (auto* cb = item->comboBox()) {
            connect(cb, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SettingsWindow::saveSettings, Qt::UniqueConnection);
        } else if (auto* cb = item->checkBox()) {
            connect(cb, &QCheckBox::toggled, this, &SettingsWindow::saveSettings, Qt::UniqueConnection);
        } else if (auto* sb = item->spinBox()) {
            connect(sb, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsWindow::saveSettings, Qt::UniqueConnection);
        } else if (auto* le = qobject_cast<QLineEdit*>(item->controlWidget())) {
            connect(le, &QLineEdit::textChanged, this, &SettingsWindow::saveSettings, Qt::UniqueConnection);
        } else if (auto* wrapper = item->controlWidget()) {
            if (auto* le = wrapper->findChild<QLineEdit*>()) {
                connect(le, &QLineEdit::textChanged, this, &SettingsWindow::saveSettings, Qt::UniqueConnection);
            }
        }
    }
}

void SettingsWindow::closeEvent(QCloseEvent* event) {
    saveSettings();
    event->accept();
}