#pragma once
#include "note.hpp"
#include <vector>
#include <QString>
#include <QTextStream>

inline std::vector<Note*> notesL;
void updateNoteCards();

extern std::vector<QString> decks;
static QString curDeck;
bool checkValidDeck();

void loadDefaultNotes();
void initNotes();

QString tryReadLine(QTextStream& in, QString error = "");
QString makeSafe(QString str);
QString unSafe(QString str);

QString getPath();
void writeNotes();
void changeDeck(QString newname);
int renameDeck(QString newname);
int deleteDeck();

