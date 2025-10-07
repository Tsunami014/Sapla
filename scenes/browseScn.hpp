#pragma once
#include "baseScn.hpp"
#include "../items/listIt.hpp"

class BrowseScene : public BaseScene {
public:
    BrowseScene();
    void onEvent(QEvent* event) override;
    void resize() override;

protected:
    TextListItem* tl;
};
