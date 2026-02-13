#include "settingscontrolfactory.h"
#include "settingsitem.h"
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QComboBox>

QWidget* SettingsControlFactory::createControlWithReset(SettingsItem* item, QWidget* controlWidget)
{
    QWidget* wrapper = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(wrapper);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    layout->addWidget(controlWidget);

    QPushButton* resetBtn = new QPushButton("↺");
    resetBtn->setToolTip("Сбросить к значению по умолчанию");
    resetBtn->setFixedSize(20, 20);
    resetBtn->setStyleSheet("QPushButton { border: none; }");

    QObject::connect(resetBtn, &QPushButton::clicked, [item, controlWidget]() {
        QVariant defaultVal = item->factory()->defaultValue();

        if (auto* cb = qobject_cast<QCheckBox*>(controlWidget)) {
            cb->setChecked(defaultVal.toBool());
        } else if (auto* le = qobject_cast<QLineEdit*>(controlWidget)) {
            le->setText(defaultVal.toString());
        } else if (auto* sb = qobject_cast<QSpinBox*>(controlWidget)) {
            sb->setValue(defaultVal.toInt());
        } else if (auto* combo = qobject_cast<QComboBox*>(controlWidget)) {
            combo->setCurrentIndex(defaultVal.toInt());
        }
    });

    layout->addWidget(resetBtn);
    wrapper->setLayout(layout);
    return wrapper;
}