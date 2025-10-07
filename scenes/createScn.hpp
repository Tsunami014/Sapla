#pragma once
#include "baseScn.hpp"
#include "../items/listIt.hpp"

class CreateScene : public BaseScene {
public:
    CreateScene();
    void onEvent(QEvent* event) override;
    void resize() override;

protected:
    TextListItem* tl;
};
