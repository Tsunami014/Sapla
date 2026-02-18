#include <vector>
#include <QString>

extern std::vector<QString> decks;
extern QString curDeck;
bool checkValidDeck();
int deckIdx();

void changeDeck(QString newname);
int renameDeck(QString newname);
int deleteDeck(bool ask = true);

enum DeckCopyType {
    Copy_Regular,
    Copy_NoSchedule
};
bool copyDeck(DeckCopyType typ = Copy_Regular, bool tmp = false);
