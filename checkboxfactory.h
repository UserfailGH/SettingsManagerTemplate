#ifndef CHECKBOXFACTORY_H
#define CHECKBOXFACTORY_H

#include "settingscontrolfactory.h"
#include <QtWidgets/QCheckBox>
#include <QVariant>

class CheckBoxFactory : public SettingsControlFactory {
public:
    explicit CheckBoxFactory(const QVariant& defaultValue = false);

    QWidget* create() const override;
    QVariant defaultValue() const override;

private:
    QVariant defaultValue_;
};

#endif // CHECKBOXFACTORY_H