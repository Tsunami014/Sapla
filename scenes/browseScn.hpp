#pragma once
#include "baseScn.hpp"
#include "../menu.hpp"
#include "../notes/note.hpp"
#include "../wids/topbar.hpp"
#include "../base/markdown.hpp"
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QMenuBar>

class BrowseScene : public BaseScene {
public:
    BrowseScene();
    bool keyEv(QKeyEvent* event) override;

public slots:
    void typed();
    void selectionChange();
    void newNote();

protected:
    QTreeWidget* tree;
    QVBoxLayout* form;
    MarkdownEdit* te;
    Topbar* bar;

private:
    MenuAction m;
    MenuItem resetIt;
    MenuItem clearIt;
    bool doubleCheck(QString prompt);
};
