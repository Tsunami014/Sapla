#include "../core.hpp"
#include "../log.hpp"
#include "../menu.hpp"
#include "../base/svgRend.hpp"
#include "getPlugins.hpp"
#include <QStandardPaths>
#include <QCoreApplication>
#include <QRandomGenerator>
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
    auto fail = [&](QString error){
        Log::Error(MODULE) << error;
        lib->unload();
        failedPlugs.push_back({fname, error, file.filePath(), false});
        delete lib;
        return false;
    };

    auto verFn = reinterpret_cast<Version (*)()>(lib->resolve("_versions"));
    if (verFn) {
        Version ver = verFn();
        if (ver.from > VERSION || ver.to < VERSION) {
            return fail(QString(
"Incorrect version! \
Plugin requires versions in range %1 - %2, but this app is version %3!")
                .arg(ver.from).arg(ver.to).arg(VERSION)
            );
        }
    } else {
        return fail("No '_versions' function found in "+fname);
    }
    auto regFn = reinterpret_cast<Registry (*)()>(lib->resolve("_register"));
    if (regFn) {
        Registry reg = regFn();
        plugs.push_back(new Plugin(fname, reg, lib, file.filePath()));
        return true;
    } else {
        return fail("No '_register' function found in "+fname);
    }
}

_PFuncs* PlugFns = new _PFuncs();
std::vector<Plugin*> plugs = {};
std::vector<FailedPlug> failedPlugs = {};
std::vector<DisabldPlug> disabldPlugs;

#define sync(from, to) to.insert(to.end(), from.fns, from.fns + from.count)
Plugin::Plugin(QString nme, Registry& r, QLibrary* library, QString pth)
    : name(std::move(nme)), reg(r), lib(library), path(pth) {
        sync(reg.onStyl, PlugFns->stylFns);
        sync(reg.onPlay, PlugFns->playFns);
        desc = QString::fromUtf8(reg.desc);
        isBI = false;
    }
Plugin::~Plugin() {
    for (int i = 0; i < reg.onUnload.count; i++) {
        reg.onUnload.fns[i]();
    }
    lib->unload();
    delete lib;
}

void clearPlugins() {
    for (int i = 1; i < plugs.size(); i++) delete plugs[i];
    plugs.clear();
    failedPlugs.clear();
    disabldPlugs.clear();
    if (PlugFns != nullptr) {
        delete PlugFns;
        PlugFns = new _PFuncs();
    }
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

    initPallete();
    for (auto* f : PlugFns->stylFns) f();
    initColours();
    MG->initStyles();
    MG->removeGame();
    Log::Info(MODULE) << "Loaded plugins:\n" << plugs.size() << " successfully\n"
                                             << failedPlugs.size() << " unsuccessfully\n"
                                             << disabldPlugs.size() << " disabled!";
}

