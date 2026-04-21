#pragma once
#include <QString>
#include <map>

const uint MAX_RECURSION = 15;

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

    void GeneratePatterns(QString conts, uint& i);
    bool parseArg(QStringList args, QString& repl, QString pref, QString suff);
    QString replace_main(QStringList args);
    QString replace_inner(QStringList args, QString conts, uint depth);
};

extern std::map<QString, Template> globalTemplates;
