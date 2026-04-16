#include "browseInfo.hpp"
#include "notes/features.hpp"
#include "browseInfoBase.hpp"
#include "help.hpp"


class FeatInfo : public SelectInfo {
    virtual QString title() { return "FeatInfo"; }
    virtual QMap<QString, QString> getObjs() {
        QMap<QString, QString> merged;
        for (const auto& f : Feats) {
            merged.insert(f->help());
        }
        return merged;
    }
};
class ArgInfo : public TextInfo {
    virtual QString title() { return "ArgInfo"; }
    virtual QString getTxt() {
        return ARGUMENT_HELP;
    }
};


template<typename T>
void BrowseInfos::mkhelp(QString nam) {
    auto it = new MenuItem(nam, Menues->HelpMenu);
    helps.push_back(it);
    connect(it, &QAction::triggered, this, []() {
        Dialog* obj = new T();
        obj->run();
        delete obj;
    });
}
BrowseInfos::BrowseInfos() {
    mkhelp<FeatInfo>("Note feature help");
    mkhelp<ArgInfo>("Argument help");
}
BrowseInfos::~BrowseInfos() {
    for (auto& h : helps) delete h;
}
