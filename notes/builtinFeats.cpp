#include "features.hpp"

constexpr auto MO = QRegularExpression::MultilineOption;

const QRegularExpression noteInfRe(R"(^ *@ *([^@: \n]+) *: *(.+?) *@ *$\n?)", MO);
const QRegularExpression scheduleInfRe(R"(\n?^\|\|(.*)\|\|$\n*)", MO);
QString TagFeats::replacements(QString& txt, Side s) const {
    return txt.replace(scheduleInfRe, "")
              .replace(noteInfRe, "");
}
QString TagFeats::markup(QString& line) const {
    static const QRegularExpression re(R"((^ *@ *)([^@: \n]+)( *:)(.+?)@ *$)", MO);
    return line
        .replace(re, QString(
            "<b style='color:%1;'>\\1</b>"
            "<b style='color:%2;'>\\2</b>"
            "<b style='color:%1;'>\\3</b>"
            "\\4"
            "<b style='color:%1;'>@</b>"
        ).arg(cols[0], cols[1]));
}
QMap<QString, QString> TagFeats::help() const {
    return {
           {"Tag\n@tag:@",
            "Add tags (separated by ,) for this note"
        }, {"Priority\n@prio:@",
            "(also can be `@priority:@`)\n"
            "Give the card a priority\n"
            "This gets converted to a number, where 0 is the default and positives are more important."
        }
    };
}



const QRegularExpression templDefRe(R"(^ *%\|\s*([^|%\n ]+) *[ |\n] *((?:.|\n)*?)\s*\|% *)", MO);
const QRegularExpression templApplyRe(R"(%%\s*([^%\n ]+)\s*(?:[ |\n]\s*(.*?)(?:\s*[|\n]\s*(.*?))*?)?\s*%%)", MO);
QString TemplateFeats::replacements(QString& txt, Side s) const {
    return txt.replace(templDefRe, "");
}
QString TemplateFeats::markup(QString& line) const {
    if (line.startsWith("||") && line.endsWith("||")) {
        return "<i style='color:#CCC'>Scheduling info</i>";
    }
    QString nln = line
        .replace("%%", QString("<b style='color:%1;'>%%</b>").arg(cols[0]));

    int si = 0;
    while (si < nln.size() && nln[si].isSpace()) si++;
    QString lftstrpln = nln.mid(si);
    if (lftstrpln.startsWith("%|")) {
        nln.replace(si, 2, QString("<b style='color:%1;'>%2</b>").arg(cols[1], "%|"));
    }

    int ei = nln.size() - 1;
    while (ei >= 0 && nln[ei].isSpace()) ei--;
    QString rtstrpln = nln.left(ei+1);
    if (rtstrpln.endsWith("|%")) {
        nln.replace(ei-1, 2, QString("<b style='color:%1;'>%2</b>").arg(cols[1], "|%"));
    }

    return nln;
}
QMap<QString, QString> TemplateFeats::help() const {
    return {
           {"Template definition\n%||%",
            "Defines a note template\n"
            "See this screen help for more info"
        }, {"Template usage\n%%",
            "Uses a template for the note\n"
            "See this screen help for more info"
        }
    };
}
