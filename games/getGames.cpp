#include "../main.hpp"
#include "../log.hpp"
#include "../menu.hpp"
#include "getGames.hpp"
#include <QStandardPaths>
#include <QDir>

const QString MODULE = "getGames";

QString getGamesPath() {
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(path);
    return path + "/gamePlugs";
}

std::vector<GamePlugin> games = {};
std::vector<std::pair<QString, QString>> failedGames = {};

GamePlugin::GamePlugin(QString nme, Registry& r, QLibrary* library)
    : name(std::move(nme)), reg(r), lib(library) { reg.loadFn(); }
GamePlugin::~GamePlugin() {
    reg.unloadFn();
    delete lib;
}
bool GamePlugin::run() {
    return reg.runFn();
}

void loadGames() {
    games.clear();
    failedGames.clear();

    QDir dir(getGamesPath());
#if defined(Q_OS_WIN)
    QStringList filters = {"*.dll"};
#elif defined(Q_OS_MAC)
    QStringList filters = {"*.dylib"};
#else
    QStringList filters = {"*.so"};
#endif
    QFileInfoList files = dir.entryInfoList(filters, QDir::Files);
    for (const QFileInfo& file : files) {
        auto* lib = new QLibrary(file.absoluteFilePath());
        QString fname = file.fileName();
        if (!lib->load()) {
            Log::Error(MODULE) << "Failed to load " << fname << ": " << lib->errorString();
            failedGames.push_back({fname, lib->errorString()});
            delete lib;
            continue;
        }

        auto regFn = reinterpret_cast<Registry (*)()>(lib->resolve("_register"));
        if (regFn) {
            Registry reg = regFn();
            games.emplace_back(fname, reg, lib);
            Log::Info(MODULE) << "Loaded plugin: " << fname;
        } else {
            QString error = "No '_register' function found in "+fname;
            Log::Error(MODULE) << error;
            lib->unload();
            failedGames.push_back({fname, error});
            delete lib;
        }
    }
}

