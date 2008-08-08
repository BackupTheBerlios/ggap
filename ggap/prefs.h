#ifndef GGAP_PREFS_H
#define GGAP_PREFS_H

#include <QVariant>
#include <QStringList>
#include <QFont>

namespace ggap {

enum SettingsType {
    SettingsPrefs,
    SettingsState
};

class Prefs {
    SettingsType type;

public:
    Prefs(SettingsType type) : type(type) {}

    QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;
    void setValue(const QString &key, const QVariant &value, const QVariant &defaultValue = QVariant());
    void unsetValue(const QString &key);

    enum BoolKey {
        SaveWorkspace,
        UseDefaultGap
    };

    bool value(BoolKey key);
    void setValue(BoolKey key, bool value);

    enum StringKey {
        GapRootDir,
        GapExe,
        GapArgs,
        HelpHomePage,
        DefaultFileFormat
    };

    QString value(StringKey key);
    void setValue(StringKey key, const QString &value);

    enum StringListKey {
        HelpBookmarks,
        ExtraGapRoots
    };

    QStringList value(StringListKey key);
    void setValue(StringListKey key, const QStringList &value);

    enum ByteArrayKey {
        PageSetup
    };

    QByteArray value(ByteArrayKey key);
    void setValue(ByteArrayKey key, const QByteArray &value);

    enum FontKey {
        WorksheetFont,
        HelpFont
    };

    QFont value(FontKey key);
    void setValue(FontKey key, const QFont &value);

    enum IntKey {
        CompressionLevel
    };

    int value(IntKey key);
    void setValue(IntKey key, int value);
};

inline bool prefsValue(Prefs::BoolKey k)
{
    return Prefs(SettingsPrefs).value(k);
}

inline QString prefsValue(Prefs::StringKey k)
{
    return Prefs(SettingsPrefs).value(k);
}

inline QStringList prefsValue(Prefs::StringListKey k)
{
    return Prefs(SettingsPrefs).value(k);
}

inline int prefsValue(Prefs::IntKey k)
{
    return Prefs(SettingsPrefs).value(k);
}

inline QByteArray prefsValue(Prefs::ByteArrayKey k)
{
    return Prefs(SettingsPrefs).value(k);
}

inline QFont prefsValue(Prefs::FontKey k)
{
    return Prefs(SettingsPrefs).value(k);
}

inline QVariant prefsValue(const QString &key, const QVariant &dflt = QVariant())
{
    return Prefs(SettingsPrefs).value(key, dflt);
}

inline QVariant stateValue(const QString &key, const QVariant &dflt = QVariant())
{
    return Prefs(SettingsState).value(key, dflt);
}

template<typename K, typename V>
inline void setPrefsValue(const K &key, const V &value)
{
    Prefs(SettingsPrefs).setValue(key, value);
}

template<typename K, typename V>
inline void setStateValue(const K &key, const V &value)
{
    Prefs(SettingsState).setValue(key, value);
}

template<typename K>
inline void unsetPrefsValue(const K &key)
{
    Prefs(SettingsPrefs).unsetValue(key);
}

template<typename K>
inline void unsetStateValue(const K &key)
{
    Prefs(SettingsState).unsetValue(key);
}

}

#endif
