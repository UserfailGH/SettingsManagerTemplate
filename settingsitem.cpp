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

SettingsItem::SettingsItem(SettingsItem* parent,
                           const QString& name,
                           const QString& id,
                           const QString& description,
                           SettingsControlFactory* factory,
                           bool savingEnabled)
    : parent_(parent), name_(name), id_(id), description_(description),
      factory_(factory), savingEnabled_(savingEnabled)
{
    if (parent_) {
        parent_->addChild(this);
    }
}

SettingsItem::~SettingsItem()
{
    qDeleteAll(children_);
    delete controlWidget_;
}

void SettingsItem::setFactory(SettingsControlFactory* factory, bool savingEnabled)
{
    factory_ = factory;
    savingEnabled_ = savingEnabled;
}

QVariant SettingsItem::defaultValue() const
{
    if (factory_) {
        return factory_->defaultValue();
    }
    return QVariant();
}

void SettingsItem::resetToDefault()
{
    if (!factory_ || !controlWidget_) {
        qWarning() << "Нельзя сбросить: нет фабрики или controlWidget";
        return;
    }

    QWidget* wrapper = controlWidget_;
    QHBoxLayout* wrapperLayout = qobject_cast<QHBoxLayout*>(wrapper->layout());
    if (!wrapperLayout) {
        qWarning() << "Layout обёртки не найден";
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
        qWarning() << "Старый контроль не найден";
        return;
    }

    QWidget* newControl = factory_->create();
    if (!newControl) {
        qWarning() << "Фабрика вернула nullptr";
        return;
    }

    wrapperLayout->removeWidget(oldControl);
    delete oldControl;

    wrapperLayout->insertWidget(0, newControl);

    wrapper->update();
    wrapper->adjustSize();
}

void SettingsItem::addChild(SettingsItem* child)
{
    children_.append(child);
}

SettingsItem* SettingsItem::child(int index) const
{
    if (index >= 0 && index < children_.size()) {
        return children_[index];
    }
    return nullptr;
}

QHBoxLayout* SettingsItem::createWidget()
{
    if (!factory_) {
        qWarning() << "Нет фабрики для создания виджета";
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
        qWarning() << "Фабрика вернула nullptr при создании контроля";
        return nullptr;
    }

    controlWidget_ = SettingsControlFactory::createControlWithReset(this, control);
    if (!controlWidget_) {
        qWarning() << "createControlWithReset вернул nullptr";
        delete control;
        return nullptr;
    }

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

    if (auto* combo = qobject_cast<QComboBox*>(controlWidget_)) {
        return combo->currentText();
    }
    if (auto* check = qobject_cast<QCheckBox*>(controlWidget_)) {
        return check->isChecked();
    }
    if (auto* spin = qobject_cast<QSpinBox*>(controlWidget_)) {
        return spin->value();
    }
    if (auto* edit = qobject_cast<QLineEdit*>(controlWidget_)) {
        return edit->text();
    }

    return QVariant();
}

QComboBox* SettingsItem::comboBox() const
{
    if (auto* wrapper = qobject_cast<QWidget*>(controlWidget_)) {
        return wrapper->findChild<QComboBox*>();
    }
    return qobject_cast<QComboBox*>(controlWidget_);
}

QCheckBox* SettingsItem::checkBox() const
{
    if (auto* wrapper = qobject_cast<QWidget*>(controlWidget_)) {
        return wrapper->findChild<QCheckBox*>();
    }
    return qobject_cast<QCheckBox*>(controlWidget_);
}

QSpinBox* SettingsItem::spinBox() const
{
    if (auto* wrapper = qobject_cast<QWidget*>(controlWidget_)) {
        return wrapper->findChild<QSpinBox*>();
    }
    return qobject_cast<QSpinBox*>(controlWidget_);
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