#ifndef FILEBROWSEFACTORY_H
#define FILEBROWSEFACTORY_H

#include "settingscontrolfactory.h"
#include "lineeditfactory.h"
#include "pushbuttonfactory.h"
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

class FileBrowseFactory : public SettingsControlFactory {
public:
    FileBrowseFactory(LineEditFactory* lineEdit, PushButtonFactory* pushButton);
    ~FileBrowseFactory();
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

#endif // FILEBROWSEFACTORY_H