#pragma once
#include "menu.hpp"

class BrowseInfos : public QObject {
    Q_OBJECT;
public:
    BrowseInfos();
    ~BrowseInfos();
private:
    std::vector<MenuItem*> helps;
    template<typename T> void mkhelp(QString nam);
};
