#ifndef SETTINGSCACHE_H
#define SETTINGSCACHE_H

#include <QObject>
#include <QMap>
#include <QVariant>
#include <QReadWriteLock>

class SettingsCache : public QObject
{
    Q_OBJECT

public:
    static SettingsCache& instance();

    void setValue(const QString& group, const QString& key, const QVariant& value);
    QVariant getValue(const QString& group, const QString& key, const QVariant& defaultValue = QVariant()) const;
    bool contains(const QString& group, const QString& key) const;
    void remove(const QString& group, const QString& key);
    void clear();
    void clearGroup(const QString& group);

    // Методы для массовой загрузки/сохранения
    void loadFromSettings();
    void saveToSettings();

private:
    SettingsCache(QObject* parent = nullptr);
    ~SettingsCache() = default;

    SettingsCache(const SettingsCache&) = delete;
    SettingsCache& operator=(const SettingsCache&) = delete;

    QMap<QString, QMap<QString, QVariant>> cache; // group -> (key -> value)
    mutable QReadWriteLock lock;
};

#endif // SETTINGSCACHE_H
