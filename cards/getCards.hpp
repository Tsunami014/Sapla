#pragma once
#include <vector>
#include "cardTyps.hpp"

QString tryReadLine(QTextStream& in, QString error = "");
QString makeSafe(QString str);
QString unSafe(QString str);

QString getPath();
void initCards();
void writeCards();

