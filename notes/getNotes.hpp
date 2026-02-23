#pragma once
#include "note.hpp"
#include <vector>
#include <QString>
#include <QTextStream>

inline std::vector<Note*> notesL;
void updateNoteCards();

void loadDefaultNotes();
void clearTempNotes();
void initNotes();

QString makeSafe(QString str);
QString unSafe(QString str);

QString getDecksPath();
void writeNotes();

