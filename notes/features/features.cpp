// Misc function definitions for features to use
#include "features.hpp"
#include "../../plugins/getPlugins.hpp"

QStringList cols = {
    "#E6DDD6",
    "#D6E0D6",
    "#F0D6F0",
    "#D6E0F0",
    "#F0F0D6",
    "#E6FFF0",
    "#E0E0E0",
    "#F0F0F0",
    "#DDE6E6",
    "#E6E6DD",
    "#FDDFDF",
    "#E6DDE6",
    "#DDE6DD"
};
QString _AutoColour::nxtCol() {
    QString& ret = cols[idx];
    idx = (idx + 1) % cols.size();
    return ret;
}

Schedule getSchd(std::map<int, Schedule> schds, int idx) {
    auto it = schds.find(idx);
    if (it != schds.end()) {
        return schds.at(idx);
    }
    return Schedule(idx);
}

void registerNoteFeatures() {
    CardFeats.clear();
    Feats.clear();
    REGISTER_CFEAT(SingleSideFeat);
    REGISTER_CFEAT(DoubleSideFeat);
    REGISTER_CFEAT(SecretFeat);
    REGISTER_FEAT(TagFeats);
    REGISTER_FEAT(TemplateFeats);
    REGISTER_FEAT(HiddenFeat);

    for (auto* f : PlugFns->loadFeatFns) f();

    std::stable_sort(Feats.begin(), Feats.end(), [](const auto& a, const auto& b) {
        return a->order() > b->order();
    });
    std::stable_sort(CardFeats.begin(), CardFeats.end(), [](const auto& a, const auto& b) {
        return a->order() > b->order();
    });

    for (auto& feat : Feats) {
        feat->init();
    }
}

QString trimNL(const QString& orig) {
    QString nstr = orig;
    static const QRegularExpression re(R"(^\s*\n|\n\s*$)");
    return nstr.remove(re);
}


QString FeatReg::highersReplace(QString inp) {
    QString name = getName();
    int ordr = order();
    uint clen = Feats.size();
    uint toIdx;
    bool found = false;
    for (uint i = clen-1; i >= 0; i--) {
        if (!found) {
            if (Feats[i]->getName() == name) {
                toIdx = i;
                found = true;
            }
        } else {
            if (Feats[i]->order() == ordr) {
                toIdx = i;
            } else {
                break;
            }
        }
    }
    if (!found) { // Should never happen
        Log::Error(MODULE) << "FeatReg not found in feats!";
        return inp;
    }
    for (uint i = 0; i < toIdx; i++) {
        inp = Feats[i]->replacements(inp, SIDE_HIDE);
    }
    return inp;
}
