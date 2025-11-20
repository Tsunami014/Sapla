#pragma once
#include "note.hpp"
#include <vector>
#include <QString>
#include <QTextStream>

inline std::vector<Note*> notesL;
void updateNoteCards();

QString tryReadLine(QTextStream& in, QString error = "");
QString makeSafe(QString str);
QString unSafe(QString str);

QString getPath();
void initNotes();
void writeNotes();

