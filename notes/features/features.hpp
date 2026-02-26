// A file to import containing all the important stuff
#pragma once
#include "notes/features.hpp"
#include "log.hpp"
#include <QRegularExpression>

const QString MODULE = "CardFeature";
constexpr auto MO = QRegularExpression::MultilineOption;

Schedule getSchd(std::map<int, Schedule> schds, int idx);
QString trimNL(const QString& orig);
