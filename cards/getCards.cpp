#include "getCards.hpp"
#include "cardTyps.hpp"
#include <iostream>
#include <QStandardPaths>
#include <QDir>

std::vector<BaseCardTyp*> cards = {};

QString getPath() {
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(path);
    return path + "/savedata.txt";
}

void writeCards() {

}

void initCards() {
    for (auto* c : cards) delete c;
    cards = {};
    QString fullpth = getPath();
    QFile file(fullpth);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // File does not exist, so MAKE IT EXIST.
        QString fullpth = getPath();
        QFile file(fullpth);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) { // This should never fail (as it's writing to a new file if not exists), but we need to check every possibility
            qFatal() << "Failed writing to file at `" << fullpth << "`!";
        }
        QTextStream out(&file);
        out << "t\nWhat is Australia?\nA country\n"
               "t\nWhat is the meaning of life?\n42\n"
               "t\nWhat is 1 + 1?\n2\n"
               "t\nWhat is this?\nA flashcard app\n"
        ;
        file.close();
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qFatal() << "Failed reading to file at `" << fullpth << "`!";
        }
    }

    QTextStream in(&file);
    QString line = in.readLine();
    while (!line.isNull()) {
        BaseCardTyp* newC;
        if (line == "" || line[0] == "#") {
            // Nothing or comment
        } else {
            bool done = false;
            for (auto& typ : CardRegistry::registry) {
                if (typ.canParse(line)) {
                    cards.push_back(typ.parse(line, in));
                    done = true;
                    break;
                }
            }
            if (!done) qFatal() << "Card parser cannot be found for header: '" << line << "'!";
        }
        line = in.readLine();
    }

    if (cards.empty()) { qFatal() << "No flashcards present!"; }
    std::cout << "Successfully loaded configuration from `" << fullpth.toStdString() << "`, containing " << cards.size() << " cards.\n";
}

