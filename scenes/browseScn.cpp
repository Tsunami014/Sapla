#include "browseScn.hpp"
#include "../core.hpp"
#include "../help.hpp"
#include "../log.hpp"
#include "../notes/noteTree.hpp"
#include "../notes/getNotes.hpp"
#include "../notes/decks.hpp"
#include "../notes/cardList.hpp"
#include "../base/font.hpp"
#include <QTimer>
#include <QHeaderView>
#include <QKeyEvent>
#include <QTextBlock>
#include <QSplitter>

void BrowseScene::prevIdxTyp::reset() {
    idx = 0; max = 0;
}

BrowseScene::BrowseScene(Note* sel)
    : BaseScene(),
    info(""),
    newnote("New note (alt+enter)", Menues->FileMenu),
    delnote("Delete note (alt+delete)", Menues->FileMenu),
    clearIt("Clear notes", Menues->FileMenu),
    resetIt("Reset notes to default", Menues->FileMenu) {
        helpStr = &BROWSE_HELP;
        MG->changeBG("dirt");
        MG->removeGame();

        tree = getNoteTree(this);
        QWidget::connect(tree, &QTreeWidget::itemSelectionChanged, this, &BrowseScene::selectionChange);

        te = new MarkdownEdit(this);
        te->setDisabled(true);
        QObject::connect(te, &MarkdownEdit::textChanged, this, &BrowseScene::typed);
        QObject::connect(te, &MarkdownEdit::altEnter, this, &BrowseScene::newNote);
        QObject::connect(te, &MarkdownEdit::altDelete, this, &BrowseScene::delNote);

        side = SIDE_FRONT;
        prevIdx = {0, 0};
        prevIdxLabl = new QLabel(this);
        prevIdxLabl->setFont(getFont(1.5));
        auto* prevBtn = new SvgBtn(":/assets/back.svg");
        connect(prevBtn, &SvgBtn::clicked, this, [=](){
            if (prevIdx.idx > 1) {
                prevIdx.idx--;
                side = SIDE_FRONT;
                updatePrev();
            }
        });
        auto* nxtBtn = new SvgBtn(":/assets/forward.svg");
        connect(nxtBtn, &SvgBtn::clicked, this, [=](){
            if (prevIdx.idx < prevIdx.max) {
                prevIdx.idx++;
                side = SIDE_FRONT;
                updatePrev();
            }
        });

        siw = new SchdInfoWid(this);
        preview = new MarkdownEdit(this);
        preview->setButton(true);
        connect(preview, &MarkdownEdit::clicked, this, [=](){
            if (side == SIDE_FRONT) side = SIDE_BACK;
            else if (side == SIDE_BACK) side = SIDE_FRONT;
            updatePrev();
        });
        updatePrev();

        auto* hlay = new QHBoxLayout();
        hlay->addStretch();
        hlay->addWidget(prevIdxLabl);
        hlay->addStretch();
        hlay->addWidget(prevBtn);
        hlay->addWidget(nxtBtn);

        auto* vLay = new QVBoxLayout();
        vLay->addWidget(te, 2);
        vLay->addWidget(siw);
        vLay->addLayout(hlay);
        vLay->addWidget(preview, 1);
        auto* vLayW = new QWidget(this);
        vLayW->setLayout(vLay);

        auto* mSplit = new QSplitter(Qt::Horizontal, this);
        mSplit->addWidget(tree);
        mSplit->addWidget(vLayW);
        mSplit->setHandleWidth(6);
        mSplit->setStretchFactor(0, 1);
        mSplit->setStretchFactor(1, 1);

        QTimer::singleShot(0, this, [=](){
            int w = this->width() / 2;
            if (w > 0) mSplit->setSizes(QList<int>{w, w});
        });

        auto* root = new QHBoxLayout(this);
        root->addWidget(mSplit);

        connect(&newnote, &QAction::triggered, this, &BrowseScene::newNote);
        connect(&delnote, &QAction::triggered, this, &BrowseScene::delNote);
        connect(&clearIt, &QAction::triggered, this, [this](){
            if (doubleCheck("delete all your notes")) {
                for (auto* n : notesL) delete n;
                notesL.clear();
                updateNoteCards(); // Also updates globals and stuff
                tree->clear();
                selectionChange();
            }
        });
        connect(&resetIt, &QAction::triggered, this, [this](){
            if (doubleCheck("delete all your notes and replace them with the defaults")) {
                loadDefaultNotes();
                resetNoteTree(tree);
                selectionChange();
            }
        });
        updateInfo();

        if (sel != nullptr) {
            for (int i = 0; i < tree->topLevelItemCount(); ++i) {
                QTreeWidgetItem* item = tree->topLevelItem(i);
                if (getNote(item) == sel) {
                    tree->setCurrentItem(item, 0);
                    break;
                }
            }
        }
    }
Note* BrowseScene::getSelNote() {
    return getNote(tree->selectedItems().first());
}

