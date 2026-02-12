#ifndef SETTINGSCONTROLFACTORY_H
#define SETTINGSCONTROLFACTORY_H

#include <QtWidgets/QWidget>
#include <QVariant>

class SettingsItem;

class SettingsControlFactory {
public:
    virtual ~SettingsControlFactory() = default;
    virtual QWidget* create() const = 0;
    virtual QVariant defaultValue() const = 0;

    static QWidget* createControlWithReset(SettingsItem* item, QWidget* controlWidget);
};

#endif // SETTINGSCONTROLFACTORY_H