#pragma once
#include "baseScn.hpp"
#include "../menu.hpp"
#include <QLabel>
#include <QComboBox>

class HomeScene : public BaseScene {
public:
    HomeScene();
    void dialogClose() override;
private:
    QComboBox* deckNam;
    MenuItem deleteTmps;
};