void BrowseScene::updateInfo() {
    auto progs = getOverallProgress();
    float perc;
    if (progs.totCards > 0) {
        perc = std::round((
            progs.complete/progs.totCards
        ) * 10000)/100;
    } else {
        perc = 100;
    }
    info.setText(
        QString("%1 cards, %2% complete")
            .arg(progs.totCards)
            .arg(perc)
    );
}
void BrowseScene::updatePrev() {
    updateInfo();
    preview->setDisabled(true);
    if (tree->selectedItems().size() == 0) {
        prevIdxLabl->setText("Select a note");
        preview->setMarkdown("");
        siw->chngNote();
        return;
    }
    Note* n = getSelNote();
    int nOO = n->getNumCards();
    if (prevIdx.max != nOO) {
        int idx = prevIdx.idx;
        if (idx > nOO) {
            idx = nOO;
            side = SIDE_FRONT;
        }
        prevIdx.idx = idx;
        prevIdx.max = nOO;
    }
    if (nOO == 0) { prevIdx.idx = 0; }
    else {
        if (prevIdx.idx == 0)
            prevIdx.idx = 1;
    }
    if (n->error != "") {
        prevIdxLabl->setText("<span style='color:red;'>Encountered errors!</span>");
        preview->setMarkdown(n->error);
        side = SIDE_FRONT;
        return;
    }
    siw->chngNote(n);
    if (prevIdx.idx == 0) {
        prevIdxLabl->setText("No cards to preview!");
        QString txt = n->contents();
        for (auto* f : Feats) {
            txt = f->replacements(txt, SIDE_GETFC);
        }
        preview->setMarkdown(txt);
        return;
    }
    preview->setDisabled(false);
    QString sidestr;
    if (side == SIDE_FRONT) sidestr = " (front)";
    else if (side == SIDE_BACK) sidestr = " (back)";
    prevIdxLabl->setText(QString("Preview card %1/%2%3").arg(prevIdx.idx).arg(prevIdx.max).arg(sidestr));
    preview->setMarkdown(n->getFlashCard(prevIdx.idx-1)->getSide(side));
}

void BrowseScene::typed() {
    QSignalBlocker blocker(te);
    QList<QTreeWidgetItem*> selected = tree->selectedItems();
    if (selected.size() != 1) {
        te->setText("");
        te->setDisabled(true);
        prevIdx.reset();
        updatePrev();
        return;
    }
    Note* n = getSelNote();
    n->setContents(te->getMarkdown());
    writeNotes();

    updatePrev();
    updateItem(selected.first(), n);
    if (n->isGlobal()) updateAllItems(tree);
}
void BrowseScene::selectionChange() {
    QList<QTreeWidgetItem*> selected = tree->selectedItems();
    if (selected.isEmpty()) {
        te->setMarkdown("");
        te->setDisabled(true);
        prevIdx.reset();
        updatePrev();
        return;
    }

    Note* n = getSelNote();

    te->setMarkdown(n->contents());
    te->setDisabled(false);

    updatePrev();
}

void BrowseScene::newNote() {
    if (!checkValidDeck()) return;
    auto* n = new Note("");
    n->update();
    notesL.push_back(n);
    auto* it = addToTree(tree, n);
    tree->setCurrentItem(it);
    writeNotes();

    tree->sortItems(
        tree->header()->sortIndicatorSection(),
        tree->header()->sortIndicatorOrder()
    );
    te->setFocus();
}
void BrowseScene::delNote() {
    QList<QTreeWidgetItem*> selected = tree->selectedItems();
    if (selected.isEmpty())
        return;

    QTreeWidgetItem* item = selected.first();
    QSignalBlocker block(tree);

    QTreeWidgetItem* parent = item->parent();
    int idx; int maxlen;
    if (parent) {
        idx = parent->indexOfChild(item);
        maxlen = parent->childCount();
    } else {
        idx = tree->indexOfTopLevelItem(item);
        maxlen = tree->topLevelItemCount();
    }
    bool nxt = maxlen > 1;
    if (nxt && idx == maxlen-1) idx--;

    Note* data = getNote(item);
    bool glob = data->isGlobal();
    notesL.erase(std::remove(notesL.begin(), notesL.end(), data), notesL.end());
    delete data;
    updateNoteCards(); // Also updates globals and stuff
    writeNotes();
    QTreeWidgetItem* nxtSel;
    if (parent) {
        parent->removeChild(item);
        if (nxt) nxtSel = parent->child(idx);
    } else if (QTreeWidget *tree = item->treeWidget()) {
        tree->takeTopLevelItem(tree->indexOfTopLevelItem(item));
        if (nxt) nxtSel = tree->topLevelItem(idx);
    }

    if (nxt) {
        tree->setCurrentItem(nxtSel);
    } else {
        tree->clearSelection();
    }
    selectionChange();
    delete item;
    if (glob) updateAllItems(tree);
}

bool BrowseScene::keyEv(QKeyEvent* event) {
    if (MG->handleEv(event)) return true;
    if (event->modifiers() & Qt::ControlModifier && event->modifiers() & Qt::ShiftModifier) {
        int key = event->key();
        if (key == Qt::Key_Return) {
            newNote();
            return true;
        }
        if (key == Qt::Key_Delete || key == Qt::Key_Backspace) {
            delNote();
            return true;
        }
    }
    return false;
}
