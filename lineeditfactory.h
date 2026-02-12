#ifndef LINEEDITFACTORY_H
#define LINEEDITFACTORY_H

#include "settingscontrolfactory.h"
#include <QtWidgets/QLineEdit>

class LineEditFactory : public SettingsControlFactory {
public:
    explicit LineEditFactory(const QString& defaultText = "");

    QWidget* create() const override;
    QVariant defaultValue() const override;

private:
    QString defaultText_;
};

#endif // LINEEDITFACTORY_H