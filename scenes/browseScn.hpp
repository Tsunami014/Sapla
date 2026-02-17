#pragma once
#include "baseScn.hpp"
#include "../menu.hpp"
#include "../notes/note.hpp"
#include "../base/markdown.hpp"
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QLabel>

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

    void updatePrev();
    Side side;
    MarkdownEdit* preview;
    struct prevIdxTyp {
        int idx;
        int max;
        void reset();
    };
    prevIdxTyp prevIdx;
private:
    QLabel* prevIdxLabl;

    MenuItem newnote;
    MenuItem delnote;
    MenuItem helps;
    MenuItem resetIt;
    MenuItem clearIt;
};
