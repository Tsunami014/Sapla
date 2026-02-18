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

QString tryReadLine(QTextStream& in, QString error = "");
QString makeSafe(QString str);
QString unSafe(QString str);

QString getPath();
void writeNotes();

