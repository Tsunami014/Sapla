#pragma once
#include <QString>
#include <map>

class TemplPattern {
public:
    TemplPattern(QString conts, uint idx);
    QString apply();
    QString apply(QString arg);
    QString name;
    unsigned int idx;
protected:
    QString def;
};

class Template {
public:
    Template(QString conts, QString ptn);
    QString replace(QStringList args);
protected:
    QString conts;
    std::map<QString, TemplPattern> ptns;
};

extern std::map<QString, Template> globalTemplates;
