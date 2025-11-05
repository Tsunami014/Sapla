#include "../core.hpp"
#include "../log.hpp"
#include "../menu.hpp"
#include "builtin/game.hpp"
#include "getGames.hpp"
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDir>

const QString MODULE = "getGames";

#if defined(Q_OS_WIN)
const QString suffix = ".dll";
#elif defined(Q_OS_MAC)
const QString suffix = ".dylib";
#else
const QString suffix = ".so";
#endif

QString getGamesPath() {
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/gamePlugs";
    QDir().mkpath(path);
    return path;
}

bool tryLoad(const QFileInfo& file) {
    auto* lib = new QLibrary(file.absoluteFilePath());
    QString fname = file.fileName();
    if (!lib->load()) {
        Log::Error(MODULE) << "Failed to load " << fname << ": " << lib->errorString();
        failedGames.push_back({fname, lib->errorString()});
        delete lib;
        return false;
    }

    auto regFn = reinterpret_cast<Registry (*)()>(lib->resolve("_register"));
    if (regFn) {
        Registry reg = regFn();
        games.push_back(new GamePlugin(fname, reg, lib));
        return true;
    } else {
        QString error = "No '_register' function found in "+fname;
        Log::Error(MODULE) << error;
        lib->unload();
        failedGames.push_back({fname, error});
        delete lib;
        return false;
    }
}

std::vector<GamePlugin*> games = {};
std::vector<std::pair<QString, QString>> failedGames = {};

GamePlugin::GamePlugin(QString nme, Registry& r, QLibrary* library)
    : name(std::move(nme)), reg(r), lib(library) { reg.loadFn(); permanent = false; }
GamePlugin::~GamePlugin() {
    reg.unloadFn();
    //delete lib;
}
bool GamePlugin::run() {
    return reg.runFn();
}

void loadGames() {
    if (games.size() == 0) {
        QFileInfo BIlib(QCoreApplication::applicationDirPath()+"/libbuiltinGame"+suffix);
        if (tryLoad(BIlib)) {
            games[0]->permanent = true;
        } else {
            Log::Warn(MODULE) << "Could not load built-in game!";
        }
    } else {
        for (int i = 1; i < games.size(); i++) delete games[i];
        auto* g = games[0];
        games.clear();
        games.push_back(g);
    }
    failedGames.clear();

    QDir dir(getGamesPath());
    QFileInfoList files = dir.entryInfoList(QStringList{"*"+suffix}, QDir::Files);
    for (const QFileInfo& file : files) {
        tryLoad(file);
    }

    Log::Info(MODULE) << "Loaded " << games.size() << " game(s) successfully and " << failedGames.size() << " game(s) unsuccessfully!";
}

