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
#include <QDir>

SettingsWidgetBuilder::SettingsWidgetBuilder(QList<SettingsItem*> widgetList, QObject* parent)
    : QObject(parent), widgetList_(widgetList)
{
    qDebug() << "=== SettingsWidgetBuilder constructor ===";
    qDebug() << "Total items:" << widgetList_.size();

    // Показываем где будут настройки
    QSettings settings("TestLabs", "TestSettings");
    qDebug() << "Settings will be stored in:" << settings.fileName();
    qDebug() << "Settings file exists:" << QFile(settings.fileName()).exists();

    setupTreeUI();
    loadSettings();
    connectSignalsForAutoSave();

    qDebug() << "=== SettingsWidgetBuilder initialized ===";
}

void SettingsWidgetBuilder::loadSettings() {
    qDebug() << "\n=== LOADING SETTINGS ===";
    QSettings settings("TestLabs", "TestSettings");

    qDebug() << "Settings file:" << settings.fileName();
    qDebug() << "File exists:" << QFile(settings.fileName()).exists();

    // Показываем все что уже есть в настройках
    qDebug() << "All keys in settings:";
    QStringList allKeys = settings.allKeys();
    for (const QString& key : allKeys) {
        qDebug() << "  " << key << "=" << settings.value(key);
    }

    if (allKeys.isEmpty()) {
        qDebug() << "  No keys found in settings!";
    }

    // Собираем все элементы которые можно сохранять
    QList<SettingsItem*> allItems;
    for (SettingsItem* item : std::as_const(widgetList_)) {
        allItems.append(item);
        allItems.append(item->getAllChildren());
    }

    qDebug() << "Total items to check:" << allItems.size();

    int loadedCount = 0;
    for (SettingsItem* item : allItems) {
        if (!item->isSavingEnabled() || item->isGroup()) {
            qDebug() << "Skipping (group/no save):" << item->name();
            continue;
        }

        QString key = item->id(); // Используем ID как ключ (плоская структура)
        QVariant defaultValue = item->getValue();
        QVariant savedValue = settings.value(key, defaultValue);

        qDebug() << "Item:" << item->name();
        qDebug() << "  Key:" << key;
        qDebug() << "  Default value:" << defaultValue;
        qDebug() << "  Saved value:" << savedValue;
        qDebug() << "  Has saved value:" << savedValue.isValid();

        if (savedValue.isValid() && savedValue != defaultValue) {
            applyValueToWidget(item, savedValue);
            loadedCount++;
            qDebug() << "  ✓ LOADED from settings";
        } else {
            // Всегда применяем значение (даже дефолтное)
            applyValueToWidget(item, defaultValue);
            qDebug() << "  → Using default value";
        }
    }

    qDebug() << "=== LOADING COMPLETE ===";
    qDebug() << "Loaded values for" << loadedCount << "items";
}

void SettingsWidgetBuilder::saveSettings() {
    qDebug() << "\n💾 MANUAL SAVE SETTINGS CALLED";

    QSettings settings("TestLabs", "TestSettings");

    int count = 0;
    for (SettingsItem* item : std::as_const(widgetList_)) {
        if (!item->isSavingEnabled() || item->isGroup()) continue;

        QVariant value = item->getValue();
        settings.setValue(item->id(), value);
        qDebug() << "Saved" << item->id() << "=" << value;
        count++;
    }

    settings.sync(); // ВАЖНО!

    qDebug() << "✅ Saved" << count << "settings";
    qDebug() << "Settings file:" << settings.fileName();

    // Проверяем что сохранилось
    QSettings check("TestLabs", "TestSettings");
    qDebug() << "Stored keys:";
    for (const QString& key : check.allKeys()) {
        qDebug() << "  " << key << "=" << check.value(key);
    }
}

