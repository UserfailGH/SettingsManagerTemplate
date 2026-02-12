#ifndef SPINBOXFACTORY_H
#define SPINBOXFACTORY_H

#include "settingscontrolfactory.h"
#include <QSpinBox>

class SpinBoxFactory : public SettingsControlFactory {
public:
    SpinBoxFactory(int defaultValue, int min, int max);
    QWidget* create() const override;
    QVariant defaultValue() const override;

private:
    int defaultValue_;
    int min_;
    int max_;
};

#endif // SPINBOXFACTORY_H