#include "svgRend.hpp"
#include "../log.hpp"
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

_colourMapTyp colours = {
    // Backgrounds
    {"sBgTrees",  "#26432F"},
    {"fBgLight",  "#56B75D"},
    {"fBgDark",   "#216E3B"},
    {"sFarTrees", "#436253"},
    {"fFarTrees", "#699C6B"},
    // The tree
    {"ftreeDark",   "#0F2B0D"},
    {"ftreeLight",  "#24682A"},
    {"ftreeHl",     "#588642"},
    {"ftreeShadow", "#20471C"},
    {"ftreeShadow2","#2F5325"},
    // Tree base
    {"fbaseOuter", "#4E533A"},
    {"fbaseInner", "#24311E"},
};

QByteArray RenderSvg(const QString& svgPath) {
    QFile file(svgPath);
    if (!file.open(QIODevice::ReadOnly)) {
        Log::Warn("SvgRenderer") << "Could not open SVG: " << svgPath;
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
