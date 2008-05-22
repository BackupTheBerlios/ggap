/*
 *   mdprefs.h
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@math.tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef MD_PREFS_H
#define MD_PREFS_H

#include <mooui/mdmanager.h>
#include <mooutils/mooprefs.h>

G_BEGIN_DECLS


const char *md_manager_setting  (MdManager  *mgr,
                                 const char *setting_name);

#define MD_PREFS_TITLE_FORMAT             "window_title"
#define MD_PREFS_TITLE_FORMAT_NO_DOC      "window_title_no_doc"
#define MD_PREFS_USE_TABS                 "use_tabs"
#define MD_PREFS_OPEN_NEW_WINDOW          "open_new_window"

#define MD_PREFS_SAVE_SESSION             "save_session"

#define MD_PREFS_AUTO_SAVE                "auto_save"
#define MD_PREFS_AUTO_SAVE_INTERVAL       "auto_save_interval"
#define MD_PREFS_MAKE_BACKUPS             "make_backups"

#define MD_PREFS_LAST_DIR                 "last_dir"
#define MD_PREFS_DIALOGS                  "dialogs"
#define MD_PREFS_DIALOG_OPEN              MD_PREFS_DIALOGS "/open"
#define MD_PREFS_DIALOG_SAVE              MD_PREFS_DIALOGS "/save"
#define MD_PREFS_DIALOGS_OPEN_FOLLOWS_DOC "open_dialog_follows_doc"


G_END_DECLS

#endif /* MD_PREFS_H */
