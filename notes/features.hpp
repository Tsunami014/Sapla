#pragma once
#include <QString>
#include "note.hpp"
#include "../wids/topbar.hpp"

using FeatReturnTyp = std::optional<std::vector<FlashCard>>;
struct FeatRegistry {
    virtual QString markup(QString& line) = 0;
    virtual FeatReturnTyp getFlashCards(Note* parent, const QString& txt) = 0;
    virtual std::vector<BtnFeatures> btns() = 0;
};
inline std::vector<std::unique_ptr<FeatRegistry>> Feats;

#define INIT_FEAT(nam) \
struct nam : FeatRegistry {\
    QString markup(QString& line) override;\
    FeatReturnTyp getFlashCards(Note* parent, const QString& txt) override;\
    std::vector<BtnFeatures> btns() override;\
};
#define REGISTER_FEAT(nam) Feats.push_back(std::make_unique<nam>())


void registerNoteFeatures();

