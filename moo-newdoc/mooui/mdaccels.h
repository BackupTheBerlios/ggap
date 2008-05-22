#ifndef MD_ACCELS_H
#define MD_ACCELS_H

#include <mooutils/mooaccel.h>

#define MD_ACCEL_NEW          MOO_ACCEL_NEW
#define MD_ACCEL_OPEN         MOO_ACCEL_OPEN
#define MD_ACCEL_OPEN_URI     MOO_ACCEL_CTRL "<Shift>O"
#define MD_ACCEL_SAVE         MOO_ACCEL_SAVE
#define MD_ACCEL_SAVE_AS      MOO_ACCEL_SAVE_AS
#define MD_ACCEL_CLOSE        MOO_ACCEL_CLOSE
#define MD_ACCEL_PAGE_SETUP   MOO_ACCEL_PAGE_SETUP
#define MD_ACCEL_PRINT        MOO_ACCEL_PRINT

#define MD_ACCEL_NEW_WINDOW MOO_ACCEL_CTRL "<Shift>N"

#ifndef GDK_WINDOWING_QUARTZ

#define MD_ACCEL_SWITCH_TO_TAB "<Alt>"
#define MD_ACCEL_PREV_TAB "<Alt>Left"
#define MD_ACCEL_NEXT_TAB "<Alt>Right"

#else /* GDK_WINDOWING_QUARTZ */

#define MD_ACCEL_SWITCH_TO_TAB "<Meta>"
#define MD_ACCEL_PREV_TAB "<Meta>Left"
#define MD_ACCEL_NEXT_TAB "<Meta>Right"

#endif /* GDK_WINDOWING_QUARTZ */

/* no such shortcut on Mac */
#define MD_ACCEL_RELOAD "F5"
/* XXX Shift-Command-W is Close File on Mac */
#define MD_ACCEL_CLOSE_ALL MOO_ACCEL_CTRL "<Shift>W"

#endif /* MD_ACCELS_H */
