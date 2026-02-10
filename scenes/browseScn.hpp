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
    void delNote();

protected:
    Note* getSelNote();
    Note* getNote(QTreeWidgetItem* item);
    QTreeWidget* tree;
    QVBoxLayout* form;
    MarkdownEdit* te;
    Topbar* bar;

    void updatePrev();
    Side side;
    MarkdownEdit* preview;
    struct prevIdxTyp {
        int idx;
        int max;
        void reset();
    };
    prevIdxTyp prevIdx;

    bool doubleCheck(QString prompt);
private:
    QLabel* prevIdxLabl;

    MenuAction m;
    MenuItem resetIt;
    MenuItem clearIt;
};
