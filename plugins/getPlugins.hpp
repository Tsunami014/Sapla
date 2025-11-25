#pragma once
#include "link.hpp"
#include "../scenes/baseScn.hpp"
#include <QLibrary>

const extern QString suffix;
QString getPlugsPath();
QString getBIDisPath();

class Plugin {
public:
    Plugin(QString name, Registry& reg, QLibrary* lib, QString pth);
    ~Plugin();
    QString name;
    QString path;
    QString desc;
    bool isBI;
private:
    Registry reg;
    QLibrary* lib;
};

struct FailedPlug {
    QString name;
    QString error;
    QString path;
    bool isBI;
};
struct DisabldPlug {
    QString name;
    QString path;
    bool isBI;
};

using BoolFns = std::vector<bool(*)()>;
using VoidFns = std::vector<void(*)()>;
struct _PFuncs {
    BoolFns playFns;
    VoidFns stylFns;
};
extern _PFuncs* PlugFns;

extern std::vector<Plugin*> plugs;
extern std::vector<FailedPlug> failedPlugs;
extern std::vector<DisabldPlug> disabldPlugs;
void clearPlugins();
void loadPlugins();

