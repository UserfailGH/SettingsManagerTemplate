#include "comboboxfactory.h"

ComboBoxFactory::ComboBoxFactory(const QStringList& items, int defaultIndex)
    : items_(items), defaultIndex_(defaultIndex)
{
    if (defaultIndex < 0 || defaultIndex >= items.size()) {
        defaultIndex_ = 0;
    }
}

ComboBoxFactory::ComboBoxFactory(const QString& defaultValue, const QStringList& items)
    : items_(items), defaultIndex_(items.indexOf(defaultValue))
{
    if (defaultIndex_ == -1) {
        defaultIndex_ = 0;
    }
}

QWidget* ComboBoxFactory::create() const {
    QComboBox* combo = new QComboBox();
    combo->addItems(items_);
    combo->setCurrentIndex(defaultIndex_);
    return combo;
}

QVariant ComboBoxFactory::defaultValue() const {
    return defaultIndex_;
}