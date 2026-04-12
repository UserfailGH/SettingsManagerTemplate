#include "spinboxfactory.h"
#include <QSpinBox>
#include <algorithm>

SpinBoxFactory::SpinBoxFactory(int min, int max)
    : min_(min), max_(max) {}

QWidget* SpinBoxFactory::create() const {
    QSpinBox* sb = new QSpinBox();
    sb->setRange(std::min(min_, max_), std::max(min_, max_));
    return sb;
}