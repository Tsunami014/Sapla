#include <vector>
#include <QString>

extern std::vector<QString> decks;
extern QString curDeck;
bool checkValidDeck();

void changeDeck(QString newname);
int renameDeck(QString newname);
int deleteDeck();
int deckIdx();
