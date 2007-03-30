result := RunDialogEntry("Short text",
                         rec(buttons:=DIALOG_BUTTONS_OK_CANCEL,
                             default:=DIALOG_RESPONSE_OK,
                             title:="An Entry"));
if result[1] = DIALOG_RESPONSE_OK then
  RunDialogMessage(DIALOG_INFO, Concatenation("You entered:\n", result[2]));
else
  RunDialogMessage(DIALOG_INFO, "Cancel was pressed");
fi;

RunDialogText("Some text,\nand some more",
              rec(buttons:=DIALOG_BUTTONS_CLOSE));
