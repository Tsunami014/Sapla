#include "eximport.hpp"
#include "decks.hpp"
#include "getNotes.hpp"
#include "../menu.hpp"
#include "../log.hpp"
#include "../core.hpp"
#include <QFileDialog>

const QString MODULE = "ExImport";

struct Port {
    QString name;
    QString ext;
    std::function<QString()> xport;
    std::function<QString(QString)> import;
    QString getFilter() const {
        return QString("%1 (*%2)").arg(name).arg(ext);
    }
};

const auto close = [](){
    dialogging = false;
    MG->curScene->dialogClose();
};

const std::vector<Port> ports = {
    { "Deck file", ".deck",
        [](){
            QString out;
            for (auto* n : notesL) {
                out += makeSafe(n->contents()) + "\n";
            }
            return out;
        }, [](QString in){ return in; }
    }
};
/*"Markdown file (*.md)",
"Markdown table (*.md)",
"CSV file (*.csv)",
"Plain text (*.txt)"*/
bool xport(const Port& p, QString pth) {
    QFile file(pth);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream(&file) << p.xport();
        file.close();
        Log::Info(MODULE) << "Exported deck to file `" << pth << "`!";
        close();
        return true;
    } else {
        Log::Error(MODULE) << "Unable to open file `" << pth << "`!";
        close();
        return false;
    }
}

const QString anyfilter() {
    QStringList out;
    for (auto& p : ports) out << "*"+p.ext;
    return "Any supported file ("+out.join(' ')+")";
}
const QStringList filters() {
    QStringList l;
    for (auto& p : ports) l << p.getFilter();
    return l;
}
bool tryExport() {
    QString filtr;
    dialogging = true;
    MG->curScene->dialogOpen();
    QString out = QFileDialog::getSaveFileName(nullptr, "Export deck "+curDeck, QDir::homePath()+"/"+curDeck, filters().join(";;"), &filtr);
    if (out.isEmpty()) {
        close();
        return false;
    }

    QString name;
    if (out.contains('/')) {
        name = out.sliced(out.lastIndexOf('/')+1);
    } else {
        name = out;
    }
    if (name.contains('.')) {
        QString ext = name.sliced(name.lastIndexOf('.'));
        for (auto& p : ports) {
            if (p.ext == ext) {
                return xport(p, out);
            }
        }
    }
    for (auto& p : ports) {
        if (p.getFilter() == filtr) {
            return xport(p, out+p.ext);
        }
    }
    // Should never happen
    Log::Warn(MODULE) << "Unable to determine an export filter!";
    close();
    return false;
}
bool tryImport() {
    dialogging = true;
    MG->curScene->dialogOpen();
    QString out = QFileDialog::getOpenFileName(nullptr, "Import deck", QDir::homePath(), anyfilter());
    if (out.isEmpty()) {
        close();
        return false;
    }

    QFile file(out);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Log::Error(MODULE) << "Failed to read from file `" << out << "`!";
        return false;
    }
    QString content = file.readAll();
    file.close();

    QString name;
    if (out.contains('/')) {
        name = out.sliced(out.lastIndexOf('/')+1);
    } else {
        name = out;
    }
    if (name.contains('.')) {
        QString ext = name.sliced(name.lastIndexOf('.'));
        for (auto& p : ports) {
            if (p.ext == ext) {
                QString newname = name.sliced(0, name.lastIndexOf('.'));
                bool resp = loadNewDeck(p.import(content), newname);
                if (resp) Log::Info(MODULE) << "Imported deck `" << newname << "`!";
                else Log::Error(MODULE) << "Failed importing deck `" << newname << "`!";
                close();
                return resp;
            }
        }
    }
    Log::Warn(MODULE) << "Unable to determine an import filter! To import a file, I need the correct file extension.";
    close();
    return false;
}
