#pragma once
#include "link.hpp"
#include "../scenes/baseScn.hpp"
#include <QLibrary>

const extern QString suffix;
QString getGamesPath();

class GamePlugin {
public:
    GamePlugin(QString name, Registry& reg, QLibrary* lib, QString pth);
    ~GamePlugin();
    QString name;
    QString path;
    bool run();
    bool permanent;
private:
    Registry reg;
    QLibrary* lib;
};

struct FailedGame {
    QString name;
    QString error;
    QString path;
};

extern std::vector<GamePlugin*> games;
extern std::vector<FailedGame> failedGames;
extern std::vector<std::pair<QString, QString>> disabldGames;
void loadGames();

