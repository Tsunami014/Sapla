#pragma once
#include <QString>

const QString APP_HELP = 
"<h1>Sapla help</h1>"
    "<h2>How the app works</h2>"
        "<p>This app is a flashcard app where you complete flashcards in order to grow a tree.</p>"
        "<p>You'll get it as you go along, but if not you can always check the help for specifc places by the <b>Help->This screen</b> menu.</p>"
;

const QString BROWSE_HELP = 
"<h2>Keybinds</h2>"
    "<ul>"
        "<li>&lt;Ctrl+Delete&gt; to delete currently selected item</li>"
        "<li>&lt;Esc&gt; to go back</li>"
    "</ul>"
"<h2>Menu items</h2>"
    "<ul>"
        "<li>Under the <b>File</b> menu there are items for resetting/deleting your notes</li>"
        "<li>The <b>New Note</b> menu button creates a new note</li>"
    "</ul>"
"<h2>Notes and cards</h2>"
    "<p><i>Notes</i> are blocks of information and <i>cards</i> are a way to view that information.</p>"
    "<p>Cards are created when specific features are added to cards.</p>"
    "<p>For example, you can split your note into a front and a back side with <b>---</b> to separate it into a front and back side</p>"
    "<p>There are some sample cards for you to look at for help.</p>"
"<h2>How to use</h2>"
    "<p>You can select notes on the left and edit their contents on the right</p>"
    "<p>Press the 'New note' button in the title to make a new note</p>"
    "<p>Notes can contain things in them that create cards or that affect the text."
        "By default, notes create 0 cards so don't expect them to do stuff unless you add things to them</p>"
    "<p>Things you can add to your notes are in a layout above the note. "
        "Pressing the arrow expands/contracts to see extra. "
        "Pressing the settings button allows you to see all of them at once, and clicking on each will reveal details and help for if you are unsure of what they do.</p>"
"<h2>Note templates</h2>"
    "<p>Note templates are used in 2 phases - defining them, then using them.</p>"
    "<p>There are some examples of note template uses in the example cards</p>"
    "<h3>Defining note templates</h3>"
        "<p>Note templates are contained within <b>%|</b> and <b>|%</b> and they are made of a header and contents."
            "All extra whitespace around the note templates will be removed. (if you need it use a backslash)</p>"
        "<p>The header is separated from the contents by either a space, <b>|</b> or newline."
            "This is the name to specify when using the note template.</p>"
        "<p>The body will be directly substituted into the text, but can contain arguments via a % and a number, e.g. <b>%1</b>."
            "See below for how they are used when using the template.</p>"
    "<h3>Using note templates</h3>"
        "<p>Note templates uses are surrounded with <b>%%</b>."
            "The arguments are separated by either a <b>|</b> or newline (but the first you can use a space for as well)</p>"
        "<p>The first thing is the name of the card to use, then the arguments to be substituted in order.</p>"
;

const QString PLUGVIEW_HELP = 
"<p>Select plugins to view information about them and disable them.</p>"
"<p>Press the <b>Load plugin</b> menu item to load one or more plugins from your hard drive."
    "This gives you the option to move them or to copy them (move gets rid of the original file, copy keeps the original).</p>"
"<h2>Status types</h2>"
    "<ul>"
        "<li>🤎 - the plugin is disabled</li>"
        "<li>💔 - the plugin failed to load</li>"
        "<li>💖 - the plugin loaded successfully!</li>"
    "</ul>"
;

