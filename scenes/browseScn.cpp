#include "browseScn.hpp"
#include "../core.hpp"
#include "../help.hpp"
#include "../log.hpp"
#include "../notes/noteTree.hpp"
#include "../notes/getNotes.hpp"
#include "../notes/decks.hpp"
#include "../notes/cardList.hpp"
#include "../base/font.hpp"
#include "../base/seedrng.hpp"
#include <QTimer>
#include <QHeaderView>
#include <QKeyEvent>
#include <QTextBlock>
#include <QSplitter>

const QString MODULE = "BrowseScene";

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

        filter = new QLineEdit(this);
        filter->setFont(getFont(1.2));
        filter->setPlaceholderText("Filter...");
        QObject::connect(filter, &QLineEdit::textChanged, this, &BrowseScene::filterChanged);

        te = new MarkdownEdit(this);
        QObject::connect(te, &MarkdownEdit::textChanged, this, &BrowseScene::typed);
        QObject::connect(te, &MarkdownEdit::altEnter, this, &BrowseScene::newNote);
        QObject::connect(te, &MarkdownEdit::altDelete, this, [this](){delNote();});

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
        preview->setProperty("Disabled", true);
        connect(preview, &MarkdownEdit::clicked, this, [=](){
            if (side == SIDE_FRONT) side = SIDE_BACK;
            else if (side == SIDE_BACK) side = SIDE_FRONT;
            updatePrev();
        });
        updatePrev();

        auto* vLay = new QVBoxLayout();
        vLay->addWidget(filter);
        vLay->addWidget(tree);
        auto* vLayW = new QWidget(this);
        vLayW->setLayout(vLay);

        auto* hlay = new QHBoxLayout();
        hlay->addStretch();
        hlay->addWidget(prevIdxLabl);
        hlay->addStretch();
        hlay->addWidget(prevBtn);
        hlay->addWidget(nxtBtn);

        auto* vLay2 = new QVBoxLayout();
        vLay2->addWidget(te, 2);
        vLay2->addWidget(siw);
        vLay2->addLayout(hlay);
        vLay2->addWidget(preview, 1);
        auto* vLayW2 = new QWidget(this);
        vLayW2->setLayout(vLay2);

        auto* mSplit = new QSplitter(Qt::Horizontal, this);
        mSplit->addWidget(vLayW);
        mSplit->addWidget(vLayW2);
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
        filterChanged(); // Also runs updateInfo

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
    QString filtr = filter->text();
    _overallProgr progr;
    if (filtr.isEmpty()) {
        progr = getOverallProgress();
    } else {
        progr = getOverallFilteredProgress(filtr);
    }
    float perc;
    if (progr.totCards > 0) {
        perc = std::round((
            progr.complete/progr.totCards
        ) * 10000)/100;
        info.setText(
            QString("%1 cards, %2% complete")
                .arg(progr.totCards)
                .arg(perc)
        );
    } else {
        info.setText("0 cards");
    }
}
void BrowseScene::updatePrev(bool refresh) {
    if (refresh) {
        seed = getSeed();
    }
    updateInfo();
    auto setDisabledness = [&](bool disabled) {
        preview->setProperty("Disabled", disabled);
        preview->style()->unpolish(preview);
        preview->style()->polish(preview);
        preview->update();
    };
    if (tree->selectedItems().size() == 0) {
        prevIdxLabl->setText("Select a note");
        preview->setMarkdown("");
        siw->chngNote();
        setDisabledness(true);
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
        setDisabledness(true);
        return;
    }
    siw->chngNote(n);
    if (prevIdx.idx == 0) {
        prevIdxLabl->setText("No cards to preview!");
        preview->setMarkdown(n->preview());
        setDisabledness(true);
        return;
    }
    preview->setProperty("Disabled", false);
    QString sidestr;
    if (side == SIDE_FRONT) sidestr = " (front)";
    else if (side == SIDE_BACK) sidestr = " (back)";
    prevIdxLabl->setText(QString("Preview card %1/%2%3").arg(prevIdx.idx).arg(prevIdx.max).arg(sidestr));
    setSeed(seed);
    preview->setMarkdown(n->getFlashCard(prevIdx.idx-1)->getSide(side));
    setDisabledness(false);
}

void BrowseScene::filterChanged() {
    filterTree(tree, filter->text());
    updateInfo();
}

void BrowseScene::typed() {
    QSignalBlocker blocker(te);
    QList<QTreeWidgetItem*> selected = tree->selectedItems();
    QString md = te->getMarkdown();
    if (md == "") {
        if (selected.size() > 0) {
            delNote(true);
        }
        return;
    }
    Note* n;
    QTreeWidgetItem* it;
    bool updAll;
    if (selected.size() == 0) {
        n = new Note(md);
        n->update();
        notesL.push_back(n);
        it = addToTree(tree, n);
        tree->setCurrentItem(it);
        updAll = n->isGlobal();
    } else {
        n = getSelNote();
        updAll = n->setContents(md);
        it = selected.first();
    }
    writeNotes();

    updatePrev(true);
    if (updAll) {
        updateAllItems(tree);
    } else {
        updateItem(it, n);
    }
    if (n->isGlobal()) updateAllItems(tree);
}
void BrowseScene::selectionChange() {
    QList<QTreeWidgetItem*> selected = tree->selectedItems();
    filterTree(tree, filter->text());
    if (selected.isEmpty()) {
        te->setMarkdown("");
        prevIdx.reset();
        updatePrev(true);
        return;
    }
    selected.first()->setHidden(false);
    Note* n = getSelNote();
    te->setMarkdown(n->contents());
    updatePrev(true);
}

void BrowseScene::newNote() {
    if (!checkValidDeck()) return;
    tree->clearSelection();
    writeNotes();

    tree->sortItems(
        tree->header()->sortIndicatorSection(),
        tree->header()->sortIndicatorOrder()
    );
    te->setFocus();
}
void BrowseScene::delNote(bool desel) {
    QList<QTreeWidgetItem*> selected = tree->selectedItems();
    if (selected.isEmpty())
        return;

    QTreeWidgetItem* item = selected.first();
    QSignalBlocker block(tree);

    Note* data = getNote(item);
    bool glob = data->isGlobal();
    auto it = std::find(notesL.begin(), notesL.end(), data);
    if (it != notesL.end()) {
        notesL.erase(it);
    } else {
        Log::Error(MODULE) << "Was unable to delete note!";
        return;
    }
    delete data;
    updateNoteCards(); // Also updates globals and stuff
    writeNotes();
    QTreeWidgetItem* nxtSel = nullptr;
    tree->clearSelection();
    if (desel) {
        tree->setCurrentItem(nullptr);
    }
    delete item;
    selectionChange();
    if (glob) updateAllItems(tree);
}

bool BrowseScene::keyEv(QKeyEvent* event) {
    int key = event->key();
    if (key == Qt::Key_Escape && tree->selectedItems().length() > 0) {
        tree->clearSelection();
        selectionChange();
        return true;
    }
    if (MG->handleEv(event)) return true;
    if (event->modifiers() & Qt::ControlModifier && event->modifiers() & Qt::ShiftModifier) {
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
