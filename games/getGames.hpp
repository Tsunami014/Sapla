#pragma once
#include "link.hpp"
#include "../scenes/baseScn.hpp"
#include <QLibrary>

class GamePlugin {
public:
    GamePlugin(QString name, Registry& reg, QLibrary* lib);
    ~GamePlugin();
    QString name;
    bool run();
private:
    Registry reg;
    QLibrary* lib;
};

QString getGamesPath();
extern std::vector<GamePlugin> games;
extern std::vector<std::pair<QString, QString>> failedGames;
void loadGames();

