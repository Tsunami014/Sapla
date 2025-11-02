#include "svgRend.hpp"
#include "../log.hpp"
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

_colourMapTyp colours = {
    // Backgrounds: Spring,    Summer,    Automn,    Winter
    {"sBgTrees",  {"#26432F", "#26432F", "#3B3122", "#26432F"}},
    {"fBgLight",  {"#56B75D", "#56B75D", "#C57631", "#56B75D"}},
    {"fBgDark",   {"#216E3B", "#216E3B", "#91371E", "#216E3B"}},
    {"sFarTrees", {"#436253", "#436253", "#76614C", "#436253"}},
    {"fFarTrees", {"#699C6B", "#699C6B", "#A56D3F", "#699C6B"}},
    // The tree:      Spring,    Summer,    Automn,    Winter
    {"ftreeDark",   {"#0F2B0D", "#0F2B0D", "#2B1B10", "#0F2B0D"}},
    {"ftreeLight",  {"#24682A", "#24682A", "#AC651F", "#24682A"}},
    {"ftreeHl",     {"#588642", "#588642", "#CA905D", "#588642"}},
    {"ftreeShadow", {"#20471C", "#20471C", "#63371C", "#20471C"}},
    {"ftreeShadow2",{"#2F5325", "#2F5325", "#8A5834", "#2F5325"}},
    // Tree base:    Spring,    Summer,    Automn,    Winter
    {"fbaseOuter", {"#4E533A", "#4E533A", "#534534", "#4E533A"}},
    {"fbaseInner", {"#24311E", "#24311E", "#432C19", "#24311E"}},
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
                    auto* cols = &colours[cls];
                    QString col;
                    switch (season) {
                        case SPRING: col = std::get<0>(*cols); break;
                        case SUMMER: col = std::get<1>(*cols); break;
                        case AUTUMN: col = std::get<2>(*cols); break;
                        case WINTER: col = std::get<3>(*cols); break;
                    }
                    attrs.append(looking4, col);
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
