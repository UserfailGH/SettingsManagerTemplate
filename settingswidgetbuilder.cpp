#include "settingswidgetbuilder.h"
#include "settingsitem.h"
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QStackedWidget>
#include <QScrollArea>
#include <QLineEdit>
#include <QSettings>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QLabel>
#include <QDebug>
#include <QFile>
#include <QPushButton>
#include <QMessageBox>

SettingsWidgetBuilder::SettingsWidgetBuilder(QList<SettingsItem*> widgetList, QObject* parent)
    : QObject(parent), widgetList_(widgetList), embedLayout_(nullptr), resetAllButton_(nullptr)
{
    setupTreeUI();
    loadSettings();
    connectSignalsForAutoSave();
}

void SettingsWidgetBuilder::setupTreeUI() {
    QTreeWidget* treeWidget = new QTreeWidget();
    treeWidget->setHeaderHidden(true);
    treeWidget->setFixedWidth(250);

    QStackedWidget* stackedWidget = new QStackedWidget();

    buildSettingsTree(treeWidget, stackedWidget);

    QWidget* containerWidget = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(containerWidget);

    QHBoxLayout* contentLayout = new QHBoxLayout();
    contentLayout->addWidget(treeWidget, 1);
    contentLayout->addWidget(stackedWidget, 3);

    resetAllButton_ = new QPushButton("Reset All Settings");

    mainLayout->addLayout(contentLayout);
    mainLayout->addWidget(resetAllButton_);

    embedLayout_ = new QHBoxLayout();
    QWidget* embedWidget = new QWidget();
    embedWidget->setLayout(mainLayout);
    embedLayout_->addWidget(embedWidget);

    connect(treeWidget, &QTreeWidget::currentItemChanged,
            this, &SettingsWidgetBuilder::onTreeItemChanged);

    connect(resetAllButton_, &QPushButton::clicked,
            this, &SettingsWidgetBuilder::resetAllSettings);

    treeWidget->expandAll();
}

void SettingsWidgetBuilder::resetAllSettings() {
    QMessageBox::StandardButton reply = QMessageBox::question(
        nullptr,
        "Reset All Settings",
        "Are you sure you want to reset all settings to default values?",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        for (SettingsItem* item : std::as_const(widgetList_)) {
            QList<SettingsItem*> allItems = item->getAllChildren();
            allItems.prepend(item);

            for (SettingsItem* setting : allItems) {
                if (!setting->isGroup()) {
                    setting->resetToDefault();
                }
            }
        }
        saveSettings();
    }
}

void SettingsWidgetBuilder::buildSettingsTree(QTreeWidget* treeWidget, QStackedWidget* stackedWidget) {
    treeWidget->clear();

    for (SettingsItem* item : std::as_const(widgetList_)) {
        if (item->parent() == nullptr) {
            addTreeItem(treeWidget, stackedWidget, item, nullptr);
        }
    }
}

void SettingsWidgetBuilder::addTreeItem(QTreeWidget* treeWidget, QStackedWidget* stackedWidget,
                                        SettingsItem* settingsItem, QTreeWidgetItem* parentTreeItem) {
    QTreeWidgetItem* treeItem;
    if (parentTreeItem) {
        treeItem = new QTreeWidgetItem(parentTreeItem);
    } else {
        treeItem = new QTreeWidgetItem(treeWidget);
    }

    treeItem->setText(0, settingsItem->name());
    treeItem->setData(0, Qt::UserRole, QVariant::fromValue(settingsItem));

    if (settingsItem->isGroup()) {
        QFont font = treeItem->font(0);
        font.setBold(true);
        treeItem->setFont(0, font);

        createGroupPage(stackedWidget, settingsItem);
    }

    for (int i = 0; i < settingsItem->childCount(); ++i) {
        addTreeItem(treeWidget, stackedWidget, settingsItem->child(i), treeItem);
    }
}

void SettingsWidgetBuilder::createGroupPage(QStackedWidget* stackedWidget, SettingsItem* groupItem) {
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);

    QWidget* contentWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(contentWidget);

    QLabel* titleLabel = new QLabel(groupItem->name());
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    layout->addWidget(titleLabel);

    if (!groupItem->description().isEmpty()) {
        QLabel* descLabel = new QLabel(groupItem->description());
        descLabel->setWordWrap(true);
        layout->addWidget(descLabel);
    }

    for (int i = 0; i < groupItem->childCount(); ++i) {
        SettingsItem* child = groupItem->child(i);
        if (!child->isGroup()) {
            QHBoxLayout* itemLayout = child->createWidget();
            if (itemLayout) {
                layout->addLayout(itemLayout);
            }
        }
    }

    layout->addStretch();
    scrollArea->setWidget(contentWidget);
    stackedWidget->addWidget(scrollArea);

    groupPages_[groupItem] = scrollArea;
}

