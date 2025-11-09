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

bool tryLoad(const QFileInfo& file) {
    auto* lib = new QLibrary(file.absoluteFilePath());
    QString fname = file.fileName();
    if (!lib->load()) {
        Log::Error(MODULE) << "Failed to load " << fname << ": " << lib->errorString();
        failedGames.push_back({fname, lib->errorString(), file.path()});
        delete lib;
        return false;
    }

    auto regFn = reinterpret_cast<Registry (*)()>(lib->resolve("_register"));
    if (regFn) {
        Registry reg = regFn();
        games.push_back(new GamePlugin(fname, reg, lib, file.path()));
        return true;
    } else {
        QString error = "No '_register' function found in "+fname;
        Log::Error(MODULE) << error;
        lib->unload();
        failedGames.push_back({fname, error, file.path()});
        delete lib;
        return false;
    }
}

std::vector<GamePlugin*> games = {};
std::vector<FailedGame> failedGames = {};
std::vector<std::pair<QString, QString>> disabldGames;

GamePlugin::GamePlugin(QString nme, Registry& r, QLibrary* library, QString pth)
    : name(std::move(nme)), reg(r), lib(library), path(pth) { reg.loadFn(); permanent = false; }
GamePlugin::~GamePlugin() {
    reg.unloadFn();
    //delete lib;
}
bool GamePlugin::run() {
    return reg.runFn();
}

void loadGames() {
    if (games.size() == 0) {
        const QString biGameNam = "libbuiltinGame"+suffix;
        QString fpth = QCoreApplication::applicationDirPath()+"/"+biGameNam;
        if (!QFile::exists(fpth)) {
            fpth = fpth+"."+disabl;
            if (!QFile::exists(fpth)) {
                Log::Warn(MODULE) << "Could not locate built-in game!";
            } else {
                disabldGames.push_back({biGameNam+"."+disabl, fpth});
            }
        } else {
            QFileInfo BIlib(fpth);
            if (tryLoad(BIlib)) {
                games[0]->permanent = true;
            } else {
                Log::Warn(MODULE) << "Error when loading built-in game!";
            }
        }
    } else {
        for (int i = 1; i < games.size(); i++) delete games[i];
        auto* g = games[0];
        games.clear();
        games.push_back(g);
    }
    failedGames.clear();
    disabldGames.clear();

    QDir dir(getGamesPath());
    QFileInfoList files = dir.entryInfoList(QStringList{"*"+suffix, "*"+suffix+"."+disabl}, QDir::Files);
    for (const QFileInfo& file : files) {
        if (file.suffix() == disabl) {
            disabldGames.push_back({file.fileName(), file.path()});
        } else {
            tryLoad(file);
        }
    }

    Log::Info(MODULE) << "Loaded " << games.size() << " game(s) successfully and " << failedGames.size() << " game(s) unsuccessfully!";
}

