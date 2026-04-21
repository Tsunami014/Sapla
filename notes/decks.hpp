#include <vector>
#include <QString>

const QString curDeckSetting = "curDeck";

extern std::vector<QString> decks;
extern QString curDeck;
bool checkValidDeck();
int deckIdx();

void changeDeck(QString newname, bool createNew = true);
int renameDeck(QString newname);
int deleteDeck(bool ask = true);

enum DeckCopyType {
    Copy_Regular,
    Copy_NoSchedule
};
bool copyDeck(DeckCopyType typ = Copy_Regular, bool tmp = false);
bool loadNewDeck(QString conts, QString name);
