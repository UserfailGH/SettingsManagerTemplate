#include "settingsitem.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>

// Конструктор для обычных настроек
SettingsItem::SettingsItem(SettingsItem* parent, const QString& name, const QString& id,
                           const QString& description, SettingsControlFactory* factory,
                           bool enableSaving)
    : parentItem_(parent), name_(name), id_(id), description_(description),
      factory_(factory), controlWidget_(nullptr), enableSaving_(enableSaving)
{
    if (parentItem_) {
        parentItem_->appendChild(this);
    }
}

// Конструктор для групп
SettingsItem::SettingsItem(SettingsItem* parent, const QString& name, const QString& id,
                           const QString& description)
    : parentItem_(parent), name_(name), id_(id), description_(description),
      factory_(nullptr), controlWidget_(nullptr), enableSaving_(false)
{
    if (parentItem_) {
        parentItem_->appendChild(this);
    }
}

SettingsItem::~SettingsItem() {
    delete factory_;
    delete controlWidget_;
    qDeleteAll(childItems_);
}

// Методы для древовидной структуры
SettingsItem* SettingsItem::parent() const {
    return parentItem_;
}

void SettingsItem::appendChild(SettingsItem* child) {
    childItems_.append(child);
}

SettingsItem* SettingsItem::child(int row) {
    if (row < 0 || row >= childItems_.size())
        return nullptr;
    return childItems_.at(row);
}

int SettingsItem::childCount() const {
    return childItems_.count();
}

int SettingsItem::row() const {
    if (parentItem_)
        return parentItem_->childItems_.indexOf(const_cast<SettingsItem*>(this));
    return 0;
}

QList<SettingsItem*> SettingsItem::getAllChildren() const {
    QList<SettingsItem*> allChildren;
    for (SettingsItem* child : childItems_) {
        allChildren.append(child);
        allChildren.append(child->getAllChildren());
    }
    return allChildren;
}

SettingsItem* SettingsItem::findItemById(const QString& id) const {
    if (id_ == id) return const_cast<SettingsItem*>(this);

    for (SettingsItem* child : childItems_) {
        SettingsItem* found = child->findItemById(id);
        if (found) return found;
    }

    return nullptr;
}

SettingsItem* SettingsItem::findItemByName(const QString& name) const {
    if (name_ == name) return const_cast<SettingsItem*>(this);

    for (SettingsItem* child : childItems_) {
        SettingsItem* found = child->findItemByName(name);
        if (found) return found;
    }

    return nullptr;
}

QString SettingsItem::name() const {
    return name_;
}

QString SettingsItem::id() const {
    return id_;
}

QString SettingsItem::description() const {
    return description_;
}

QString SettingsItem::value() const {
    return value_;
}

void SettingsItem::setValue(const QVariant& value) {
    value_ = value.toString();
}

SettingsControlFactory* SettingsItem::factory() const {
    return factory_;
}

QHBoxLayout* SettingsItem::createWidget() const {
    if (isGroup()) {
        return nullptr;
    }

    QHBoxLayout* rowLayout = new QHBoxLayout();
    QVBoxLayout* leftLayout = new QVBoxLayout();
    QLabel* nameLabel = new QLabel(name_);
    QLabel* hintLabel = new QLabel(description_);
    leftLayout->addWidget(nameLabel);
    leftLayout->addWidget(hintLabel);

    controlWidget_ = factory_->create();
    rowLayout->addLayout(leftLayout);
    rowLayout->addWidget(controlWidget_, 1);

    return rowLayout;
}

QWidget* SettingsItem::controlWidget() const {
    return controlWidget_;
}

QVariant SettingsItem::getValue() const {
    if (!controlWidget_ || isGroup()) {
        return QVariant();
    }

    if (QLineEdit* lineEdit = qobject_cast<QLineEdit*>(controlWidget_)) {
        return lineEdit->text();
    } else if (QComboBox* comboBox = qobject_cast<QComboBox*>(controlWidget_)) {
        return comboBox->currentText();
    } else if (QCheckBox* checkBox = qobject_cast<QCheckBox*>(controlWidget_)) {
        return checkBox->isChecked();
    } else if (QSpinBox* spinBox = qobject_cast<QSpinBox*>(controlWidget_)) {
        return spinBox->value();
    } else if (QWidget* fileBrowse = qobject_cast<QWidget*>(controlWidget_)) {
        QLineEdit* lineEdit = fileBrowse->findChild<QLineEdit*>();
        if (lineEdit) {
            return lineEdit->text();
        }
    }

    return QVariant();
}

bool SettingsItem::isSavingEnabled() const {
    return enableSaving_ && !isGroup();
}

QComboBox* SettingsItem::comboBox() const {
    return qobject_cast<QComboBox*>(controlWidget_);
}

QCheckBox* SettingsItem::checkBox() const {
    return qobject_cast<QCheckBox*>(controlWidget_);
}

QSpinBox* SettingsItem::spinBox() const {
    return qobject_cast<QSpinBox*>(controlWidget_);
}