#include "settingsitem.h"
#include "settingscontrolfactory.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QDebug>

SettingsItem::SettingsItem(const QString& id,
                           const QString& name,
                           const QString& description,
                           const QVariant& defaultValue,
                           SettingsItem* parent,
                           SettingsControlFactory* factory,
                           bool enableSaving)
    : parent_(parent)
    , name_(name)
    , id_(id)
    , description_(description)
    , defaultValue_(defaultValue)
    , factory_(factory)
    , enableSaving_(enableSaving)
{
    if (parent_) {
        parent_->appendChild(this);
    }
}

SettingsItem::SettingsItem(const QString& id,
                           const QString& name,
                           const QString& description,
                           SettingsItem* parent)
    : parent_(parent)
    , name_(name)
    , id_(id)
    , description_(description)
    , factory_(nullptr)
    , enableSaving_(false)
{
    if (parent_) {
        parent_->appendChild(this);
    }
}

SettingsItem::~SettingsItem()
{
    qDeleteAll(children_);
    delete controlWidget_;
}

void SettingsItem::appendChild(SettingsItem* child)
{
    children_.append(child);
}

SettingsItem* SettingsItem::child(int row) const
{
    if (row >= 0 && row < children_.size()) {
        return children_[row];
    }
    return nullptr;
}

int SettingsItem::row() const
{
    if (parent_) {
        return parent_->children_.indexOf(const_cast<SettingsItem*>(this));
    }
    return 0;
}

QList<SettingsItem*> SettingsItem::getAllChildren() const
{
    QList<SettingsItem*> result;
    for (SettingsItem* child : children_) {
        result.append(child);
        result.append(child->getAllChildren());
    }
    return result;
}

SettingsItem* SettingsItem::findItemById(const QString& id) const
{
    if (id_ == id) return const_cast<SettingsItem*>(this);
    for (SettingsItem* child : children_) {
        SettingsItem* found = child->findItemById(id);
        if (found) return found;
    }
    return nullptr;
}

SettingsItem* SettingsItem::findItemByName(const QString& name) const
{
    if (name_ == name) return const_cast<SettingsItem*>(this);
    for (SettingsItem* child : children_) {
        SettingsItem* found = child->findItemByName(name);
        if (found) return found;
    }
    return nullptr;
}

void SettingsItem::resetToDefault()
{
    if (!factory_ || !controlWidget_) {
        qWarning() << "Cannot reset: no factory or controlWidget";
        return;
    }

    QWidget* wrapper = controlWidget_;
    QHBoxLayout* wrapperLayout = qobject_cast<QHBoxLayout*>(wrapper->layout());
    if (!wrapperLayout) {
        qWarning() << "Wrapper layout not found";
        return;
    }

    QWidget* oldControl = nullptr;
    for (int i = 0; i < wrapperLayout->count(); ++i) {
        QLayoutItem* item = wrapperLayout->itemAt(i);
        if (item && item->widget() && qobject_cast<QPushButton*>(item->widget()) == nullptr) {
            oldControl = item->widget();
            break;
        }
    }

    if (!oldControl) {
        qWarning() << "Old control not found";
        return;
    }

    QWidget* newControl = factory_->create();
    if (!newControl) {
        qWarning() << "Factory returned nullptr";
        return;
    }

    if (auto* spinBox = qobject_cast<QSpinBox*>(newControl)) {
        spinBox->setValue(defaultValue_.toInt());
    } else if (auto* lineEdit = qobject_cast<QLineEdit*>(newControl)) {
        lineEdit->setText(defaultValue_.toString());
    } else if (auto* checkBox = qobject_cast<QCheckBox*>(newControl)) {
        checkBox->setChecked(defaultValue_.toBool());
    } else if (auto* comboBox = qobject_cast<QComboBox*>(newControl)) {
        int index = comboBox->findText(defaultValue_.toString());
        if (index >= 0) comboBox->setCurrentIndex(index);
    }

    wrapperLayout->removeWidget(oldControl);
    delete oldControl;
    wrapperLayout->insertWidget(0, newControl);

    wrapper->update();
    wrapper->adjustSize();
}

QHBoxLayout* SettingsItem::createWidget()
{
    if (!factory_) {
        qWarning() << "No factory to create widget";
        return nullptr;
    }

    QHBoxLayout* layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    QLabel* label = new QLabel(name_ + ":");
    label->setMinimumWidth(150);
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    layout->addWidget(label);

    QWidget* control = factory_->create();
    if (!control) {
        qWarning() << "Factory returned nullptr";
        return nullptr;
    }

    // Устанавливаем значение по умолчанию из defaultValue_
    if (auto* spinBox = qobject_cast<QSpinBox*>(control)) {
        spinBox->setValue(defaultValue_.toInt());
    } else if (auto* lineEdit = qobject_cast<QLineEdit*>(control)) {
        lineEdit->setText(defaultValue_.toString());
    } else if (auto* checkBox = qobject_cast<QCheckBox*>(control)) {
        checkBox->setChecked(defaultValue_.toBool());
    } else if (auto* comboBox = qobject_cast<QComboBox*>(control)) {
        int index = comboBox->findText(defaultValue_.toString());
        if (index >= 0) comboBox->setCurrentIndex(index);
    }

    controlWidget_ = SettingsControlFactory::createControlWithReset(this, control);
    layout->addWidget(controlWidget_, 1);

    return layout;
}

QVariant SettingsItem::getValue() const
{
    if (!controlWidget_) return QVariant();

    if (auto* wrapper = qobject_cast<QWidget*>(controlWidget_)) {
        if (auto* combo = wrapper->findChild<QComboBox*>()) {
            return combo->currentText();
        }
        if (auto* check = wrapper->findChild<QCheckBox*>()) {
            return check->isChecked();
        }
        if (auto* spin = wrapper->findChild<QSpinBox*>()) {
            return spin->value();
        }
        if (auto* edit = wrapper->findChild<QLineEdit*>()) {
            return edit->text();
        }
    }
    return QVariant();
}

QComboBox* SettingsItem::comboBox() const
{
    if (auto* wrapper = qobject_cast<QWidget*>(controlWidget_)) {
        return wrapper->findChild<QComboBox*>();
    }
    return nullptr;
}

QCheckBox* SettingsItem::checkBox() const
{
    if (auto* wrapper = qobject_cast<QWidget*>(controlWidget_)) {
        return wrapper->findChild<QCheckBox*>();
    }
    return nullptr;
}

QSpinBox* SettingsItem::spinBox() const
{
    if (auto* wrapper = qobject_cast<QWidget*>(controlWidget_)) {
        return wrapper->findChild<QSpinBox*>();
    }
    return nullptr;
}