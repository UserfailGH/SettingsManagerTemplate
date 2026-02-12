#ifndef PUSHBUTTONFACTORY_H
#define PUSHBUTTONFACTORY_H

#include "settingscontrolfactory.h"
#include <QPushButton>
#include <QString>

class PushButtonFactory : public SettingsControlFactory {
public:
    explicit PushButtonFactory(const QString& buttonText);
    QWidget* create() const override;
    QVariant defaultValue() const override;

private:
    QString buttonText_;
};

#endif // PUSHBUTTONFACTORY_H