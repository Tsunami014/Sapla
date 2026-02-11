#include <QStringList>

QStringList defNotes = QString(R"(

What is **Australia**?
    *  (This is important to know)
---
A ~~city~~ ==country==
@prio: 2@


What _is_ the meaning of life?
---
42
>  From a book or something
@prio: 1@


What is `1 + 1`?
---
- ==2==
> + *You should know this*!
@prio: 2@


[[What happened i:I]]n _2025_[[?:]]
===
[[When was t:T]]his app was made[[?:]]


%|debug
This is the [[front:back]]
|%
Here is some text in a note but not in a card so you'll never see it when playing because it doesn't have any features that create a card such as `---`.
The fact there's a note template here makes no difference.
@tag:test@


[[What is this:For]] testing[[?\n(*hint\: it's below*):.]]
%%debug%%
===
Note templates[[ play what role in this card?:.]]
> %%debug%%
@tag:test@


Testing: (will generate no cards as there is no feature to do so)
[\[broken via backslash]]
\* broken again
Actual backslash: \\
Fake newline: \n<- fake newline (try typing here)
 @ tag : test ,use\,less @ 


Can you have note templates with regular cards?
%| highlight **==%1==** |%
---
%%highlight Yes!%%
@tag:test@

)").trimmed().split("\n\n\n");
