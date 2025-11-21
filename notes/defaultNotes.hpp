#include <QStringList>

QStringList defNotes = {
    "What is **Australia**?\n"
    "    *  (This is important to know)\n"
    "---\n"
    "A country",

    "What *is* the meaning of life?\n"
    "---\n"
    "42",

    "What is 1 + 1?\n"
     "---\n"
     "- 2\n"
     "+ You should know this!",

    "[[What happened i:I]]n 2025[[?:]]\n"
     "===\n"
     "[[When was t:T]]his app was made[[?:]]",

    "%|debug\n"
     "This is the [[front:back]]\n"
     "|%\n"
     "Here is some text in a note but not in a card so you'll never see it when playing",

    "[[What is this:For]] testing[[?\\n(*hint\\: it's below*):.]]\n"
     "%%debug%%\n"
     "===\n"
     "Note templates[[ play what role in this card?:.]]\n"
     "%%debug%%",

    "Testing backslashes:\n"
     "[\\[broken via backslash]]\n"
     "\\* broken again\n"
     "---\n"
     "Actual backslash: \\\\\n"
     "Fake newline: \\n"
     "<- fake newline (try typing here)\n"
     "Rules for backslashes: place it wherever it works (usually on the *inside* not the outside)",
};
