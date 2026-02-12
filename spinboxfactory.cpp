#include "spinboxfactory.h"
#include <algorithm>

SpinBoxFactory::SpinBoxFactory(int defaultValue, int min, int max)
    : defaultValue_(defaultValue), min_(min), max_(max) {}

QWidget* SpinBoxFactory::create() const {
    QSpinBox* sb = new QSpinBox();
    sb->setRange(std::min(min_, max_), std::max(min_, max_));
    sb->setValue(defaultValue_);
    return sb;
}

QVariant SpinBoxFactory::defaultValue() const {
    return defaultValue_;
}