void SettingsWidgetBuilder::onTreeItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous) {
    if (!current) return;

    SettingsItem* settingsItem = current->data(0, Qt::UserRole).value<SettingsItem*>();
    if (settingsItem && settingsItem->isGroup()) {
        if (groupPages_.contains(settingsItem)) {
            QWidget* page = groupPages_[settingsItem];
            QStackedWidget* stackedWidget = findChild<QStackedWidget*>();
            if (stackedWidget) {
                int index = stackedWidget->indexOf(page);
                if (index >= 0) {
                    stackedWidget->setCurrentIndex(index);
                }
            }
        }
    }
}

void SettingsWidgetBuilder::loadSettings() {
    QSettings settings("TestLabs", "TestSettings");

    for (SettingsItem* item : std::as_const(widgetList_)) {
        QList<SettingsItem*> allItems = item->getAllChildren();
        allItems.prepend(item);

        for (SettingsItem* setting : allItems) {
            if (!setting->isSavingEnabled() || setting->isGroup()) continue;

            QString key = setting->id();
            QVariant defaultValue = setting->defaultValue();
            QVariant savedValue = settings.value(key, defaultValue);

            if (savedValue.isValid() && savedValue != defaultValue) {
                applyValueToWidget(setting, savedValue);
            } else {
                applyValueToWidget(setting, defaultValue);
            }
        }
    }
}

void SettingsWidgetBuilder::saveSettings() {
    QSettings settings("TestLabs", "TestSettings");

    for (SettingsItem* item : std::as_const(widgetList_)) {
        if (!item->isSavingEnabled() || item->isGroup()) continue;

        QVariant value = item->getValue();
        settings.setValue(item->id(), value);
    }

    settings.sync();
}

void SettingsWidgetBuilder::applyValueToWidget(SettingsItem* item, const QVariant& value) {
    if (QComboBox* comboBox = item->comboBox()) {
        QString text = value.toString();
        int index = comboBox->findText(text);
        if (index >= 0) {
            comboBox->setCurrentIndex(index);
        }
    }
    else if (QCheckBox* checkBox = item->checkBox()) {
        bool checked = value.toBool();
        checkBox->setChecked(checked);
    }
    else if (QSpinBox* spinBox = item->spinBox()) {
        int val = value.toInt();
        spinBox->setValue(val);
    }
    else if (QLineEdit* lineEdit = qobject_cast<QLineEdit*>(item->controlWidget())) {
        QString text = value.toString();
        lineEdit->setText(text);
    }
    else if (QWidget* compositeWidget = item->controlWidget()) {
        QLineEdit* lineEdit = compositeWidget->findChild<QLineEdit*>();
        if (lineEdit) {
            QString text = value.toString();
            lineEdit->setText(text);
        }
    }
}

void SettingsWidgetBuilder::connectSignalsForAutoSave() {
    for (SettingsItem* item : std::as_const(widgetList_)) {
        if (!item->isSavingEnabled() || item->isGroup()) continue;

        QWidget* control = item->controlWidget();
        if (!control) continue;

        if (auto* combo = qobject_cast<QComboBox*>(control)) {
            connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                   [this]() {
                       this->saveSettings();
                   });
        }
        else if (auto* check = qobject_cast<QCheckBox*>(control)) {
            connect(check, &QCheckBox::toggled,
                   [this]() {
                       this->saveSettings();
                   });
        }
        else if (auto* spin = qobject_cast<QSpinBox*>(control)) {
            connect(spin, QOverload<int>::of(&QSpinBox::valueChanged),
                   [this]() {
                       this->saveSettings();
                   });
        }
        else if (auto* lineEdit = qobject_cast<QLineEdit*>(control)) {
            connect(lineEdit, &QLineEdit::textChanged,
                   [this]() {
                       this->saveSettings();
                   });
        }
        else {
            QLineEdit* innerLineEdit = control->findChild<QLineEdit*>();
            if (innerLineEdit) {
                connect(innerLineEdit, &QLineEdit::textChanged,
                       [this]() {
                           this->saveSettings();
                       });
            }
        }
    }
}

QWidget* SettingsWidgetBuilder::getEmbeddedWidget() const {
    return embedLayout_->parentWidget();
}

SettingsWidgetBuilder::~SettingsWidgetBuilder() {
    QSettings settings("TestLabs", "TestSettings");

    for (SettingsItem* item : std::as_const(widgetList_)) {
        if (!item->isSavingEnabled() || item->isGroup()) continue;

        settings.setValue(item->id(), item->getValue());
    }

    settings.sync();
}