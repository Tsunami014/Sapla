#include "../features.hpp"

constexpr auto MO = QRegularExpression::MultilineOption;

const QRegularExpression noteInfRe(R"((?: +|^)#((?:\\ |[^ \n])+) *(?:(?= )|$))", MO);
QString TagFeats::replacements(QString& txt, Side s) const {
    return txt.replace(noteInfRe, "");
}
QString TagFeats::markup(QString& line) const {
    static const QRegularExpression re(R"(( +|^)#((?:\\ |[^ \n])+)( *)(?:(?= )|$))", MO);
    return line
        .replace(re, QString(
            "\\1"
            "<b style='color:%1;'>#\\2</b>"
            "\\3"
        ).arg(col));
}
QMap<QString, QString> TagFeats::help() const {
    return {
           {"Tag\n#tag",
            "Add a tag for this note"
        }, {"Priority\n#!prio",
            "Give the card a priority\n"
            "This gets converted to a number, where 0 is the default and the higher the number is, the more important it is."
        }
    };
}



const QString templBaseRe = R"(\s*([^|:\n]+?)\s*([|:\n](?!\|)(?:.|\n)*?)?)";
const QRegularExpression templDefRe("^\\s*\\|=" + templBaseRe + "=\\|\\s*$", MO);
const QRegularExpression templApplyRe("\\|\\|" + templBaseRe + "\\|\\|");
const QRegularExpression scheduleInfRe(R"((?:\n+|^)<<(.*)>>\n*(?=\n|$))");
QString TemplateFeats::replacements(QString& txt, Side s) const {
    return txt
        .replace(templDefRe, "")
        .replace(scheduleInfRe, "")
    ;
}
QString TemplateFeats::markup(QString& line) const {
    if (line.startsWith("&lt;&lt;") && line.endsWith("&gt;&gt;")) {
        return "<i style='color:#CCC'>Scheduling info</i>";
    }
    QString nln = line
        .replace("||", QString("<b style='color:%1;'>||</b>").arg(cols[0]));

    int si = 0;
    while (si < nln.size() && nln[si].isSpace()) si++;
    QString lftstrpln = nln.mid(si);
    if (lftstrpln.startsWith("|=")) {
        nln.replace(si, 2, QString("<b style='color:%1;'>%2</b>").arg(cols[1], "|="));
    }

    int ei = nln.size() - 1;
    while (ei >= 0 && nln[ei].isSpace()) ei--;
    QString rtstrpln = nln.left(ei+1);
    if (rtstrpln.endsWith("=|")) {
        nln.replace(ei-1, 2, QString("<b style='color:%1;'>%2</b>").arg(cols[1], "=|"));
    }

    return nln;
}
QMap<QString, QString> TemplateFeats::help() const {
    return {
           {"Template definition\n|= =|",
            "Defines a note template\n"
            "See this screen help for more info"
        }, {"Template usage\n|| ||",
            "Uses a template for the note\n"
            "See this screen help for more info"
        }
    };
}
