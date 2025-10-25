#pragma once
#include <QByteArray>

enum Seasons { SPRING, SUMMER, AUTUMN, WINTER };
inline Seasons season = AUTUMN;

QByteArray RenderSvg(const QString& svgPath);

