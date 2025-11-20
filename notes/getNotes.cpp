#include "getNotes.hpp"
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
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(path);
    return path + "/savedata.txt";
}

void updateNoteCards() {
    for (auto* n : notesL) {
        n->updateCards();
    }
}

void writeNotes() {
    QString fullpth = getPath();
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
    MG->removeGame();
    //Log::Debug(MODULE) << "Successfully wrote " << cards.size() << " cards to the configuration file at:\n" << fullpth;
}

void initNotes() {
    QString fullpth = getPath();
    QFile file(fullpth);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // File does not exist, so MAKE IT EXIST.
        notesL = {
            new Note("What is **Australia**?\n"
                 "    *  (This is important to know)\n"
                 "---\n"
                 "A country"),
            new Note("What *is* the meaning of life?\n"
                 "---\n"
                 "42"),
            new Note("What is 1 + 1?\n"
                 "---\n"
                 "- 2\n"
                 "+ You should know this!"),
            new Note("[[What happened i:I]]n 2025[[?:]]\n"
                 "===\n"
                 "[[When was t:T]]his app was made[[?:]]")
        };
        updateNoteCards();
        writeNotes();
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
    Log::Debug(MODULE) << "Successfully loaded " << notesL.size() << " cards!";
}

