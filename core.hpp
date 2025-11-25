#pragma once
#include <QMenuBar>
#include <QStackedLayout>
#include "game.hpp"

const int VERSION = 9;

extern MainGame* MG;
extern const QString* helpStr;

int runApp(int argc, char *argv[]);

