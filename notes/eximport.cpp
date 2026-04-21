#include "eximport.hpp"
#include "decks.hpp"
#include "../menu.hpp"
#include <QFileDialog>

const QString anyfilter = "Any supported file (*.deck *.md *.csv *.txt);;";
const QString filter =
    "Deck file (*.deck);;"
    "Markdown file (*.md);;"
    "Markdown table (*.md);;"
    "CSV file (*.csv);;"
    "Plain text (*.txt)"
;
bool tryExport() {
    dialogging = true;
    QString out = QFileDialog::getSaveFileName(nullptr, "Export deck "+curDeck, QDir::homePath(), filter);
    dialogging = false;
    if (out.isEmpty()) return false;
    return false;
}
bool tryImport() {
    dialogging = true;
    QString out = QFileDialog::getOpenFileName(nullptr, "Import deck", QDir::homePath(), anyfilter+filter);
    dialogging = false;
    if (out.isEmpty()) return false;
    return false;
}
