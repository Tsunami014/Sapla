#include "../core.hpp"
#include "../log.hpp"
#include "../menu.hpp"
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
const QString disabl = "dis";

QString getGamesPath() {
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/gamePlugs";
    QDir().mkpath(path);
    return path;
}
QString getDisBIGPath() {
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(path);
    return path + "/BIDisabled";
}

bool tryLoad(const QFileInfo& file) {
    auto* lib = new QLibrary(file.absoluteFilePath());
    QString fname = file.fileName();
    if (!lib->load()) {
        Log::Error(MODULE) << "Failed to load " << fname << ": " << lib->errorString();
        failedGames.push_back({fname, lib->errorString(), file.filePath(), false});
        delete lib;
        return false;
    }

    auto regFn = reinterpret_cast<Registry (*)()>(lib->resolve("_register"));
    if (regFn) {
        Registry reg = regFn();
        games.push_back(new GamePlugin(fname, reg, lib, file.filePath()));
        return true;
    } else {
        QString error = "No '_register' function found in "+fname;
        Log::Error(MODULE) << error;
        lib->unload();
        failedGames.push_back({fname, error, file.filePath(), false});
        delete lib;
        return false;
    }
}

std::vector<GamePlugin*> games = {};
std::vector<FailedGame> failedGames = {};
std::vector<DisabldGame> disabldGames;

GamePlugin::GamePlugin(QString nme, Registry& r, QLibrary* library, QString pth)
    : name(std::move(nme)), reg(r), lib(library), path(pth) { reg.loadFn(); isBI = false; }
GamePlugin::~GamePlugin() {
    reg.unloadFn();
    lib->unload();
    delete lib;
}
bool GamePlugin::run() {
    return reg.runFn();
}

void clearGames() {
    for (int i = 1; i < games.size(); i++) delete games[i];
    games.clear();
    failedGames.clear();
    disabldGames.clear();
}

void loadGames() {
    clearGames();

    const QString biGameNam = "libbuiltinGame"+suffix;
    QString fpth = QCoreApplication::applicationDirPath()+"/"+biGameNam;
    if (!QFile::exists(fpth)) {
        Log::Warn(MODULE) << "Could not locate built-in game!";
    } else {
        if (QFile::exists(getDisBIGPath())) {
            disabldGames.push_back({biGameNam, fpth, true});
        } else {
            QFileInfo BIlib(fpth);
            if (tryLoad(BIlib)) {
                games.back()->isBI = true;
            } else {
                Log::Warn(MODULE) << "Error when loading built-in game!";
                failedGames.back().isBI = true;
            }
        }
    }

    QDir dir(getGamesPath());
    QFileInfoList files = dir.entryInfoList(QStringList{"*"+suffix, "*"+suffix+"."+disabl}, QDir::Files);
    for (const QFileInfo& file : files) {
        if (file.suffix() == disabl) {
            disabldGames.push_back({file.fileName(), file.filePath(), false});
        } else {
            tryLoad(file);
        }
    }

    Log::Info(MODULE) << "Loaded games:\n" << games.size() << " successfully\n"
                                           << failedGames.size() << " unsuccessfully\n"
                                           << disabldGames.size() << " disabled!";
}

