#include "getCards.hpp"
#include <iostream>
#include <QStandardPaths>
#include <QDir>

std::vector<BaseCardTyp*> cards = {};

QString getPath() {
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(path);
    return path + "/savedata.txt";
}

BaseCardTyp* readTextCard(QString line, QTextStream& in) {
    QString front = in.readLine();
    if (front.isNull()) { qFatal() << "Text item has no contents!"; }
    QString back = in.readLine();
    if (front.isNull()) { qFatal() << "Text item has no back!"; }
    return new TextCard(front, back);
}

void initCards() {
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
        } else if (line[0] == "t") {
            newC = readTextCard(line, in);
        } else { qFatal() << "Unknown card type '" << line << "'!"; }
        cards.push_back(newC);
        line = in.readLine();
    }

    if (cards.empty()) { qFatal() << "No flashcards present!"; }
    std::cout << "Successfully loaded configuration from `" << fullpth.toStdString() << "`, containing " << cards.size() << " cards.\n";
}

