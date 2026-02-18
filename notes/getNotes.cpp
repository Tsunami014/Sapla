#include "getNotes.hpp"
#include "defaultNotes.hpp"
#include "cardList.hpp"
#include "../core.hpp"
#include "../log.hpp"
#include <QStandardPaths>
#include <QDir>

const QString MODULE = "getNotes";

std::vector<QString> decks;
QString curDeck;

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

bool checkValidDeck() {
    if (curDeck != "") return true;
    Log::Warn(MODULE) << "Deck does not exist! Please rename this empty one or choose an existing one in the home screen.";
    return false;
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

void changeDeck(QString newname) {
    for (auto* n : notesL) delete n;
    notesL.clear();

    curDeck = newname;
    if (curDeck == "") return;
    QString fullpth = getPath()+"/"+newname;
    QFile file(fullpth);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // File does not exist, so MAKE IT EXIST
        writeNotes();
        Log::Debug(MODULE) << "Successfully created deck " << newname << "!";
        return;
    }

    QTextStream in(&file);
    QString line = in.readLine();
    while (!line.isNull()) {
        if (line == "" || line[0] == "#") {
            // Nothing or comment
        } else {
            notesL.push_back(new Note(unSafe(line)));
        }
        line = in.readLine();
    }

    file.close();
    updateNoteCards();
    Log::Debug(MODULE) << "Successfully loaded " << notesL.size() << " notes from deck " << newname << "!";
}
int renameDeck(QString newname) {
    if (newname == "") {
        //Log::Warn(MODULE) << "Deck name cannot be empty!";
        return -2;
    }
    if (newname == curDeck) return -2;

    QString pth = getPath()+"/";
    if (curDeck == "") {
        if (QFile::exists(pth+newname)) {
            //Log::Warn(MODULE) << "Deck name already exists! Deck not created.";
            return -1;
        }
        curDeck = newname;
        writeNotes();
        initNotes(); // Update list to be the order it would be created
        auto it = std::find(decks.begin(), decks.end(), curDeck);
        if (it == decks.end()) { // Should never happen but you never know...
            Log::Warn(MODULE) << "Could not find newly created deck in decks vector!";
            curDeck = "";
            return -2;
        }
        //Log::Debug(MODULE) << "Successfully created deck " << newname << "!";
        return -std::distance(decks.begin(), it) - 10;
    }

    auto it = std::find(decks.begin(), decks.end(), curDeck);
    if (it == decks.end()) { // Again, should never happen.
        Log::Warn(MODULE) << "Could not find deck in decks vector!";
        return -1;
    }
    if (QFile::rename(pth+curDeck, pth+newname)) {
        int index = std::distance(decks.begin(), it);
        decks[index] = newname;
        curDeck = newname;
        return index;
    } else {
        Log::Warn(MODULE) << "Deck name already exists or failed to write there!";
        return -1;
    }
}
int deleteDeck() {
    if (!doubleCheck("delete the deck '"+curDeck+"'")) return -1;
    QString pth = getPath()+"/"+curDeck;
    if (!QFile::remove(pth)) {
        Log::Warn(MODULE) << "Unable to delete deck file!";
        return -1;
    }
    QString oldname = curDeck;
    curDeck = "";
    for (auto* n : notesL) delete n;
    notesL.clear();
    auto it = std::find(decks.begin(), decks.end(), oldname);
    if (it == decks.end()) { // Should never happen
        Log::Warn(MODULE) << "Could not find deck in decks vector!";
        return -1;
    }
    decks.erase(it);
    Log::Debug(MODULE) << "Successfully deleted deck " << oldname << "!";
    return std::distance(decks.begin(), it);
}
int deckIdx() {
    auto it = std::find(decks.begin(), decks.end(), curDeck);
    if (it == decks.end()) { // Should never happen
        return 0;
    }
    return std::distance(decks.begin(), it);
}
