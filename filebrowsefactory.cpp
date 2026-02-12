#include "filebrowsefactory.h"
#include <QHBoxLayout>
#include <QFileDialog>
#include <QDir>

FileBrowseFactory::FileBrowseFactory(LineEditFactory* lineEdit, PushButtonFactory* pushButton)
    : lineEdit_(lineEdit), pushButton_(pushButton), lineEditWidget_(nullptr), pushButtonWidget_(nullptr) {}

FileBrowseFactory::~FileBrowseFactory() {
    delete lineEditWidget_;
    delete pushButtonWidget_;
}

QWidget* FileBrowseFactory::create() const {
    QWidget* container = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);

    lineEditWidget_ = qobject_cast<QLineEdit*>(lineEdit_->create());
    pushButtonWidget_ = qobject_cast<QPushButton*>(pushButton_->create());

    layout->addWidget(lineEditWidget_, 1);
    layout->addWidget(pushButtonWidget_);

    QObject::connect(pushButtonWidget_, &QPushButton::clicked, [this]() {
        QString path = QFileDialog::getOpenFileName(nullptr, "Select File", QDir::homePath());
        if (!path.isEmpty()) {
            lineEditWidget_->setText(path);
        }
    });

    return container;
}

QVariant FileBrowseFactory::defaultValue() const {
    return lineEdit_->defaultValue();
}

QLineEdit* FileBrowseFactory::getLineEdit() const { return lineEditWidget_; }
QPushButton* FileBrowseFactory::getPushButton() const { return pushButtonWidget_; }