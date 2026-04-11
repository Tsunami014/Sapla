// Features that have hardcoded behaviour built in
#include "features.hpp"

const QRegularExpression scheduleInfRe(R"((?:\n+|^)<<(.*)>>\n*(?=\n|$))");
QString ScheduleFeat::replacements(QString& txt, Side s) const {
    return txt.remove(scheduleInfRe);
}
QString ScheduleFeat::markup(QString& line) const {
    if (line.startsWith("&lt;&lt;") && line.endsWith("&gt;&gt;")) {
        return "<i style='color:#CCC;font-size:14px'>Scheduling info</i>";
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
    static const QRegularExpression re(R"(( +|^|(?<=>))#((?:\\ |[^ \n<])+)( *)(?:(?=[ <])|$))", MO);
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
        }
    };
}


const QRegularExpression staRe(R"((?<!\\)(?:\\\\)*\|)", MO);
QStringList splTemplArgs(QString args) {
    QStringList parts = args.split(staRe);
    for (QString &p : parts) {
        p = p.trimmed();
    }
    return parts;
}
const QString templBaseNameRe = R"(\s*(?<nam>[^|: \n]+?)\s*)";
const QString templBasePatternRe = R"(([|: \n]\s*(?<!\\)\[(?<ptn>(?:\\\]|[^\n\]])+)\]\s*)?)";
const QString templBaseContentsRe = R"(([|: \n]\s*(?<conts>(?:.|\n)*?)\s*)??)";
const QString templDefBase =
    templBaseNameRe + templBasePatternRe + templBaseContentsRe;
const QString templDefPref = R"(\s*^\s*)";
const QString templDefSuff = R"(\s*$\s*?(?=\n)?)";
const QRegularExpression templDefRe(
    templDefPref + QString(R"(\|=%1=\|)").arg(templDefBase) + templDefSuff, MO);
const QRegularExpression templLoclDefRe(
    templDefPref + QString(R"(\|:%1:\|)").arg(templDefBase) + templDefSuff, MO);
const QRegularExpression templApplyRe(
    QString(R"((?<!\\)\|(?:\|%1|!(?<nam2>[^ \t\n:=|])\s*)%2(?<!\\)\|\|)")
        .arg(templBaseNameRe).arg(templBaseContentsRe), MO);
QString TemplateFeat::check(QString& txt, QString& err) const {
    std::map<QString, Template> loclTempls;
    {
        auto it = templLoclDefRe.globalMatch(txt);
        while (it.hasNext()) {
            auto m = it.next();
            QString title = m.captured("nam");
            if (loclTempls.find(title) != loclTempls.end()) {
                err += "Multiple local templates named `" + title + "`!\n";
                if (!loclTempls[title].failed())
                    loclTempls.emplace(title, Template());
            } else {
                loclTempls.try_emplace(title, 
                    m.captured("conts"), m.captured("ptn")
                );
            }
        }
    }
    txt = txt
        .remove(templDefRe)
        .remove(templLoclDefRe)
    ;
    auto it = templApplyRe.globalMatch(txt);
    int offs = 0;
    while (it.hasNext()) {
        auto m = it.next();

        QString g1 = m.captured("nam");
        QString name = g1.isNull() ? m.captured("nam2") : g1;
        Template* templ;
        QString repl;
        if (auto it = loclTempls.find(name); it != loclTempls.end()) {
            templ = &it->second;
        } else if (auto it = globalTemplates.find(name); it != globalTemplates.end()) {
            templ = &it->second;
        } else {
            err += "Unknown template name: `" + name + "`\n";
            continue;
        }
        if (templ->failed()) {
            err += "Template `" + name + "` failed!\n";
            continue;
        }
        QString match = m.captured("conts");
        if (!match.isNull()) {
            repl = templ->replace(splTemplArgs(match));
        } else {
            repl = templ->replace();
        }

        int start = m.capturedStart(0) + offs;
        int end = m.capturedEnd(0) + offs;
        txt.replace(start, end - start, repl);
        offs += repl.length() - (end - start);
    }
    return txt;
}
QString TemplateFeat::replacements(QString& txt, Side s) const {
    return txt
        .remove(templDefRe)
        .remove(templLoclDefRe)
    ;
}
const QRegularExpression templApplyReplRe(R"((?<!\\)(\|![^ \t\n:=<]))");
QString TemplateFeat::markup(QString& line) const {
    const QString boldify = "<b style='color:%1;'>%2</b>";
    QString nln = line
        .replace(templApplyReplRe, QString(boldify).arg(cols[0]).arg("\\1"));

    int si = 0;
    while (si < nln.size() && nln[si].isSpace()) si++;
    QString lftstrpln = nln.mid(si);
    if (lftstrpln.startsWith("|=")) {
        nln.replace(si, 2, QString(boldify).arg(cols[1], "|="));
    } else if (lftstrpln.startsWith("|:")) {
        nln.replace(si, 2, QString(boldify).arg(cols[1], "|:"));
    }

    int ei = nln.size() - 1;
    while (ei >= 0 && nln[ei].isSpace()) ei--;
    QString rtstrpln = nln.left(ei+1);
    if (rtstrpln.endsWith("=|")) {
        nln.replace(ei-1, 2, QString(boldify).arg(cols[1], "=|"));
    } else if (rtstrpln.endsWith(":|")) {
        nln.replace(ei-1, 2, QString(boldify).arg(cols[1], ":|"));
    }

    return nln;
}
QMap<QString, QString> TemplateFeat::help() const {
    return {
           {"Template definition\n|= =|",
            "Defines a note template\n"
            "See this screen help for more info"
        }, {"Local template definition\n|: :|",
            "Defines a note template but only for this note (other notes can't use it)\n"
            "See this screen help for more info"
        }, {"Template usage\n|| ||",
            "Uses a template for the note\n"
            "See this screen help for more info"
        }
    };
}
