#pragma once
#include "../scenes/baseScn.hpp"
#include <QLibrary>

struct Version { int from; int to; };
using InitFn = bool (*)();

class GamePlugin {
public:
    GamePlugin(QString name, Version& vers, QLibrary* lib, InitFn playFn);
    ~GamePlugin();
    QString name;
    Version vers;
    QLibrary* lib;
    bool run();
private:
    InitFn playFn;
};

QString getGamesPath();
extern std::vector<GamePlugin> games;
extern std::vector<std::pair<QString, QString>> failedGames;
void loadGames();

