#include "settingscontrolfactory.h"
#include "settingsitem.h"
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QComboBox>
#include <QDebug>

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

    QObject::connect(resetBtn, &QPushButton::clicked, [wrapper, item]() mutable {
        QVariant defaultVal = item->factory()->defaultValue();

        if (auto* cb = wrapper->findChild<QCheckBox*>()) {
            cb->setChecked(defaultVal.toBool());
            qDebug() << "Сброс чекбокса выполнен";
            return;
        }

        if (auto* le = wrapper->findChild<QLineEdit*>()) {
            le->setText(defaultVal.toString());
            qDebug() << "Сброс line edit выполнен";
            return;
        }

        if (auto* sb = wrapper->findChild<QSpinBox*>()) {
            sb->setValue(defaultVal.toInt());
            qDebug() << "Сброс spinbox выполнен";
            return;
        }

        if (auto* combo = wrapper->findChild<QComboBox*>()) {
            combo->setCurrentIndex(defaultVal.toInt());
            qDebug() << "Сброс комбобокса выполнен";
            return;
        }

        qWarning() << "Не найден ни один поддерживаемый виджет для сброса внутри обёртки. "
                   << "Тип обёртки:" << wrapper->metaObject()->className();
    });

    layout->addWidget(resetBtn);
    wrapper->setLayout(layout);
    return wrapper;
}