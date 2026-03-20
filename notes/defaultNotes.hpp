#include <QStringList>

QStringList defNotes = QString(R"(

What _is_ **Australia**?
    *  (This is important to know)
---
A ~~city~~ ==country==
#!2


The meaning of life is {42}!
>  From a book or something
#!1


What is `1 + 1`?
---
- ==2==
> + *You should know this*!
#!2


[[What happened i::I]]n _2025_[[?::]]
===
[[When was t::T]]his app was made[[?::]]


[[Notice how this is hidden and both the group1s will always be next to each other? (The groups can be called anything)]]
1 + 1 = 5 - {3::group1??it's a number} (this is {easy::group1})
2 + 2 = {-1??it's a number} + 5
You're doing [[great||excellent||wonderful]]!


|=debug
This is the [[front::back]]
=|
Here is some text in a note but not in a card so you'll never see it when playing because it doesn't have any features that create a card such as `---`.
The fact there's a note template here makes no difference.
#test


[[What is this::For]] testing[[?\n(*hint\: it's below*)::.]]
||debug||
===
Note templates[[ play what role in this card?::.]]
> ||debug||
[[ Text that will never be seen! ]]
#test


Testing: (will generate no cards as there is no feature to do so)
[\[broken via backslash]]
\* broken again
Actual backslash: \\
Fake newline: \n<- fake newline (try typing here)
 #test #use\ less 


Can you have note templates with regular cards?
(Testing with a local template but it's the same with global templates)
|: highlight: **==%1==** :|
---
||highlight: Yes!||
#test


This is a mirror feature!
The text will be the same on both sides.
This is a great place to ..shuffle words.. or use the hidden==[[..?::feature!]]==
#test
///
This is a note too but with nothing in it. But because of the mirror feature it will show too!
#test
[[It also doesn't include blank notes, as seen by below (unecessary, only to demonstrate)]]
///

)").trimmed().split("\n\n\n");
