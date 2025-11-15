#include "../core.hpp"
#include "../log.hpp"
#include "../menu.hpp"
#include "../base/svgRend.hpp"
#include "getPlugins.hpp"
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDir>

const QString MODULE = "getPlugins";

#if defined(Q_OS_WIN)
const QString suffix = ".dll";
#elif defined(Q_OS_MAC)
const QString suffix = ".dylib";
#else
const QString suffix = ".so";
#endif
const QString disabl = "dis";

QString getPlugsPath() {
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/plugs";
    QDir().mkpath(path);
    return path;
}
QString getBIDisPath() {
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(path);
    return path + "/BIDisabled";
}

bool tryLoad(const QFileInfo& file) {
    auto* lib = new QLibrary(file.absoluteFilePath());
    QString fname = file.fileName();
    if (!lib->load()) {
        Log::Error(MODULE) << "Failed to load " << fname << ": " << lib->errorString();
        failedPlugs.push_back({fname, lib->errorString(), file.filePath(), false});
        delete lib;
        return false;
    }

    auto regFn = reinterpret_cast<Registry (*)()>(lib->resolve("_register"));
    if (regFn) {
        Registry reg = regFn();
        plugs.push_back(new Plugin(fname, reg, lib, file.filePath()));
        return true;
    } else {
        QString error = "No '_register' function found in "+fname;
        Log::Error(MODULE) << error;
        lib->unload();
        failedPlugs.push_back({fname, error, file.filePath(), false});
        delete lib;
        return false;
    }
}

std::vector<Plugin*> plugs = {};
std::vector<FailedPlug> failedPlugs = {};
std::vector<DisabldPlug> disabldPlugs;

Plugin::Plugin(QString nme, Registry& r, QLibrary* library, QString pth)
    : name(std::move(nme)), reg(r), lib(library), path(pth) { reg.loadFn(); isBI = false; }
Plugin::~Plugin() {
    reg.unloadFn();
    lib->unload();
    delete lib;
}
bool Plugin::run() {
    return reg.runFn();
}
void Plugin::loadCols() {
    return reg.loadColsFn();
}

void clearPlugins() {
    for (int i = 1; i < plugs.size(); i++) delete plugs[i];
    plugs.clear();
    failedPlugs.clear();
    disabldPlugs.clear();
}

void loadPlugins() {
    clearPlugins();

    const QString biPlugNam = "libbuiltin"+suffix;
    QString fpth = QCoreApplication::applicationDirPath()+"/"+biPlugNam;
    if (!QFile::exists(fpth)) {
        Log::Warn(MODULE) << "Could not locate built-in plugin!";
    } else {
        if (QFile::exists(getBIDisPath())) {
            disabldPlugs.push_back({biPlugNam, fpth, true});
        } else {
            QFileInfo BIlib(fpth);
            if (tryLoad(BIlib)) {
                plugs.back()->isBI = true;
            } else {
                Log::Warn(MODULE) << "Error when loading built-in plugin!";
                failedPlugs.back().isBI = true;
            }
        }
    }

    QDir dir(getPlugsPath());
    QFileInfoList files = dir.entryInfoList(QStringList{"*"+suffix, "*"+suffix+"."+disabl}, QDir::Files);
    for (const QFileInfo& file : files) {
        if (file.suffix() == disabl) {
            disabldPlugs.push_back({file.fileName(), file.filePath(), false});
        } else {
            tryLoad(file);
        }
    }

    initColours();
    for (auto* p : plugs) {
        p->loadCols();
    }
    Log::Info(MODULE) << "Loaded plugins:\n" << plugs.size() << " successfully\n"
                                             << failedPlugs.size() << " unsuccessfully\n"
                                             << disabldPlugs.size() << " disabled!";
}

