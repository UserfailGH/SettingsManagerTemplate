#include "checkboxfactory.h"
#include <QVariant>

CheckBoxFactory::CheckBoxFactory(const QVariant& defaultValue)
    : defaultValue_(defaultValue) {}

QWidget* CheckBoxFactory::create() const {
    QCheckBox* cb = new QCheckBox();
    cb->setChecked(defaultValue_.toBool());
    return cb;
}

QVariant CheckBoxFactory::defaultValue() const {
    return defaultValue_;
}