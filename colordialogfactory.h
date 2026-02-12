#ifndef COLORDIALOGFACTORY_H
#define COLORDIALOGFACTORY_H

#include "settingscontrolfactory.h"
#include "lineeditfactory.h"
#include "pushbuttonfactory.h"
#include <QLineEdit>
#include <QPushButton>
#include <QColorDialog>

class ColorDialogFactory : public SettingsControlFactory {
public:
    ColorDialogFactory(LineEditFactory* lineEdit, PushButtonFactory* pushButton);
    ~ColorDialogFactory();
    QWidget* create() const override;
    QVariant defaultValue() const override;

    QLineEdit* getLineEdit() const;
    QPushButton* getPushButton() const;

private:
    LineEditFactory* lineEdit_;
    PushButtonFactory* pushButton_;
    mutable QLineEdit* lineEditWidget_;
    mutable QPushButton* pushButtonWidget_;
};

#endif // COLORDIALOGFACTORY_H