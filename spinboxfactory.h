#ifndef SPINBOXFACTORY_H
#define SPINBOXFACTORY_H

#include "settingscontrolfactory.h"

class SpinBoxFactory : public SettingsControlFactory {
public:
    SpinBoxFactory(int min, int max);
    QWidget* create() const override;

private:
    int min_;
    int max_;
};

#endif