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

void initNotes() {
    QDir d(getPath());
    decks.clear();
    decks.push_back("");
    for (const auto& item : d.entryList()) {
        if (!item.startsWith('.')) decks.push_back(item);
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

