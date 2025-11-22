#include "svgRend.hpp"
#include "../log.hpp"
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QRandomGenerator>

const QString MODULE = "SvgRenderer";

_colourMapTyp pallete = {
    {"light",  "#56B75D"},
    {"dark",   "#20471C"},
    {"shadow", "#436253"},
    {"faded",  "#699C6B"},
    {"alight", "#B87333"},
    {"adark",  "#5A280A"}
};

QString getCol(QString palleteCol, int avgOffs, int offset, int diff) {
    return getQCol(palleteCol, avgOffs, offset, diff).name(QColor::HexRgb);
}
QColor getQCol(QString palleteCol, int avgOffs, int offset, int diff) {
    if (!pallete.contains(palleteCol)) {
        Log::Error(MODULE) << "Could not find pallete colour " << palleteCol;
    }
    QColor c(pallete[palleteCol]);

    int qdiff = diff/4;
    int hdiff = QRandomGenerator::global()->bounded(-qdiff, qdiff);
    auto wiggle = [hdiff, qdiff](int v) {
        int delta = QRandomGenerator::global()->bounded(-qdiff, qdiff);
        int out = v + hdiff + delta;
        return out;
    };
    int r = wiggle(c.red()   + offset);
    int g = wiggle(c.green() + offset);
    int b = wiggle(c.blue()  + offset);

    if (avgOffs != 0) {
        int tot = r + g + b;
        if (tot != 0) {
            qreal off = avgOffs;
            r += off / tot * r;
            g += off / tot * g;
            b += off / tot * b;
        }
    }

    return QColor(std::clamp(r, 0, 255), std::clamp(g, 0, 255), std::clamp(b, 0, 255));
}

_colourMapTyp colours = {};

void initColours() {
    // Backgrounds
    colours["sBgTrees"] = getCol("dark", 42);
    colours["fBgLight"] = getCol("light");
    colours["fBgDark"]  = getCol("shadow", -20, 10);
    colours["sFarTrees"]= getCol("shadow");
    colours["fFarTrees"]= getCol("faded");
    // The tree
    colours["ftreeDark"]   = getCol("dark", -60);
    colours["ftreeLight"]  = getCol("dark", 50);
    colours["ftreeHl"]     = getCol("shadow", 70);
    colours["ftreeShadow"] = getCol("dark");
    colours["ftreeShadow2"]= getCol("dark", 30, 20);
    // Tree base
    colours["fbaseOuter"] = getCol("shadow", -42);
    colours["fbaseInner"] = getCol("dark", -42);
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