void SettingsWidgetBuilder::applyValueToWidget(SettingsItem* item, const QVariant& value) {
    qDebug() << "  Applying to widget -" << item->name() << "value:" << value;

    bool applied = false;

    if (QComboBox* comboBox = item->comboBox()) {
        QString text = value.toString();
        int index = comboBox->findText(text);
        if (index >= 0) {
            comboBox->setCurrentIndex(index);
            qDebug() << "    ✓ ComboBox set to:" << text << "index:" << index;
            applied = true;
        } else {
            qDebug() << "    ✗ ComboBox text not found:" << text;
            qDebug() << "    Available options:" << comboBox->count();
            for (int i = 0; i < comboBox->count(); i++) {
                qDebug() << "      " << i << ":" << comboBox->itemText(i);
            }
        }
    }
    else if (QCheckBox* checkBox = item->checkBox()) {
        bool checked = value.toBool();
        checkBox->setChecked(checked);
        qDebug() << "    ✓ CheckBox set to:" << checked;
        applied = true;
    }
    else if (QSpinBox* spinBox = item->spinBox()) {
        int val = value.toInt();
        spinBox->setValue(val);
        qDebug() << "    ✓ SpinBox set to:" << val;
        applied = true;
    }
    else if (QLineEdit* lineEdit = qobject_cast<QLineEdit*>(item->controlWidget())) {
        QString text = value.toString();
        lineEdit->setText(text);
        qDebug() << "    ✓ LineEdit set to:" << text;
        applied = true;
    }
    else if (QWidget* compositeWidget = item->controlWidget()) {
        QLineEdit* lineEdit = compositeWidget->findChild<QLineEdit*>();
        if (lineEdit) {
            QString text = value.toString();
            lineEdit->setText(text);
            qDebug() << "    ✓ Composite LineEdit set to:" << text;
            applied = true;
        } else {
            qDebug() << "    ✗ No LineEdit found in composite widget";
            qDebug() << "    Widget children:";
            for (QObject* child : compositeWidget->children()) {
                qDebug() << "      " << child->metaObject()->className() << child->objectName();
            }
        }
    }
    else {
        qDebug() << "    ✗ Unknown widget type";
        if (item->controlWidget()) {
            qDebug() << "    Widget type:" << item->controlWidget()->metaObject()->className();
        } else {
            qDebug() << "    No control widget!";
        }
    }

    if (!applied) {
        qDebug() << "    ✗✗✗ FAILED to apply value to widget!";
    }
}

void SettingsWidgetBuilder::connectSignalsForAutoSave() {
    qDebug() << "\n=== CONNECTING SIGNALS ===";

    for (SettingsItem* item : std::as_const(widgetList_)) {
        if (!item->isSavingEnabled() || item->isGroup()) continue;

        QWidget* control = item->controlWidget();
        if (!control) continue;

        // ПРОСТОЙ И НАДЕЖНЫЙ СПОСОБ:
        // Подключаемся к любому изменению
        if (auto* combo = qobject_cast<QComboBox*>(control)) {
            connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                   [this]() {
                       qDebug() << "ComboBox changed, saving...";
                       this->saveSettings();
                   });
        }
        else if (auto* check = qobject_cast<QCheckBox*>(control)) {
            connect(check, &QCheckBox::toggled,
                   [this]() {
                       qDebug() << "CheckBox changed, saving...";
                       this->saveSettings();
                   });
        }
        else if (auto* spin = qobject_cast<QSpinBox*>(control)) {
            connect(spin, QOverload<int>::of(&QSpinBox::valueChanged),
                   [this]() {
                       qDebug() << "SpinBox changed, saving...";
                       this->saveSettings();
                   });
        }
        else if (auto* lineEdit = qobject_cast<QLineEdit*>(control)) {
            connect(lineEdit, &QLineEdit::textChanged,
                   [this]() {
                       qDebug() << "LineEdit changed, saving...";
                       this->saveSettings();
                   });
        }
        else {
            // Для составных виджетов
            QLineEdit* innerLineEdit = control->findChild<QLineEdit*>();
            if (innerLineEdit) {
                connect(innerLineEdit, &QLineEdit::textChanged,
                       [this]() {
                           qDebug() << "Inner LineEdit changed, saving...";
                           this->saveSettings();
                       });
            }
        }
    }
}

// Остальные методы без изменений
void SettingsWidgetBuilder::setupTreeUI() {
    QTreeWidget* treeWidget = new QTreeWidget();
    treeWidget->setHeaderHidden(true);
    treeWidget->setFixedWidth(250);

    QStackedWidget* stackedWidget = new QStackedWidget();

    buildSettingsTree(treeWidget, stackedWidget);

    QWidget* containerWidget = new QWidget();
    embedLayout_ = new QHBoxLayout(containerWidget);
    embedLayout_->addWidget(treeWidget, 1);
    embedLayout_->addWidget(stackedWidget, 3);

    connect(treeWidget, &QTreeWidget::currentItemChanged,
            this, &SettingsWidgetBuilder::onTreeItemChanged);

    treeWidget->expandAll();
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

QWidget* SettingsWidgetBuilder::getEmbeddedWidget() const {
    return embedLayout_->parentWidget();
}

SettingsWidgetBuilder::~SettingsWidgetBuilder() {
    qDebug() << "=== DESTRUCTOR: Saving settings ===";

    // СОХРАНЯЕМ ВСЕ НАСТРОЙКИ ПРИ ЗАКРЫТИИ
    QSettings settings("TestLabs", "TestSettings");

    for (SettingsItem* item : std::as_const(widgetList_)) {
        if (!item->isSavingEnabled() || item->isGroup()) continue;

        settings.setValue(item->id(), item->getValue());
        qDebug() << "Saved:" << item->id() << "=" << item->getValue();
    }

    settings.sync(); // ВАЖНО: принудительно сохраняем
    qDebug() << "Settings saved in destructor";
}