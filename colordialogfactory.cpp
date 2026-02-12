#include "colordialogfactory.h"
#include <QHBoxLayout>
#include <QColor>

ColorDialogFactory::ColorDialogFactory(LineEditFactory* lineEdit, PushButtonFactory* pushButton)
    : lineEdit_(lineEdit), pushButton_(pushButton), lineEditWidget_(nullptr), pushButtonWidget_(nullptr) {}

ColorDialogFactory::~ColorDialogFactory() {
    delete lineEditWidget_;
    delete pushButtonWidget_;
}

QWidget* ColorDialogFactory::create() const {
    QWidget* container = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);

    lineEditWidget_ = qobject_cast<QLineEdit*>(lineEdit_->create());
    pushButtonWidget_ = qobject_cast<QPushButton*>(pushButton_->create());

    layout->addWidget(lineEditWidget_, 1);
    layout->addWidget(pushButtonWidget_);

    QObject::connect(pushButtonWidget_, &QPushButton::clicked, [this]() {
        QColorDialog dlg;
        dlg.setOption(QColorDialog::ShowAlphaChannel);
        dlg.setCurrentColor(QColor(lineEditWidget_->text()));
        if (dlg.exec() == QDialog::Accepted) {
            lineEditWidget_->setText(dlg.currentColor().name(QColor::HexArgb).toUpper());
        }
    });

    return container;
}

QVariant ColorDialogFactory::defaultValue() const {
    return lineEdit_->defaultValue();
}

QLineEdit* ColorDialogFactory::getLineEdit() const { return lineEditWidget_; }
QPushButton* ColorDialogFactory::getPushButton() const { return pushButtonWidget_; }