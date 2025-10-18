#ifndef SETTINGSITEM_H
#define SETTINGSITEM_H

#include "settingscontrolfactory.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QString>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QList>

class SettingsItem {
public:
    // Конструктор для обычных настроек
    SettingsItem(SettingsItem* parent, const QString& name, const QString& id, const QString& description,
                 SettingsControlFactory* factory, bool enableSaving = true);

    // Конструктор для групп (без factory и enableSaving)
    SettingsItem(SettingsItem* parent, const QString& name, const QString& id, const QString& description);

    ~SettingsItem();

    // Методы для древовидной структуры
    SettingsItem* parent() const;
    void appendChild(SettingsItem* child);
    SettingsItem* child(int row);
    int childCount() const;
    int row() const;

    // Группа - это элемент без родителя ИЛИ элемент без factory
    bool isGroup() const {
        return (parent() == nullptr) || (factory() == nullptr);
    }

    QList<SettingsItem*> getAllChildren() const;
    SettingsItem* findItemById(const QString& id) const;
    SettingsItem* findItemByName(const QString& name) const;

    // Основные методы
    QString name() const;
    QString id() const;
    QString description() const;
    QString value() const;
    void setValue(const QVariant& value);
    SettingsControlFactory* factory() const;

    QHBoxLayout* createWidget() const;
    QWidget* controlWidget() const;
    QVariant getValue() const;

    bool isSavingEnabled() const;

    QComboBox* comboBox() const;
    QCheckBox* checkBox() const;
    QSpinBox* spinBox() const;

private:
    SettingsItem* parentItem_;
    QList<SettingsItem*> childItems_;
    QString name_;
    QString id_;
    QString description_;
    QString value_;
    SettingsControlFactory* factory_;
    mutable QWidget* controlWidget_;
    bool enableSaving_;
};

#endif // SETTINGSITEM_H