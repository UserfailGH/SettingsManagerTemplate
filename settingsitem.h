#ifndef SETTINGSITEM_H
#define SETTINGSITEM_H

#include <QHBoxLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QLineEdit>

#include "settingscontrolfactory.h"

class SettingsItem {
public:
    SettingsItem(const QString& id,
                 const QString& name,
                 const QString& description,
                 const QVariant& defaultValue,
                 SettingsItem* parent = nullptr,
                 SettingsControlFactory* factory = nullptr,
                 bool enableSaving = true);

    SettingsItem(const QString& id,
                 const QString& name,
                 const QString& description,
                 SettingsItem* parent = nullptr);

    ~SettingsItem();

    SettingsItem* parent() const { return parent_; }
    void appendChild(SettingsItem* child);
    SettingsItem* child(int row) const;
    int childCount() const { return children_.size(); }
    int row() const;

    bool isGroup() const {
        return (parent_ == nullptr || factory_ == nullptr);
    }

    QList<SettingsItem*> getAllChildren() const;
    SettingsItem* findItemById(const QString& id) const;
    SettingsItem* findItemByName(const QString& name) const;

    // Основные методы
    QString name() const { return name_; }
    QString id() const { return id_; }
    QString description() const { return description_; }
    QVariant defaultValue() const { return defaultValue_; }
    SettingsControlFactory* factory() const { return factory_; }

    bool isSavingEnabled() const { return enableSaving_; }

    void resetToDefault();
    QHBoxLayout* createWidget();
    QWidget* controlWidget() const { return controlWidget_; }
    void setControlWidget(QWidget* widget) { controlWidget_ = widget; }
    QVariant getValue() const;

    QComboBox* comboBox() const;
    QCheckBox* checkBox() const;
    QSpinBox* spinBox() const;

private:
    SettingsItem* parent_;
    QList<SettingsItem*> children_;

    QString name_;
    QString id_;
    QString description_;
    QVariant defaultValue_;

    SettingsControlFactory* factory_;
    QWidget* controlWidget_ = nullptr;
    bool enableSaving_;
};

#endif // SETTINGSITEM_H