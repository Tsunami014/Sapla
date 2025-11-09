#pragma once
#include "link.hpp"
#include "../scenes/baseScn.hpp"
#include <QLibrary>

const extern QString suffix;
QString getGamesPath();
QString getDisBIGPath();

class GamePlugin {
public:
    GamePlugin(QString name, Registry& reg, QLibrary* lib, QString pth);
    ~GamePlugin();
    QString name;
    QString path;
    bool run();
    bool isBI;
private:
    Registry reg;
    QLibrary* lib;
};

struct FailedGame {
    QString name;
    QString error;
    QString path;
    bool isBI;
};
struct DisabldGame {
    QString name;
    QString path;
    bool isBI;
};

extern std::vector<GamePlugin*> games;
extern std::vector<FailedGame> failedGames;
extern std::vector<DisabldGame> disabldGames;
void clearGames();
void loadGames();

