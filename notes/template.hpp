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
    QString parseArg(QStringList args, QString nam, QString pref, QString suff, uint depth);
    QString replace_inner(QStringList args, QString conts, uint depth);

private:
    using _parseRes = std::pair<QString, QChar*>;
    _parseRes parseLoop(QStringList args, QChar* it, const QString& out, uint depth);
    _parseRes parseVar(QStringList args, QChar* it, const QString& out, uint depth);
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
    QString handlefunc(QStringList args, QString fn, uint depth);
    vartyp hfvar(QStringList args, QString nam, uint depth);
};

extern std::map<QString, Template> globalTemplates;

std::vector<std::pair<QString, Template>> getTempls(QString txt, bool global);
bool templDefExists(QString txt, bool global);
QString rmTemplDefs(QString txt);
struct _applyRet {
    QChar* it;
    bool success() { return it != nullptr; }
    const QString& txt;
    QString& err;

    QString name = {};
    QString conts = {};
    QChar* start = nullptr;
    QChar* end = nullptr;
};
_applyRet* applyTempl(QString& txt, QString& err);
void applyTempl(_applyRet* inp);
