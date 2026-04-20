#pragma once
#include <QString>
#include <map>

const uint MAX_RECURSION = 10;

class Template {
public:
    Template(QString conts, QString ptn);
    Template();
    bool failed();
    QString replace();
    QString replace(QStringList args);
protected:
    QString conts;
    std::map<QString, QString> ptns;
    bool parseArg(QString& repl, QString pref, QString suff);
    QString replace(QStringList args, QString conts, uint depth=0);
};

extern std::map<QString, Template> globalTemplates;
