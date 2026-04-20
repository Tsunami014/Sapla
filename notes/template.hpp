#pragma once
#include <QString>
#include <map>

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
};

extern std::map<QString, Template> globalTemplates;
