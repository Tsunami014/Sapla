#pragma once
#include <QString>

const QString APP_HELP = 
"<h1>Sapla help</h1>"
    "<h2>How the app works</h2>"
        "<p>This app is a flashcard app where you complete flashcards in order to grow a tree.</p>"
        "<p>You'll get stuff as you go along, but if not you can always check the 'This screen' in the 'Help' menu</p>"
;

const QString BROWSE_HELP = 
"<h2>Keybinds</h2>"
    "<ul>"
        "<li>&lt;Ctrl+Delete&gt; to delete currently selected item</li>"
        "<li>&lt;Esc&gt; to go back</li>"
    "</ul>"
"<h2>How to use</h2>"
    "<p>You can select notes on the left and edit their contents on the right</p>"
    "<p>Press the 'New note' button in the title to make a new note</p>"
    "<p>Notes can contain things in them (see below) that create cards."
        "By default, notes create 0 cards so don't expect them to do stuff unless you add things to them</p>"
"<h2>Things in notes</h2>"
    "<ul>"
        "<li>Basic markdown is supported, with multiple different bullet point types!</li>"
        "<li>--- makes the text above this be the front of a flashcard and the text below on the back side</li>"
        "<li>=== does the same thing, except it creates 2 flashcards instead of 1, with the second being a reverse</li>"
    "</ul>"
;

const QString PLUGVIEW_HELP = 
"<p>Select plugins to view information about them and disable them.</p>"
"<p>Press the 'Load plugin' button to load one or more plugins from your hard drive."
    "This gives you the option to move them or to copy them (move gets rid of the original file, copy keeps the original).</p>"
"<h2>Status types</h2>"
    "<ul>"
        "<li>🤎 - the plugin is disabled</li>"
        "<li>💔 - the plugin failed to load</li>"
        "<li>💖 - the plugin loaded successfully!</li>"
    "</ul>"
;

