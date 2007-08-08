RunDialogMessage(DIALOG_INFO,
                 "Here's some important information",
                 rec(title:="Info Dialog"));
RunDialogMessage(DIALOG_QUESTION,
                 "Do something?",
                 "Doing something may have some result",
                 rec(title:="Question Dialog",
                     buttons:=DIALOG_BUTTONS_YES_NO));
RunDialogMessage(DIALOG_WARNING,
                 "Warning!",
                 "Are you really sure you want to do it?",
                 rec(title:="Warning Dialog",
                     buttons:=DIALOG_BUTTONS_OK_CANCEL,
                     default:=DIALOG_RESPONSE_CANCEL));
RunDialogMessage(DIALOG_ERROR,
                 "Oops!",
                 "Something bad happened",
                 rec(title:="Error Dialog"));
