#include <QStringList>

QStringList defNotes = {
    "What is **Australia**?\n"
    "    *  (This is important to know)\n"
    "---\n"
    "A ~~city~~ ==country==",

    "What _is_ the meaning of life?\n"
    "---\n"
    "42",

    "What is `1 + 1`?\n"
     "---\n"
     "- ==2==\n"
     "+ *You should know this*!",

    "[[What happened i:I]]n _2025_[[?:]]\n"
     "===\n"
     "[[When was t:T]]his app was made[[?:]]",

    "%|debug\n"
     "This is the [[front:back]]\n"
     "|%\n"
     "Here is some text in a note but not in a card so you'll never see it when playing "
     "because it doesn't have any features that create a card such as `---`.\n"
     "The fact there's a note template here makes no difference.\n"
     "@tag:test@",

    "[[What is this:For]] testing[[?\\n(*hint\\: it's below*):.]]\n"
     "%%debug%%\n"
     "===\n"
     "Note templates[[ play what role in this card?:.]]\n"
     "%%debug%%\n"
     "@tag:test@",

    "Can you have note templates with regular cards?\n"
     "%| highlight **==%1==** |%\n"
     "---\n"
     "%%highlight Yes!%%"
     "@tag:test@",

    "Testing:\n"
     "[\\[broken via backslash]]\n"
     "\\* broken again\n"
     "---\n"
     "Actual backslash: \\\\\n"
     "Fake newline: \\n"
     "<- fake newline (try typing here)\n"
     " @ tag : test ,use\\,less @ ",
};
