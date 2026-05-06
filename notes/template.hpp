#pragma once
#include <QString>
#include <variant>
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

    void handlePtns(QString p);
    void GeneratePatterns(QString conts, uint& i, bool rev);
    bool parseArg(QStringList args, QString& repl, QString pref, QString suff);
    QString replace_main(QStringList args);
    QString replace_inner(QStringList args, QString conts, uint depth);

    struct vartyp {
        std::variant<float, QString> dat;
        QString getStr(bool *ok) {
            if (std::holds_alternative<QString>(dat)) {
                *ok = true; return std::get<QString>(dat);
            } *ok = false; return {};
        }
        float getNum(bool *ok) {
            if (std::holds_alternative<float>(dat)) {
                *ok = true; return std::get<float>(dat);
            } *ok = false; return {};
        }
        QString displ() {
            if (std::holds_alternative<QString>(dat)) {
                return std::get<QString>(dat);
            } return QString::number(std::get<float>(dat));
        }
    };
    QString handlefunc(QStringList args, QString fn);
    vartyp hfvar(QStringList args, QString nam);
};

extern std::map<QString, Template> globalTemplates;
