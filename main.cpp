#include "core.hpp"
/* Here's the idea:
 * 3. After, remove the different cards and rewrite that part. There should only be one text input for adding cards.
 * 4. Then, change the markdown view to handle cards. It should return multiple texts broken up by --- when used in cards (toggle into return markdown as html func) but have a <hr> if not
 * 5. Make the different stuff (below). ENSURE EACH ONE IS HIGHLIGHTED.
 * 6. BUGFIX. Try your hardest to break things and find what impacts use, then fix that!
IF HAVE TIME:
 * 7. Add the buttons for the stuff below
*/
/* Other things to do
 * More shortcuts (shortcuts in help menu)
 * Menubar for browse scene 'new card' is a button, not a menu
 * Instead of appending a heart emoji to games, have the emoji in another column. Also have games return their emojis, but have the default be a sparkiling heart.
 * Add some buttons. These need to have tooltips on hover stating what they do and keyboard shortcuts to activate them, and are placed above and below the text input; and they add common stuff e.g. the ---
 * Errors as visuals
 * Error count logs on new scene entry that is not the browseScn
 * Keybinds for the buttons that do the e.g. --- and === and {{text}}
 * Extensions can add extra functions that run during markdown->html
*/
/* Stuff
 * --- separates front side from back side
 * === separates front side from back side but for double sided cards
 * {{text}} auto close passage
 * {{c1:text}} manual close passage
 * {{c1:text??hint}} close passage with hint (c1 is optional for manual)
 * [[front:back]] hidden text (front text only appears on front, likewise with back) ([[:hi]] is ok)
 * #true/false# for a <a> or <b> question e.g. true/false
Markdown elements
 * [link](site) and ![img](site) for links and images
 * %%latex%% for latex equations. Also find one for making multiline latex. Probs just use the format in Anki
 * ==highlight==, ~~strikethrough~~, `code`, ```lang\ncode blocks\n```
These get moved to the top of the card no matter where they're placed
 * @tag:tagname@ for tags
 * @priority:num@ for priority
 * <<infoType:info>> for information such as scheduling
*/

/*
 * Multiple question types randomly selected (e.g. yes/no and multiple choice)
 * Some question types cannot show the front with the back and other flags to be aware of
 * Do not log every successful game load, only log amount of successes and each error
 * Have a small text view underneath main text edit for a 'cheat sheet' - containing everything in the 'stuff' and what it does, filtering based on what you're typing (e.g. typing `=` will only bring up `===`, but typing `<<` can bring up multiple things)
*/
int main(int argc, char *argv[]) {
    return runApp(argc, argv);
}
