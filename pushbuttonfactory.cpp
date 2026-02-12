#include "pushbuttonfactory.h"

PushButtonFactory::PushButtonFactory(const QString& buttonText)
    : buttonText_(buttonText) {}

QWidget* PushButtonFactory::create() const {
    QPushButton* btn = new QPushButton(buttonText_);
    return btn;
}

QVariant PushButtonFactory::defaultValue() const {
    return buttonText_;
}