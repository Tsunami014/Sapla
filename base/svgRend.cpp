#include "svgRend.hpp"
#include "../log.hpp"
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QRandomGenerator>

const QString MODULE = "SvgRenderer";

_colourMapTyp pallete = {
    {"light",  "#8090D9"},
    {"dark",   "#0D0D1A"},
    {"shadow", "#2E2E4D"},
    {"faded",  "#5A5A7D"},
    {"alight", "#9D769D"},
    {"adark",  "#554422"}
/*
    {"light",  "#56B75D"},
    {"dark",   "#20471C"},
    {"shadow", "#436253"},
    {"faded",  "#699C6B"},
    {"alight", "#B87333"},
    {"adark",  "#5A280A"}
*/
};

QString getCol(QString palleteCol, std::tuple<int, int, int> offsets, int diff) {
    if (!pallete.contains(palleteCol)) {
        Log::Error(MODULE) << "Could not find pallete colour " << palleteCol;
    }
    QColor c(pallete[palleteCol]);

    int qdiff = diff/4;
    int hdiff = QRandomGenerator::global()->bounded(-qdiff, qdiff);
    auto wiggle = [hdiff, qdiff](int v) {
        int delta = QRandomGenerator::global()->bounded(-qdiff, qdiff);
        int out = v + hdiff + delta;
        return std::clamp(out, 0, 255);
    };
    int r = wiggle(c.red()   + std::get<0>(offsets));
    int g = wiggle(c.green() + std::get<1>(offsets));
    int b = wiggle(c.blue()  + std::get<2>(offsets));
    return QColor(r, g, b).name(QColor::HexRgb);
}

_colourMapTyp colours = {};

void initColours() {
    // Backgrounds
    colours["sBgTrees"] = getCol("dark",   {6, -5, 20});
    colours["fBgLight"] = getCol("light",  {0, 0, 0});
    colours["fBgDark"]  = getCol("shadow", {-34, 12, -24});
    colours["sFarTrees"]= getCol("shadow", {0, 0, 0});
    colours["fFarTrees"]= getCol("faded",  {0, 0, 0});
    // The tree
    colours["ftreeDark"]   = getCol("dark",  {-17, -28, -15});
    colours["ftreeLight"]  = getCol("dark",  {4, 34, 14});
    colours["ftreeHl"]     = getCol("shadow",{21, 36, -17});
    colours["ftreeShadow"] = getCol("dark",  {0, 0, 0});
    colours["ftreeShadow2"]= getCol("dark",  {15, 12, 9});
    // Tree base
    colours["fbaseOuter"] = getCol("shadow", {10, -15, -25});
    colours["fbaseInner"] = getCol("dark",   {4, -22, 2});
}

QByteArray RenderSvg(const QString& svgPath) {
    QFile file(svgPath);
    if (!file.open(QIODevice::ReadOnly)) {
        Log::Warn(MODULE) << "Could not open SVG: " << svgPath;
        return {};
    }

    QXmlStreamReader reader(&file);
    QByteArray output;
    QXmlStreamWriter writer(&output);
    writer.setAutoFormatting(false);

    while (!reader.atEnd()) {
        reader.readNext();

        if (reader.isStartElement()) {
            writer.writeStartElement(reader.name().toString());

            auto attrs = reader.attributes();
            QString classAttr = attrs.value("class").toString();

            for (const QString &cls : classAttr.split(' ', Qt::SkipEmptyParts)) {
                if (colours.contains(cls)) {
                    QString looking4;
                    if (cls[0] == 's') {
                        looking4 = "stroke";
                    } else {
                        looking4 = "fill";
                    }
                    for (int i = 0; i < attrs.size(); ++i) {
                        if (attrs[i].qualifiedName().toString() == looking4) {
                            attrs.removeAt(i);
                            break;
                        }
                    }
                    attrs.append(looking4, colours[cls]);
                }
            }

            for (const auto &a : attrs) {
                writer.writeAttribute(a.qualifiedName().toString(), a.value().toString());
            }
        }
        else if (reader.isEndElement()) {
            writer.writeEndElement();
        }
        else if (reader.isCharacters()) {
            writer.writeCharacters(reader.text().toString());
        }
    }

    return output;
}
