#include "settings.hpp"
#include "log.hpp"
#include "notes/getNotes.hpp"
#include "notes/decks.hpp"
#include <QMap>
#include <QStandardPaths>
#include <QDir>

const QString MODULE = "settings";

QMap<QString, QString>StrGlobalSettings;
QMap<QString, QString>StrLocalSettings;
QMap<QString, float>NumGlobalSettings;
QMap<QString, float>NumLocalSettings;

std::vector<Note*> settingss;

QString sanitise(const QString val) {
    QString val2 = val;
    return val2.replace('\x04', "\x04-").replace(' ', "\x04=");
}
QString unsanitise(const QString val) {
    QString val2 = val;
    return val2.replace("\x04=", " ").replace("\x04-", "\x04");
}

QString getSettingsPath() {
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir d(path);
    if (!d.exists()) {
        d.mkpath(path);
    }
    return path + "/settings";
}

void loadSettingLine(QString str, bool global) {
    if (str == "" || str[0] == "#") {
        return;
    }
    QStringList spl = str.split(' ');
    if (spl[0] == "STR") {
        if (global) StrGlobalSettings[spl[1]] = unsanitise(spl[2]);
        else StrLocalSettings[spl[1]] = unsanitise(spl[2]);
    } else if (spl[0] == "NUM") {
        bool ok;
        float num = spl[2].toFloat(&ok);
        if (!ok) {
        const QString errorstr = global ? "global" : "local";
            Log::Warn(MODULE) <<
                "Ignoring unknown " << errorstr << " number setting '" << spl[1] << "' which is not a number: '" << spl[2] << "' (will be deleted next time the settings are saved)";
        } else {
            if (global) NumGlobalSettings[spl[1]] = num;
            else NumLocalSettings[spl[1]] = num;
        }
    } else {
        const QString errorstr = global ? "global" : "local";
        Log::Warn(MODULE) <<
            "Ignoring unknown " << errorstr << " setting: '" << str << "' (will be deleted next time the settings are saved)";
    }
}

void initSettings() {
    QString pth = getSettingsPath();
    QFile file(pth);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // File does not exist, don't worry about it
        Log::Debug(MODULE) << "No global settings to load!";
        return;
    }

    QTextStream in(&file);
    QString line = in.readLine();
    while (!line.isNull()) {
        loadSettingLine(line, true);
        line = in.readLine();
    }

    file.close();
    Log::Debug(MODULE) << "Loaded global settings!";
}
static QString lastDeck = "";
void initDeckSettings() {
    if (curDeck == lastDeck) return;
    lastDeck = curDeck;
    StrLocalSettings.clear();
    NumLocalSettings.clear();
    settingss.clear();
    if (curDeck == "") return;
    // Read settings from a note tagged 'settings' and error if multiple
    std::copy_if(notesL.begin(), notesL.end(), std::back_inserter(settingss), []
        (Note* n){return n->tags.contains("settings");} );
    size_t settingsss = settingss.size();
    if (settingsss > 1) {
        Log::Error(MODULE) << "Found more than 1 settings note, found " << settingsss << "! (tagged 'settings')";
    } else if (settingsss == 1) {
        QStringList lines = settingss[0]->contents().split('\n');
        for (const QString str : lines) {
            loadSettingLine(str, false);
        }
    }
}

QString getStrSetting(QString key, bool global, QString deflt) {
    if (global) {
        return StrGlobalSettings.value(key, deflt);
    }
    return StrLocalSettings.value(key, deflt);
}
float getNumSetting(QString key, bool global, float deflt) {
    if (global) {
        return NumGlobalSettings.value(key, deflt);
    }
    return NumLocalSettings.value(key, deflt);
}

void writeGlobalSettings() {
    QString pth = getSettingsPath();
    QFile file(pth);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) { // This *should* never fail but it's still good to check
        Log::Error(MODULE) << "Failed writing to file at `" << pth << "`!";
        return;
    }
    QTextStream out(&file);
    for (auto [key, value] : StrGlobalSettings.asKeyValueRange()) {
        out << QString("\nSTR %1 %2")
            .arg(sanitise(key)).arg(sanitise(value))
            << "\n";
    }
    for (auto [key, value] : NumGlobalSettings.asKeyValueRange()) {
        out << QString("\nNUM %1 %2")
            .arg(sanitise(key)).arg(sanitise(QString::number(value)))
            << "\n";
    }
    file.close();
}
void writeLocalSettings() {
    if (curDeck == "") {
        Log::Error(MODULE) << "Can't save any deck settings, no deck selected!";
        return;
    }
    size_t settingsss = settingss.size();
    if (settingsss > 2) {
        Log::Error(MODULE) << "Can't save any settings, found more than 1 settings note; found " << settingsss << "! (Tagged 'settings')";
        return;
    } else if (settingsss == 0) {
        auto* n = new Note("");
        notesL.push_back(n);
        settingss.push_back(n);
    }
    auto* n = settingss[0];
    QString conts = "#settings";
    for (auto [key, value] : StrLocalSettings.asKeyValueRange()) {
        conts += QString("\nSTR %1 %2")
            .arg(sanitise(key)).arg(sanitise(value));
    }
    for (auto [key, value] : NumLocalSettings.asKeyValueRange()) {
        conts += QString("\nNUM %1 %2")
            .arg(sanitise(key)).arg(sanitise(QString::number(value)));
    }
    n->setContents(conts);
    n->updateCards();
    writeNotes();
}

void setStrSetting(QString key, QString value, bool global) {
    if (global) {
        StrGlobalSettings[key] = value;
        writeGlobalSettings();
    } else {
        StrLocalSettings[key] = value;
        writeLocalSettings();
    }
}
void setNumSetting(QString key, float value, bool global) {
    if (global) {
        NumGlobalSettings[key] = value;
        writeGlobalSettings();
    } else {
        NumLocalSettings[key] = value;
        writeLocalSettings();
    }
}
