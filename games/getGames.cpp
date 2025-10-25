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

GamePlugin::GamePlugin(QString name, Version& version, QLibrary* library, InitFn playFn)
    : name(std::move(name)), vers(version), lib(library), playFn(std::move(playFn)) {}
GamePlugin::~GamePlugin() { delete lib; }
bool GamePlugin::run() {
    return playFn();
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
            Log::Error(MODULE) << "Failed to load" << fname << ":" << lib->errorString();
            failedGames.push_back({fname, lib->errorString()});
            delete lib;
            continue;
        }

        auto fail = [&](QString error) {
            Log::Error(MODULE) << error;
            lib->unload();
            failedGames.push_back({fname, error});
            delete lib;
        };

        auto versFn = reinterpret_cast<Version (*)()>(lib->resolve("version"));
        Version v;
        if (versFn) {
            v = versFn();
            if (v.to < VERSION or v.from > VERSION) {
                fail(QString("Incompatable version for %1! Requires versions %2-%3, this program is version %4!")
                    .arg(fname).arg(v.from).arg(v.to).arg(VERSION));
                continue;
            }
        } else {
            fail("No version function in "+fname);
            continue;
        }
        auto playGetter = reinterpret_cast<InitFn (*)()>(lib->resolve("reg"));
        if (playGetter) {
            InitFn play = playGetter();
            if (play) {
                Log::Info(MODULE) << "Loaded plugin:" << fname;
                games.emplace_back(fname, v, lib, play);
            } else {
                fail("Register function returned null init function in " + fname);
            }
        } else {
            fail("No register function in "+fname);
        }
    }
}

