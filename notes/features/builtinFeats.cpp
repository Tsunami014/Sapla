// Features that have hardcoded behaviour built in
#include "features.hpp"

const QRegularExpression scheduleInfRe(R"((?:\n+|^)<<(.*)>>\n*(?=\n|$))");
QString ScheduleFeat::replacements(QString& txt, Side s) const {
    return txt.remove(scheduleInfRe);
}
QString ScheduleFeat::markup(QString& line) const {
    if (line.startsWith("&lt;&lt;") && line.endsWith("&gt;&gt;")) {
        return "<i style='color:#CCC'>Scheduling info</i>";
    }
    return line;
}
QMap<QString, QString> ScheduleFeat::help() const {
    return {{"Schedule",
        "Warning: only for if you are bothered\n"
        "Schedule strings have the following format:\n"
        "<<feat,idx,score,time>>\n"
        "Where feat is the feature name, "
            "idx is the idx of the note (used by each feature in a different way), "
            "score is the score given to the note of how well you remembered it, "
            "and time is the next time it will be shown.\n"
        "Multiple can be chained with | e.g. `<<f,i,s,t|f2,i2,s2,t2>>`"
    }};
}

const QRegularExpression noteInfRe(R"((?: +|^)#((?:\\ |[^ \n])+) *(?:(?= )|$))", MO);
QString TagFeat::replacements(QString& txt, Side s) const {
    return txt.remove(noteInfRe);
}
QString TagFeat::markup(QString& line) const {
    static const QRegularExpression re(R"(( +|^)#((?:\\ |[^ \n])+)( *)(?:(?= )|$))", MO);
    return line
        .replace(re, QString(
            "\\1"
            "<b style='color:%1;'>#\\2</b>"
            "\\3"
        ).arg(col));
}
QMap<QString, QString> TagFeat::help() const {
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
QString TemplateFeat::replacements(QString& txt, Side s) const {
    if (s == SIDE_NAME) {
        return txt
            .remove(templDefRe)
            .remove(scheduleInfRe)
        ;
    }
    auto it = templApplyRe.globalMatch(txt);
    int offs = 0;
    while (it.hasNext()) {
        auto m = it.next();

        QString name = m.captured(1);
        if (globalTemplates.find(name) == globalTemplates.end()) {
            continue;
        }
        QString templ = globalTemplates[name];
        QString match = m.captured(2);
        if (!match.isNull()) {
            QStringList gs = match.sliced(1).split('|');
            for (auto& g : gs) {
                templ = templ.arg(g.trimmed());
            }
        }

        int start = m.capturedStart(0) + offs;
        int end = m.capturedEnd(0) + offs;
        txt.replace(start, end - start, templ);
        offs += templ.length() - (end - start);
    }
    return txt.remove(templDefRe);
}
QString TemplateFeat::markup(QString& line) const {
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
QMap<QString, QString> TemplateFeat::help() const {
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
