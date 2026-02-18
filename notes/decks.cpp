#include "getNotes.hpp"
#include "decks.hpp"
#include "../log.hpp"
#include <QFile>
#include <QLocale>
#include <QRegularExpression>

const QString MODULE = "decks";

std::vector<QString> decks;
QString curDeck;

bool checkValidDeck() {
    if (curDeck != "") return true;
    Log::Error(MODULE) << "Deck does not exist! Please rename this empty one or choose an existing one in the home screen.";
    return false;
}
int deckIdx() {
    auto it = std::find(decks.begin(), decks.end(), curDeck);
    if (it == decks.end()) { // Should never happen
        return 0;
    }
    return std::distance(decks.begin(), it);
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
    if (newname == "" || newname == ".") {
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
            Log::Error(MODULE) << "Could not find newly created deck in decks vector!";
            curDeck = "";
            return -2;
        }
        //Log::Debug(MODULE) << "Successfully created deck " << newname << "!";
        return -std::distance(decks.begin(), it) - 10;
    }

    auto it = std::find(decks.begin(), decks.end(), curDeck);
    if (it == decks.end()) {
        Log::Error(MODULE) << "Could not find deck in decks vector! You may need to refresh the app if you changed an external file.";
        return -1;
    }
    if (QFile::rename(pth+curDeck, pth+newname)) {
        int index = std::distance(decks.begin(), it);
        decks[index] = newname;
        curDeck = newname;
        return index;
    } else {
        Log::Error(MODULE) << "Deck name already exists or failed to write there!";
        return -1;
    }
}
int deleteDeck(bool ask) {
    if (ask && !doubleCheck("delete the deck '"+curDeck+"'")) return -1;
    QString pth = getPath()+"/"+curDeck;
    if (!QFile::remove(pth)) {
        Log::Error(MODULE) << "Unable to delete deck file!";
        return -1;
    }
    QString oldname = curDeck;
    curDeck = "";
    for (auto* n : notesL) delete n;
    notesL.clear();
    auto it = std::find(decks.begin(), decks.end(), oldname);
    if (it == decks.end()) {
        Log::Error(MODULE) << "Could not find deck in decks vector! You may need to refresh the app if you changed an external file.";
        return -1;
    }
    decks.erase(it);
    Log::Debug(MODULE) << "Successfully deleted deck " << oldname << "!";
    return std::distance(decks.begin(), it);
}

const QRegularExpression copyre(
    " the ([0-9,]+)(?:st|nd|rd|th)$"
);
QString int2word(uint num) {
    QString suf;
    switch (num%10) {
        case 1: suf = "st"; break;
        case 2: suf = "nd"; break;
        case 3: suf = "rd"; break;
        default: suf = "th"; break;
    }
    return QLocale(QLocale::English).toString(num) + suf;
}
bool copyDeck(DeckCopyType typ, bool tmp) {
    QString pth = getPath()+"/";
    QString frompth = pth+curDeck;
    uint idx;
    QString nam;
    auto match = copyre.match(curDeck);
    if (match.hasMatch()) {
        bool ok;
        uint num = match.captured(1).replace(",", "").toUInt(&ok);
        if (!ok) { // Should REALLY never happen
            Log::Warn(MODULE) << "Something very wrong happened with the copy deck!";
            idx = 1;
            nam = curDeck;
        } else {
            idx = num;
            nam = curDeck.slice(0, curDeck.length()-match.captured().length());
        }
    } else {
        idx = 1;
        nam = curDeck;
    }
    if (tmp) {
        if (!nam.startsWith('.')) nam = "."+nam;
        idx = 1;
    } else if (nam.startsWith('.')) {
        nam = nam.slice(1);
        idx = 1;
    }
    while (true) {
        idx += 1;
        QString end = nam + " the "+int2word(idx);
        auto it = std::find(decks.begin(), decks.end(), end);
        if (it == decks.end()) {
            if (!QFile::copy(frompth, pth+end)) {
                Log::Error(MODULE) << "Could not copy deck! You may need to refresh the app if you changed an external file.";
                return false;
            }
            curDeck = end;
            initNotes(); // Update list to be the order it would be created
            return true;
        }
    };
}
