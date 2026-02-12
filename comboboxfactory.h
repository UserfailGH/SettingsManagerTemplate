#ifndef COMBOBOXFACTORY_H
#define COMBOBOXFACTORY_H

#include "settingscontrolfactory.h"
#include <QtWidgets/QComboBox>
#include <QStringList>

class ComboBoxFactory : public SettingsControlFactory {
public:
    explicit ComboBoxFactory(const QStringList& items, int defaultIndex = 0);

    explicit ComboBoxFactory(const QString& defaultValue, const QStringList& items);

    QWidget* create() const override;
    QVariant defaultValue() const override;

private:
    QStringList items_;
    int defaultIndex_;
};

#endif // COMBOBOXFACTORY_H