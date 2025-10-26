#pragma once
#include <QByteArray>

enum Seasons { SPRING, SUMMER, AUTUMN, WINTER };
inline Seasons season = AUTUMN;

//                                             Spring,  Summer,  Automn,  Winter
using _colourMapTyp = QMap<QString, std::tuple<QString, QString, QString, QString>>;
extern _colourMapTyp colours;

QByteArray RenderSvg(const QString& svgPath);

