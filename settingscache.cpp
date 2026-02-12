#include "settingscache.h"
#include <QSettings>

SettingsCache& SettingsCache::instance() {
    static SettingsCache instance;
    return instance;
}

SettingsCache::SettingsCache(QObject* parent)
    : QObject(parent)
{
}

void SettingsCache::setValue(const QString& group, const QString& key, const QVariant& value) {
    QWriteLocker locker(&lock);
    cache[group][key] = value;
}

QVariant SettingsCache::getValue(const QString& group, const QString& key, const QVariant& defaultValue) const {
    QReadLocker locker(&lock);
    auto groupIt = cache.find(group);
    if (groupIt != cache.end()) {
        auto keyIt = groupIt->find(key);
        if (keyIt != groupIt->end()) {
            return *keyIt;
        }
    }
    return defaultValue;
}

bool SettingsCache::contains(const QString& group, const QString& key) const {
    QReadLocker locker(&lock);
    auto groupIt = cache.find(group);
    if (groupIt != cache.end()) {
        return groupIt->contains(key);
    }
    return false;
}

void SettingsCache::remove(const QString& group, const QString& key) {
    QWriteLocker locker(&lock);
    auto groupIt = cache.find(group);
    if (groupIt != cache.end()) {
        groupIt->remove(key);
        if (groupIt->isEmpty()) {
            cache.erase(groupIt);
        }
    }
}

void SettingsCache::clear() {
    QWriteLocker locker(&lock);
    cache.clear();
}

void SettingsCache::clearGroup(const QString& group) {
    QWriteLocker locker(&lock);
    cache.remove(group);
}

void SettingsCache::loadFromSettings() {
    QWriteLocker locker(&lock);
    cache.clear();

    QSettings settings;
    QStringList groups = settings.childGroups();

    for (const QString& group : groups) {
        settings.beginGroup(group);
        QStringList keys = settings.childKeys();
        for (const QString& key : keys) {
            cache[group][key] = settings.value(key);
        }
        settings.endGroup();
    }
}

void SettingsCache::saveToSettings() {
    QReadLocker locker(&lock);
    QSettings settings;

    for (auto groupIt = cache.begin(); groupIt != cache.end(); ++groupIt) {
        settings.beginGroup(groupIt.key());
        for (auto keyIt = groupIt->begin(); keyIt != groupIt->end(); ++keyIt) {
            settings.setValue(keyIt.key(), keyIt.value());
        }
        settings.endGroup();
    }
    settings.sync();
}