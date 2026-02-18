#include "getNotes.hpp"
#include "decks.hpp"
#include "defaultNotes.hpp"
#include "cardList.hpp"
#include "../core.hpp"
#include "../log.hpp"
#include <QStandardPaths>
#include <QDir>

const QString MODULE = "getNotes";

QString tryReadLine(QTextStream& in, QString error) {
    QString out;
    while (true) {
        out = in.readLine();
        if (out.isNull()) {
            if (error != "") {
                Log::Error(MODULE) << error;
            }
            return out;
        }
        if (out == "" || out[0] == "#") continue;
        break;
    }
    return out;
}

QString makeSafe(QString str) {
    if (str == "") return "\\";
    str.replace("\\", "\\\\");
    str.replace("\x03", "\\3");
    str.replace("\n", "\x03");
    str.replace("\r", "\\r");
    str.replace("---", "\\-");
    str.replace("===", "\\=");
    return str;
}
QString unSafe(QString str) {
    if (str == "\\") return "";
    str.replace("\x03", "\n");
    str.replace("\\r", "\r");
    str.replace("\\3", "\x03");
    str.replace("\\-", "---");
    str.replace("\\=", "===");
    str.replace("\\\\", "\\");
    return str;
}

QString getPath() {
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
        + "/decks";
    QDir d(path);
    if (!d.exists()) {
        d.mkpath(path);
        auto oldDeck = curDeck;
        curDeck = "default";
        loadDefaultNotes();
        curDeck = oldDeck;
    }
    return path;
}


void loadDefaultNotes() {
    for (auto* n : notesL) delete n;
    notesL.clear();
    for (auto& n : defNotes) {
        notesL.push_back(new Note(n));
    }
    updateNoteCards();
    writeNotes();
}

void clearTempNotes() {
    QDir d(getPath());
    uint cleared = 0;
    bool errored = false;
    for (const auto& item : d.entryList(QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot)) {
        if (item.startsWith('.')) {
            const QString pth = d.absoluteFilePath(item);
            if (!QFile::moveToTrash(pth)) {
                if (!QFile::remove(pth)) {
                    Log::Error(MODULE) << "Failed deleting temporary deck file!";
                    return;
                }
                if (!errored) {
                    errored = true;
                    Log::Warn(MODULE) << "Failed moving temporary deck file to trash, deleted permanently instead";
                }
                cleared += 1;
            } else { cleared += 1; }
        }
    }
    if (cleared != 0) {
        Log::Info(MODULE) << "Cleared " << cleared << " temporary decks!";
    }
}
void initNotes() {
    QDir d(getPath());
    decks.clear();
    decks.push_back("");
    for (const auto& item : d.entryList(QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot)) {
        decks.push_back(item);
    }
}

void updateNoteCards() {
    CLclear();
    for (auto* n : notesL) {
        n->updateCards();
    }
}

void writeNotes() {
    QString fullpth = getPath()+"/"+curDeck;
    QFile file(fullpth);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) { // This *should* never fail but it's still good to check
        Log::Error(MODULE) << "Failed writing to file at `" << fullpth << "`!";
        return;
    }
    QTextStream out(&file);
    for (auto* n : notesL) {
        out << makeSafe(n->contents()) << "\n";
    }
    file.close();
    //Log::Debug(MODULE) << "Successfully wrote " << cards.size() << " cards to the configuration file at:\n" << fullpth;
}

