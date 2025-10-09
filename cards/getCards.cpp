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
    QString fullpth = getPath();
    QFile file(fullpth);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) { // This *should* never fail but it's still good to check
        qCritical() << "Failed writing to file at `" << fullpth << "`!";
        return;
    }
    QTextStream out(&file);
    for (auto& c : cards) {
        c->toFile(out);
    }
    file.close();
    qDebug() << "Successfully wrote" << cards.size() << "cards to the configuration file at:\n" << fullpth;
    //std::cout << "Successfully wrote " << cards.size() << " cards to the configuration file at:\n" << fullpth.toStdString() << "\n";
}

void initCards() {
    for (auto* c : cards) delete c;
    cards = {};
    QString fullpth = getPath();
    QFile file(fullpth);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // File does not exist, so MAKE IT EXIST.
        cards = {
            new TextCard("What is Australia?", "A country"),
            new TextCard("What is the meaning of life?", "42"),
            new TextCard("What is 1 + 1?", "2"),
            new TextCard("What is this?", "A flashcard app")
        };
        writeCards();
        return;
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

    file.close();
    std::cout << "Successfully loaded " << cards.size() << " cards from the configuration file at:\n" << fullpth.toStdString() << "\n";
}

