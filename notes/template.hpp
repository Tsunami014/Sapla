#pragma once
#include <QString>
#include <map>

class TemplPattern {
public:
    TemplPattern(QString conts, uint idx);
    QString name;
    unsigned int idx;
    QString def;
};

class Template {
public:
    Template(QString conts, QString ptn);
    QString replace();
    QString replace(QStringList args);
protected:
    QString conts;
    std::map<QString, TemplPattern> ptns;
};

extern std::map<QString, Template> globalTemplates;
