#include "settingscontrolfactory.h"
#include "settingsitem.h"
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QComboBox>
#include <QMessageBox>
#include <QDebug>

QWidget* SettingsControlFactory::createControlWithReset(SettingsItem* item, QWidget* controlWidget)
{
    QWidget* wrapper = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(wrapper);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    layout->addWidget(controlWidget);

    QPushButton* resetBtn = new QPushButton("↺");
    resetBtn->setToolTip("Reset to default value");
    resetBtn->setFixedSize(20, 20);
    resetBtn->setStyleSheet("QPushButton { border: none; }");

    QObject::connect(resetBtn, &QPushButton::clicked, [wrapper, item]() mutable {
        QMessageBox::StandardButton reply = QMessageBox::question(
            nullptr,
            "Reset Setting",
            QString("Reset setting \"%1\" to its default value?").arg(item->name()),
            QMessageBox::Yes | QMessageBox::No
        );

        if (reply != QMessageBox::Yes) {
            qDebug() << "Reset cancelled by user";
            return;
        }

        QVariant defaultVal = item->defaultValue();

        if (auto* cb = wrapper->findChild<QCheckBox*>()) {
            cb->setChecked(defaultVal.toBool());
            qDebug() << "Checkbox reset completed";
            return;
        }

        if (auto* le = wrapper->findChild<QLineEdit*>()) {
            le->setText(defaultVal.toString());
            qDebug() << "Line edit reset completed";
            return;
        }

        if (auto* sb = wrapper->findChild<QSpinBox*>()) {
            sb->setValue(defaultVal.toInt());
            qDebug() << "Spinbox reset completed";
            return;
        }

        if (auto* combo = wrapper->findChild<QComboBox*>()) {
            combo->setCurrentIndex(defaultVal.toInt());
            qDebug() << "Combobox reset completed";
            return;
        }

        qWarning() << "No supported widget found for reset inside wrapper. "
                   << "Wrapper type:" << wrapper->metaObject()->className();
    });

    layout->addWidget(resetBtn);
    wrapper->setLayout(layout);
    return wrapper;
}