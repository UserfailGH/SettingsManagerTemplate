#ifndef SETTINGSITEM_H
#define SETTINGSITEM_H

#include <QHBoxLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>

#include "settingscontrolfactory.h"

class SettingsItem {
public:
    explicit SettingsItem(SettingsItem* parent,
                          const QString& name,
                          const QString& id,
                          const QString& description = "",
                          SettingsControlFactory* factory = nullptr,
                          bool savingEnabled = true);

    ~SettingsItem();

    QString name() const { return name_; }
    QString id() const { return id_; }
    QString description() const { return description_; }
    bool isGroup() const { return childCount() > 0 && !factory_; }

    void addChild(SettingsItem* child);
    SettingsItem* child(int index) const;
    int childCount() const { return children_.size(); }
    SettingsItem* parent() const { return parent_; }

    SettingsControlFactory* factory() const { return factory_; }
    void setFactory(SettingsControlFactory* factory, bool savingEnabled = true);

    QVariant defaultValue() const;
    void resetToDefault();

    QHBoxLayout* createWidget();
    QWidget* controlWidget() const { return controlWidget_; }
    void setControlWidget(QWidget* widget) { controlWidget_ = widget; }

    bool isSavingEnabled() const { return savingEnabled_; }
    QVariant getValue() const;

    QList<SettingsItem*> getAllChildren() const;

    QComboBox* comboBox() const;
    QCheckBox* checkBox() const;
    QSpinBox* spinBox() const;

private:
    QString name_;
    QString id_;
    QString description_;

    SettingsItem* parent_;
    QList<SettingsItem*> children_;

    SettingsControlFactory* factory_;
    QWidget* controlWidget_ = nullptr;
    bool savingEnabled_;
};

#endif // SETTINGSITEM_H