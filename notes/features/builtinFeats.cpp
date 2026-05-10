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
    return {{"Schedule\n<< >>",
        "WARNING: only for if you are bothered - this is auto generated and you do not need to know ANY of this.\n"
        "Schedule strings have the following format:\n"
        "<<feat,idx,score,time>>\n"
        "Where feat is the feature name (suffixed with _<idx> if using ~~~, if not there defaults to _0), "
            "idx is the idx of the note (used by each feature in a different way), "
            "score is the score given to the note of how well you remembered it, "
            "and time is the next time it will be shown.\n"
        "Multiple can be chained with | e.g. `<<f,i,s,t|f2,i2,s2,t2>>`\n"
        "They must not have anything before or after them on the same line."
    }};
}

const QRegularExpression mnsRe("^ *~~~ *$", MO);
QString MultinoteSepFeat::markup(QString& line) const {
    if (auto m = mnsRe.match(line); m.hasMatch()) {
        return QString("<span style='color:%1'>～～～</span>").arg(col);
    }
    return line;
}
QMap<QString, QString> MultinoteSepFeat::help() const {
    return {{"Multinote separator\n~~~",
        "This splits the note into multiple notes, allowing for features to be used more than once.\n"
        "Very useful in note templates."
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


const QRegularExpression staRe(R"(\s*(?:(?<=(?<!\\)\|)>\|\s*(.+?)\s*\|<\||(.*?)\s*(?<!\\)(?:\||$))\s*)", MO | QRegularExpression::DotMatchesEverythingOption);
QStringList splTemplArgs(QString args) {
    QStringList out;
    auto it = staRe.globalMatch(args);
    while (it.hasNext()) {
        auto m = it.next();
        if (QString txt = m.captured(1); !txt.isEmpty()) out << txt;
        if (QString txt = m.captured(2); !txt.isEmpty()) out << txt;
    }
    return out;
}
QString TemplateFeat::check(QString& txt, QString& err) const {
    std::map<QString, Template> loclTempls;
    auto ts = getTempls(txt, false);
    for (auto t : ts) {
        QString title = t.first;
        if (loclTempls.find(title) != loclTempls.end()) {
            err += "Multiple local templates named `" + title + "`!\n";
            if (!loclTempls[title].failed())
                loclTempls[title] = Template();
        } else {
            loclTempls[title] = t.second;
        }
    }
    txt = rmTemplDefs(txt);
    for (uint _ = 0; _ < MAX_RECURSION; _++) {
        QString txt2 = txt;
        auto* t = applyTempl(txt2, err);
        int offs = 0;
        while (t->success()) {
            QString name = t->name;
            Template* templ;
            QString repl;
            bool good = true;
            if (auto it = loclTempls.find(name); it != loclTempls.end()) {
                templ = &it->second;
            } else if (auto it = globalTemplates.find(name); it != globalTemplates.end()) {
                templ = &it->second;
            } else {
                err += "Unknown template name: `" + name + "`\n";
                good = false;
            }
            if (templ->failed()) {
                err += "Template `" + name + "` failed!\n";
                good = false;
            }
            if (good) {
                QString match = t->conts;
                if (!match.isEmpty()) {
                    repl = templ->replace(splTemplArgs(match));
                } else {
                    repl = templ->replace();
                }
            } else { repl = ""; }

            int start = (t->start - txt2.begin()) + offs;
            int end = (t->end - txt2.begin()) + offs;
            txt.replace(start, end - start, repl);
            offs += repl.length() - (end - start);
            applyTempl(t);
        }
        delete t;
    }
    return txt;
}
QString TemplateFeat::replacements(QString& txt, Side s) const {
    return rmTemplDefs(txt);
}
const QRegularExpression templApplyReplRe(R"((?<!\\)(\|!(?:&[^;]+;|[^ \t\n:=<])))");
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
        }, {"Template usage\n|! ||",
            "Uses a template for the note\n"
            "The name for the template must come directly after the ! and be one character long (e.g. |!= ||)\n"
            "See this screen help for more info"
        }
    };
}
