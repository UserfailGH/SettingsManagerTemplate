#include "lineeditfactory.h"

LineEditFactory::LineEditFactory(const QString& defaultText)
    : defaultText_(defaultText) {}

QWidget* LineEditFactory::create() const {
    QLineEdit* le = new QLineEdit();
    le->setText(defaultText_);
    return le;
}