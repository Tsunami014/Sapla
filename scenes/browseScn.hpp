#pragma once
#include "baseScn.hpp"
#include "../menu.hpp"
#include "../notes/note.hpp"
#include "../base/markdown.hpp"
#include "../wids/schdInfWid.hpp"
#include "xtra/browseInfo.hpp"
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QLineEdit>
#include <QLabel>

class BrowseScene : public BaseScene {
public:
    BrowseScene(Note* sel = nullptr);
    bool keyEv(QKeyEvent* event) override;

public slots:
    void typed();
    void selectionChange();
    void filterChanged();
    void newNote();
    void delNote(bool desel = false);

protected:
    Note* getSelNote();
    QLineEdit* filter;
    QTreeWidget* tree;
    QVBoxLayout* form;
    MarkdownEdit* te;

    void updatePrev();
    void updateInfo();
    Side side;
    SchdInfoWid* siw;
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
    BrowseInfos helps;
    MenuItem resetIt;
    MenuItem clearIt;
    MenuAction info;
};
