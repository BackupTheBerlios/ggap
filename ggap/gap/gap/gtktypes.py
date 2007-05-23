from defs import Arg, Function, ClassInfo, Constant

IsGtkTreeViewColumn = 'IsGtkTreeViewColumn'
IsGtkWidget = 'IsGtkWidget'
IsGtkWindow = 'IsGtkWindow'
IsGtkTextView = 'IsGtkTextView'
IsGtkTreeModel = 'IsGtkTreeModel'
IsGtkTooltips = 'IsGtkTooltips'
IsGtkAdjustment = 'IsGtkAdjustment'
IsGtkTextBuffer = 'IsGtkTextBuffer'
IsGtkToolItem = 'IsGtkToolItem'
IsGtkCellRenderer = 'IsGtkCellRenderer'
IsGtkFileFilter = 'IsGtkFileFilter'
IsGtkTreeSelection = 'IsGtkTreeSelection'
IsGdkScreen = 'IsGdkScreen'
IsGdkWindow = 'IsGdkWindow'
IsGdkPixbuf = 'IsGdkPixbuf'

IsObject = 'IsObject'
IsString = 'IsString'
IsInt = 'IsInt'
IsRat = 'IsRat'
IsList = 'IsList'
IsBool = 'IsBool'

def StringArg(name):
    return (IsString, name)
ArgName = StringArg('name')
ArgFilename = StringArg('filename')
ArgPath = StringArg('path')
ArgUri = StringArg('uri')
ArgText = StringArg('text')
ArgLabel = StringArg('label')
ArgTitle = StringArg('title')

def IntArg(name):
    return (IsInt, name)
ArgX = IntArg('x')
ArgY = IntArg('y')
ArgWidth = IntArg('width')
ArgHeight = IntArg('height')
ArgStart = IntArg('start')
ArgEnd = IntArg('end')
ArgPos = IntArg('position')
def BoolArg(name):
    return (IsBool, name)
ArgSetting = BoolArg('setting')
ArgActive = BoolArg('active')

def FloatArg(name=None):
    if name:
        return (IsRat, name)
    else:
        return IsRat

class TreePathArg(Arg):
    def __init__(self, name='path'):
        Arg.__init__(self, type='IsObject', name=name,
                     transform='GTK_TREE_PATH_G2P(%(arg)s)')
class TreeIndArg(Arg):
    def __init__(self, name='index'):
        Arg.__init__(self, type='IsInt', name=name,
                     transform='GTK_TREE_INDEX_G2P(%(arg)s)')
class ListIndexArg(Arg):
    def __init__(self, name='index'):
        Arg.__init__(self, type='IsInt', name=name,
                     transform='%(arg)s - 1')

gboolean = 'IsBool'
gint = 'IsInt'
guint = 'IsInt'

GtkWindowType = IsInt
GtkWindowPosition = IsInt
GtkStateType = IsInt
GtkResizeMode = IsInt
GdkWindowTypeHint = IsInt
GtkShadowType = IsInt
GtkReliefStyle = IsInt
GtkDialogFlags = IsInt
GtkPositionType = IsInt
GtkPolicyType = IsInt
GtkCornerType = IsInt
GtkButtonBoxStyle = IsInt
GtkOrientation = IsInt
GtkToolbarStyle = IsInt
GtkIconSize = IsInt
GtkFileChooserAction = IsInt
GtkSelectionMode = IsInt
GtkSortType = IsInt
GtkTreeViewColumnSizing = IsInt
GtkPackType = IsInt

top_classes = []

class GdkPixbuf:
    __no_constructor__ = True
    __new__ = 'doc_stub'
top_classes.append(GdkPixbuf)
class GtkCellLayout:
    __no_constructor__ = True
    pack_start = Function(args=[IsGtkCellRenderer], opt_args=[BoolArg('expand')])
    pack_end = Function(args=[IsGtkCellRenderer], opt_args=[BoolArg('expand')])
    reorder = [IsGtkCellRenderer, ArgPos]
    clear = []
    add_attribute = [IsGtkCellRenderer, StringArg('property'), IntArg('column')]
#     set_attributes (GtkCellLayout *cell_layout, GtkCellRenderer *cell, ...);
    clear_attributes = [IsGtkCellRenderer]
top_classes.append(GtkCellLayout)
class GtkEditable:
    __no_constructor__ = True
    select_region = [ArgStart, ArgEnd]
    get_selection_bounds = []
    insert_text = Function(args=[ArgText], opt_args=[ArgPos])
    delete_text = [ArgStart, ArgEnd]
    get_chars = Function([ArgStart, ArgEnd], gap_name='GetText')
    cut_clipboard = []
    copy_clipboard = []
    paste_clipboard = []
    delete_selection = []
    set_position = [ArgPos]
    get_position = []
    set_editable = [ArgSetting]
    get_editable = []
top_classes.append(GtkEditable)
class GtkFileFilter:
    set_name = Function([ArgName], gap_name='SetFilterName')
    get_name = Function(gap_name='GetFilterName')
    add_mime_type = [StringArg('mime_type')]
    add_pattern = [StringArg('pattern')]
    add_pixbuf_formats = []
    get_needed = []
top_classes.append(GtkFileFilter)
class GtkFileChooser:
    __no_constructor__ = True
    set_action = [GtkFileChooserAction]
    get_action = []
    set_local_only = [ArgSetting]
    get_local_only = []
    set_select_multiple = [ArgSetting]
    get_select_multiple = []
    set_show_hidden = [ArgSetting]
    get_show_hidden = []
    set_do_overwrite_confirmation = [ArgSetting]
    get_do_overwrite_confirmation = []
    set_current_name = [ArgName]
    get_filename = []
    set_filename = [ArgFilename]
    select_filename = [ArgFilename]
    unselect_filename = [ArgFilename]
    select_all = []
    unselect_all = []
    get_filenames = []
    set_current_folder = [ArgPath]
    get_current_folder = []
    get_uri = []
    set_uri = [ArgUri]
    select_uri = [ArgUri]
    unselect_uri = [ArgUri]
    get_uris = []
    set_current_folder_uri = [ArgUri]
    get_current_folder_uri = []
    set_preview_widget = [IsGtkWidget]
    get_preview_widget = []
    set_preview_widget_active = [ArgActive]
    get_preview_widget_active = []
    set_use_preview_label = [ArgSetting]
    get_use_preview_label = []
    get_preview_filename = []
    get_preview_uri = []
    set_extra_widget = [IsGtkWidget]
    get_extra_widget = []
    add_filter = [IsGtkFileFilter]
    remove_filter = [IsGtkFileFilter]
    list_filters = []
    set_filter = [IsGtkFileFilter]
    get_filter = []
    add_shortcut_folder = [ArgPath]
    remove_shortcut_folder = [ArgPath]
    list_shortcut_folders = []
    add_shortcut_folder_uri = [ArgUri]
    remove_shortcut_folder_uri = [ArgUri]
    list_shortcut_folder_uris = []
top_classes.append(GtkFileChooser)
class GtkTreeModel:
    __no_constructor__ = True
    # TODO
top_classes.append(GtkTreeModel)
class GtkTreeSortable:
    __no_constructor__ = True
    __implements__ = ['GtkTreeModel']
    get_sort_column_id = []
    set_sort_column_id = [IntArg('sort_column_id'), (GtkSortType, 'order')]
top_classes.append(GtkTreeSortable)

class GObject:
    __no_constructor__ = True

    set_property = 'doc_stub'
    get_property = [ArgName]

    class GdkWindow:
        __no_constructor__ = True

    class GdkScreen:
        __no_constructor__ = True

        get_root_window = []
        get_display = []
        get_number = []
        get_width = []
        get_height = []
        get_width_mm = []
        get_height_mm = []
        get_toplevel_windows = []
        make_display_name = []
        get_n_monitors = []
        get_monitor_geometry = [IsInt]
        get_monitor_at_point = [IsInt, IsInt]
        get_monitor_at_window = [IsGdkWindow]

    class GdkDisplay:
        __new__ = [StringArg('display_name')]
        get_name = Function(gap_name='GetDisplayName')
        get_n_screens = []
        get_screen = [IsInt]
        get_default_screen = []
        beep = []
        close = []
        get_pointer = []
        get_window_at_pointer = []

    class GtkObject:
        __no_constructor__ = True

        destroy = []

        class GtkWidget:
            __no_constructor__ = True

            show = []
            show_now = []
            hide = []
            show_all = []
            hide_all = []
            activate = []
            reparent = [(IsGtkWidget, 'new_parent')]
            is_focus = []
            grab_focus = []
            grab_default = []
            set_name = Function([ArgName], gap_name='SetWidgetName')
            get_name = Function(gap_name='GetWidgetName')
            set_state = [GtkStateType]
            set_sensitive = [BoolArg('sensitive')]
            get_toplevel = []
            get_events = []
            get_pointer = []
            is_ancestor = [(IsGtkWidget, 'ancestor')]
#             modify_fg = [GtkStateType, GdkColor]
#             modify_bg = [GtkStateType, GdkColor]
#             modify_text = [GtkStateType, GdkColor]
#             modify_base = [GtkStateType, GdkColor]
            modify_font = Function(py_name='gap.widget_modify_font', is_meth=False, args=[IsGtkWidget, StringArg('font')])
            get_parent = []
            get_settings = []
            get_clipboard = []
            get_display = []
            get_root_window = []
            get_screen = []
            has_screen = []
            activate = []
            set_events = [IntArg('events')]
            add_events = [IntArg('events')]
            set_scroll_adjustments = [(IsGtkAdjustment, 'hadjustment'), (IsGtkAdjustment, 'vadjustment')]
            set_size_request = [IntArg('width'), IntArg('height')]

            class GtkContainer:
                __no_constructor__ = True

                add = Function([IsGtkWidget])
                remove = Function([IsGtkWidget])
#                 add_with_properties = [IsGtkWidget]
                get_resize_mode = []
                set_resize_mode = [GtkResizeMode]
                check_resize = []
#                 foreach           (GtkContainer *container,
#                                                              GtkCallback callback,
#                                                              gpointer callback_data);
#                 foreach_full      (GtkContainer *container,
#                                                              GtkCallback callback,
#                                                              GtkCallbackMarshal marshal,
#                                                              gpointer callback_data,
#                                                              GtkDestroyNotify notify);
                get_children = []
#                 child_type = []
#                 child_get         (GtkContainer *container,
#                                                              GtkWidget *child,
#                                                              const gchar *first_prop_name,
#                                                              ...);
#                 child_set         (GtkContainer *container,
#                                                              GtkWidget *child,
#                                                              const gchar *first_prop_name,
#                                                              ...);
                child_get_property = [IsGtkWidget, ArgName]
                child_set_property = [IsGtkWidget, ArgName, IsObject]
#                 forall            (GtkContainer *container,
#                                                              GtkCallback callback,
#                                                              gpointer callback_data);
                get_border_width = []
                set_border_width = [guint]
                get_focus_chain = []
                set_focus_chain = [IsList]
                unset_focus_chain = []

                class GtkBin:
                    __no_constructor__ = True

                    get_child = []

                    class GtkWindow:
                        __new__ = Function(opt_args=[GtkWindowType])
                        set_title = [ArgTitle]
                        set_resizable = [BoolArg('resizable')]
                        get_resizable = []
                        activate_focus = []
                        activate_default = []
                        set_modal = [BoolArg('modal')]
                        set_default_size = [IsInt, IsInt]
                        set_position = [GtkWindowPosition]
                        set_transient_for = [(IsGtkWindow, 'parent')]
                        set_destroy_with_parent = [ArgSetting]
                        set_screen = [IsGdkScreen]
                        is_active = []
                        has_toplevel_focus = []
                        get_focus = []
                        set_focus = [IsGtkWidget]
                        set_default = [IsGtkWidget]
                        present = []
                        present_with_time = [IsInt]
                        iconify = []
                        deiconify = []
                        stick = []
                        unstick = []
                        maximize = []
                        unmaximize = []
                        fullscreen = []
                        unfullscreen = []
                        set_keep_above = [ArgSetting]
                        set_keep_below = [ArgSetting]
                        set_decorated = [ArgSetting]
                        set_has_frame = [ArgSetting]
                        set_role = [StringArg('role')]
                        set_type_hint = [GdkWindowTypeHint]
                        set_skip_taskbar_hint = [ArgSetting]
                        set_skip_pager_hint = [ArgSetting]
                        set_urgency_hint = [ArgSetting]
                        set_accept_focus = [ArgSetting]
                        set_focus_on_map = [ArgSetting]
                        get_decorated = []
                        get_default_size = []
                        get_destroy_with_parent = []
                        get_has_frame = []
                        get_icon = []
                        get_icon_list = []
                        get_icon_name = []
                        get_mnemonic_modifier = []
                        get_modal = []
                        get_position = []
                        get_role = []
                        get_size = []
                        get_title = []
                        get_transient_for = []
                        get_type_hint = []
                        get_skip_taskbar_hint = []
                        get_skip_pager_hint = []
                        get_urgency_hint = []
                        get_accept_focus = []
                        get_focus_on_map = []
                        move = [IsInt, IsInt]
                        parse_geometry = [StringArg('geometry')]
                        reshow_with_initial_size = []
                        resize = [IsInt, IsInt]
                        set_icon_from_file = [ArgFilename]
                        set_icon_name = [StringArg('icon_name')]

                        class GtkDialog:
                            __new__ = Function(opt_args=[IsString, IsGtkWindow, GtkDialogFlags, IsList])
                            run = []
                            response = [IntArg('response')]
                            add_button = [IsString, IntArg('response')]
#                             add_buttons          (GtkDialog *dialog,
#                                                                          const gchar *first_button_text,
#                                                                          ...);
                            add_action_widget = [IsGtkWidget, IntArg('response')]
                            get_has_separator = []
                            set_default_response = [IntArg('response')]
                            set_has_separator = [ArgSetting]
                            set_response_sensitive = [IntArg('response'), BoolArg('sensitive')]
                            get_response_for_widget = [IsGtkWidget]

#                             set_alternative_button_order
#                                                                         (GtkDialog *dialog,
#                                                                          IsInt first_response_id,
#                                                                          ...);

                            class GtkAboutDialog:
                                get_name = Function(gap_name='GetApplicationName')
                                set_name = Function([ArgName], gap_name='SetApplicationName')
                                get_version = []
                                set_version = [StringArg('version')]
                                get_copyright = []
                                set_copyright = [StringArg('copyright')]
                                get_comments = []
                                set_comments = [StringArg('comments')]
                                get_license = []
                                set_license = [StringArg('license')]
                                get_wrap_license = []
                                set_wrap_license = [IntArg('setting')]
                                get_website = []
                                set_website = [StringArg('website')]
                                get_website_label = []
                                set_website_label = [StringArg('website_label')]
                                get_authors = []
                                set_authors = [IsList]
                                get_artists = []
                                set_artists = [IsList]
                                get_documenters = []
                                set_documenters = [IsList]
                                get_translator_credits = []
                                set_translator_credits = [StringArg('credits')]
                                get_logo = []
                                set_logo = [IsGdkPixbuf]
                                get_logo_icon_name = []
                                set_logo_icon_name = [StringArg('icon_name')]
#                                 void        gtk_show_about_dialog           (GtkWindow *parent,
#                                                                              const gchar *first_property_name,
#                                                                              ...);
                            class GtkColorSelectionDialog: pass
#                                 GtkWidget*  gtk_color_selection_dialog_new  (const gchar *title);
                            class GtkFileChooserDialog: pass
#                                 dialog_new     (const gchar *title,
#                                                                              GtkWindow *parent,
#                                                                              GtkFileChooserAction action,
#                                                                              const gchar *first_button_text,
#                                                                              ...);
#                                 dialog_new_with_backend
#                                                                             (const gchar *title,
#                                                                              GtkWindow *parent,
#                                                                              GtkFileChooserAction action,
#                                                                              const gchar *backend,
#                                                                              const gchar *first_button_text,
#                                                                              ...);
                            class GtkFontSelectionDialog: pass
#                                 GtkWidget*  gtk_font_selection_dialog_new   (const gchar *title);
#                                 GdkFont*    gtk_font_selection_dialog_get_font
#                                                                             (GtkFontSelectionDialog *fsd);
#                                 gchar*      gtk_font_selection_dialog_get_font_name
#                                                                             (GtkFontSelectionDialog *fsd);
#                                 IsBool    gtk_font_selection_dialog_set_font_name
#                                                                             (GtkFontSelectionDialog *fsd,
#                                                                              const gchar *fontname);
#                                 const gchar* gtk_font_selection_dialog_get_preview_text
#                                                                             (GtkFontSelectionDialog *fsd);
#                                 void        gtk_font_selection_dialog_set_preview_text
#                                                                             (GtkFontSelectionDialog *fsd,
#                                                                              const gchar *text);
                            class GtkMessageDialog:
#                                 GtkWidget*  gtk_message_dialog_new          (GtkWindow *parent,
#                                                                              GtkDialogFlags flags,
#                                                                              GtkMessageType type,
#                                                                              GtkButtonsType buttons,
#                                                                              const gchar *message_format,
#                                                                              ...);
#                                 GtkWidget*  gtk_message_dialog_new_with_markup
#                                                                             (GtkWindow *parent,
#                                                                              GtkDialogFlags flags,
#                                                                              GtkMessageType type,
#                                                                              GtkButtonsType buttons,
#                                                                              const gchar *message_format,
#                                                                              ...);
                                set_markup = [IsString]
#                                 format_secondary_text = [const gchar *message_format,
#                                                                              ...);
#                                 void        gtk_message_dialog_format_secondary_markup
#                                                                             (GtkMessageDialog *message_dialog,
#                                                                              const gchar *message_format,
#                                                                              ...);
                    class GtkPlug: pass
                    class GtkAlignment:
                        __new__ = Function(opt_args=[FloatArg('xalign'), FloatArg('yalign'), FloatArg('xscale'), FloatArg('yscale')])
                        set = Function(args=[FloatArg('xalign'), FloatArg('yalign'), FloatArg('xscale'), FloatArg('yscale')],
                                       gap_name='SetValues')
                        get_padding = []
                        set_padding = [IntArg('top'), IntArg('bottom'), IntArg('left'), IntArg('right')]
                    class GtkFrame:
#                         GtkWidget*  gtk_frame_new                   (const gchar *label);
                        set_label = [ArgLabel]
                        set_label_widget = [IsGtkWidget]
                        set_label_align = [FloatArg('xalign'), FloatArg('yalign')]
                        set_shadow_type = [GtkShadowType]
                        get_label = []
                        get_label_align = []
                        get_label_widget = []
                        get_shadow_type = []

                        class GtkAspectFrame:
#                             GtkWidget*  gtk_aspect_frame_new            (const gchar *label,
#                                                                          gfloat xalign,
#                                                                          gfloat yalign,
#                                                                          gfloat ratio,
#                                                                          IsBool obey_child);
                            set = Function(args=[FloatArg('xalign'), FloatArg('yalign'), FloatArg('ratio'), IsBool],
                                           gap_name='SetValues')

                    class GtkButton:
                        __new__ = 'doc_stub'
#                         GtkWidget*  gtk_button_new                  (void);
#                         GtkWidget*  gtk_button_new_with_label       (const gchar *label);
#                         GtkWidget*  gtk_button_new_with_mnemonic    (const gchar *label);
#                         GtkWidget*  gtk_button_new_from_stock       (const gchar *stock_id);
                        pressed = []
                        released = []
                        clicked = []
                        enter = []
                        leave = []
                        set_relief = [GtkReliefStyle]
                        get_relief = []
                        get_label = []
                        set_label = [ArgLabel]
                        get_use_stock = []
                        set_use_stock = [ArgSetting]
                        get_use_underline = []
                        set_use_underline = [ArgSetting]
                        set_focus_on_click = [ArgSetting]
                        get_focus_on_click = []
                        set_alignment = [FloatArg('xalign'), FloatArg('yalign')]
                        get_alignment = []
                        set_image = [IsGtkWidget]
                        get_image = []

                        class GtkToggleButton:
                            __new__ = Function(opt_args=[ArgLabel, BoolArg('use_underline')])
#                             GtkWidget*  gtk_toggle_button_new           (void);
#                             GtkWidget*  gtk_toggle_button_new_with_label
#                                                                         (const gchar *label);
#                             GtkWidget*  gtk_toggle_button_new_with_mnemonic
#                                                                         (const gchar *label);
                            set_mode = [BoolArg('draw_indicator')]
                            get_mode = []
                            toggled = []
                            get_active = []
                            set_active = [ArgActive]
                            get_inconsistent = []
                            set_inconsistent = [ArgSetting]

                            class GtkCheckButton:
                                __new__ = Function(opt_args=[ArgLabel, BoolArg('use_underline')])
#                                 GtkWidget*  gtk_check_button_new            (void);
#                                 GtkWidget*  gtk_check_button_new_with_label (const gchar *label);
#                                 GtkWidget*  gtk_check_button_new_with_mnemonic
#                                                                             (const gchar *label);

                                class GtkRadioButton: pass
#                                     GtkWidget*  gtk_radio_button_new            (GSList *group);
#                                     GtkWidget*  gtk_radio_button_new_from_widget
#                                                                                 (GtkRadioButton *group);
#                                     GtkWidget*  gtk_radio_button_new_with_label (GSList *group,
#                                                                                  const gchar *label);
#                                     GtkWidget*  gtk_radio_button_new_with_label_from_widget
#                                                                                 (GtkRadioButton *group,
#                                                                                  const gchar *label);
#                                     GtkWidget*  gtk_radio_button_new_with_mnemonic
#                                                                                 (GSList *group,
#                                                                                  const gchar *label);
#                                     GtkWidget*  gtk_radio_button_new_with_mnemonic_from_widget
#                                                                                 (GtkRadioButton *group,
#                                                                                  const gchar *label);
#                                     #define     gtk_radio_button_group
#                                     void        gtk_radio_button_set_group      (GtkRadioButton *radio_button,
#                                                                                  GSList *group);
#                                     GSList*     gtk_radio_button_get_group      (GtkRadioButton *radio_button);

                        class GtkColorButton:
#                             __new__ = Function(opt_args=[IsGdkColor])
#                             GtkWidget*  gtk_color_button_new            (void);
#                             GtkWidget*  gtk_color_button_new_with_color (const GdkColor *color);
#                             void        gtk_color_button_set_color      (GtkColorButton *color_button,
#                                                                          const GdkColor *color);
#                             void        gtk_color_button_get_color      (GtkColorButton *color_button,
#                                                                          GdkColor *color);
#                             void        gtk_color_button_set_alpha      (GtkColorButton *color_button,
#                                                                          guint16 alpha);
#                             guint16     gtk_color_button_get_alpha      (GtkColorButton *color_button);
#                             void        gtk_color_button_set_use_alpha  (GtkColorButton *color_button,
#                                                                          IsBool use_alpha);
#                             IsBool    gtk_color_button_get_use_alpha  (GtkColorButton *color_button);
                            set_title = [ArgTitle]
                            get_title = []

                        class GtkFontButton:
                            __new__ = Function(opt_args=[StringArg('font')])
                            set_font_name = [StringArg('font')]
                            get_font_name = []
                            set_show_style = [ArgSetting]
                            get_show_style = []
                            set_show_size = [ArgSetting]
                            get_show_size = []
                            set_use_font = [ArgSetting]
                            get_use_font = []
                            set_use_size = [ArgSetting]
                            get_use_size = []
                            set_title = [StringArg('window_title')]
                            get_title = []

                    class GtkItem:
                        __no_constructor__ = True
                        class GtkMenuItem:
                            __new__ = Function(opt_args=[ArgLabel, BoolArg('use_underline')])
                            set_right_justified = [ArgSetting]
                            set_submenu = [IsGtkWidget]
                            set_accel_path = [IsString]
                            remove_submenu = []
                            select = []
                            deselect = []
                            get_right_justified = []
                            get_submenu = []

                            # activate installed above
                            # activate = []

#                             toggle_size_request
#                                                                         (GtkMenuItem *menu_item,
#                                                                          IsInt *requisition);
#                             toggle_size_allocate
#                                                                         (GtkMenuItem *menu_item,
#                                                                          IsInt allocation);

                            class GtkCheckMenuItem:
                                __new__ = Function(opt_args=[ArgLabel, BoolArg('use_underline')])
                                get_active = []
                                set_active = [ArgActive]
                                set_show_toggle = [ArgSetting]
                                toggled = []
                                get_inconsistent = []
                                set_inconsistent = [ArgSetting]
                                set_draw_as_radio = [ArgSetting]
                                get_draw_as_radio = []

                                class GtkRadioMenuItem: pass
#                                     GtkWidget*  gtk_radio_menu_item_new         (GSList *group);
#                                     GtkWidget*  gtk_radio_menu_item_new_with_label
#                                                                                 (GSList *group,
#                                                                                  const gchar *label);
#                                     GtkWidget*  gtk_radio_menu_item_new_with_mnemonic
#                                                                                 (GSList *group,
#                                                                                  const gchar *label);
#                                     GtkWidget*  gtk_radio_menu_item_new_from_widget
#                                                                                 (GtkRadioMenuItem *group);
#                                     GtkWidget*  gtk_radio_menu_item_new_with_label_from_widget
#                                                                                 (GtkRadioMenuItem *group,
#                                                                                  const gchar *label);
#                                     GtkWidget*  gtk_radio_menu_item_new_with_mnemonic_from_widget
#                                                                                 (GtkRadioMenuItem *group,
#                                                                                  const gchar *label);
#                                     void        gtk_radio_menu_item_set_group   (GtkRadioMenuItem *radio_menu_item,
#                                                                                  GSList *group);
#                                     GSList*     gtk_radio_menu_item_get_group   (GtkRadioMenuItem *radio_menu_item);

                            class GtkImageMenuItem:
                                set_image = [IsGtkWidget]
                                get_image = []
#                                 GtkWidget*  gtk_image_menu_item_new         (void);
#                                 GtkWidget*  gtk_image_menu_item_new_from_stock
#                                                                             (const gchar *stock_id,
#                                                                              GtkAccelGroup *accel_group);
#                                 GtkWidget*  gtk_image_menu_item_new_with_label
#                                                                             (const gchar *label);
#                                 GtkWidget*  gtk_image_menu_item_new_with_mnemonic
#                                                                             (const gchar *label);

                            class GtkSeparatorMenuItem: pass
#                                 GtkWidget*  gtk_separator_menu_item_new     (void);
                            class GtkTearoffMenuItem: pass
#                                 GtkWidget*  gtk_tearoff_menu_item_new       (void);
                    class GtkComboBox:
                        __implements__ = ['GtkCellLayout']
                        __new__ = Function(py_name='gtk.combo_box_new_text')
#                         __new__ = Function(py_name='gtk.ComboBox', opt_args=['IsGtkTreeModel'])
#                         GtkWidget*  gtk_combo_box_new_text          (void);
                        get_wrap_width = []
                        set_wrap_width = [IsInt]
                        get_row_span_column = []
                        set_row_span_column = [IsInt]
                        get_column_span_column = []
                        set_column_span_column = [IsInt]
                        get_active = Function(ret_transform='%(retval)s + 1')
                        set_active = [ListIndexArg('active')]
#                         IsBool    gtk_combo_box_get_active_iter   (GtkComboBox *combo_box,
#                                                                      GtkTreeIter *iter);
#                         set_active_iter   (GtkComboBox *combo_box,
#                                                                      GtkTreeIter *iter);
                        get_model = []
                        set_model = [IsGtkTreeModel]
                        append_text = [ArgText]
                        insert_text = [ListIndexArg(), ArgText]
                        prepend_text = [ArgText]
                        remove_text = [ListIndexArg()]
                        get_active_text = []
                        popup = []
                        popdown = []
                        set_focus_on_click = [ArgSetting]
                        get_focus_on_click = []

                        class GtkComboBoxEntry:
                            __new__ = Function(py_name='gtk.combo_box_entry_new_text')
#                             GtkWidget*  gtk_combo_box_entry_new         (void);
#                             GtkWidget*  gtk_combo_box_entry_new_with_model
#                                                                         (GtkTreeModel *model,
#                                                                          IsInt text_column);
#                             GtkWidget*  gtk_combo_box_entry_new_text    (void);
                            set_text_column = [IsInt]
                            get_text_column = []

                    class GtkEventBox:
                        set_above_child = [ArgSetting]
                        get_above_child = []
                        set_visible_window = [ArgSetting]
                        get_visible_window = []

                    class GtkExpander:
                        __new__ = Function(opt_args=[ArgLabel])
                        set_expanded = [BoolArg('expanded')]
                        get_expanded = []
                        set_spacing = [IsInt]
                        get_spacing = []
                        set_label = [ArgLabel]
                        get_label = []
                        set_use_underline = [ArgSetting]
                        get_use_underline = []
                        set_use_markup = [ArgSetting]
                        get_use_markup = []
                        set_label_widget = [IsGtkWidget]
                        get_label_widget = []

                    class GtkHandleBox:
                        set_shadow_type = [GtkShadowType]
                        set_handle_position = [GtkPositionType]
                        set_snap_edge = [GtkPositionType]
                        get_handle_position = []
                        get_shadow_type = []
                        get_snap_edge = []

                    class GtkToolItem:
                        set_homogeneous = [ArgSetting]
                        get_homogeneous = []
                        set_expand = [ArgSetting]
                        get_expand = []
                        set_tooltip = [IsGtkTooltips, ArgText, IsString]
                        set_use_drag_window = [ArgSetting]
                        get_use_drag_window = []
                        set_visible_horizontal = [ArgSetting]
                        get_visible_horizontal = []
                        set_visible_vertical = [ArgSetting]
                        get_visible_vertical = []
                        set_is_important = [ArgSetting]
                        get_is_important = []
                        get_icon_size = []
                        get_orientation = []
                        get_toolbar_style = []
                        get_relief_style = []
                        retrieve_proxy_menu_item = []
                        get_proxy_menu_item = [IsString]
                        set_proxy_menu_item = [IsString, IsGtkWidget]
                        rebuild_menu = []

                        class GtkToolButton:
                            __new__ = Function(opt_args=[IsGtkWidget, IsString])
#                             GtkToolItem* gtk_tool_button_new            (GtkWidget *icon_widget,
#                                                                          const gchar *label);
#                             GtkToolItem* gtk_tool_button_new_from_stock (const gchar *stock_id);
                            set_label = [ArgLabel]
                            get_label = []
                            set_use_underline = [ArgSetting]
                            get_use_underline = []
                            set_stock_id = [StringArg('stock_id')]
                            get_stock_id = []
                            set_icon_name = [StringArg('icon_name')]
                            get_icon_name = []
                            set_icon_widget = [IsGtkWidget]
                            get_icon_widget = []
                            set_label_widget = [IsGtkWidget]
                            get_label_widget = []

                            class GtkMenuToolButton:
#                                 GtkToolItem* gtk_menu_tool_button_new       (GtkWidget *icon_widget,
#                                                                              const gchar *label);
#                                 GtkToolItem* gtk_menu_tool_button_new_from_stock
#                                                                             (const gchar *stock_id);
                                set_menu = [IsGtkWidget]
                                get_menu = []
                                set_arrow_tooltip = ['IsGtkTooltips', ArgText, IsString]

                            class GtkToggleToolButton:
                                __new__ = Function(opt_args=[ArgLabel])
                                set_active = [ArgActive]
                                get_active = []

                                class GtkRadioToolButton: pass
#                                     GtkToolItem* gtk_radio_tool_button_new      (GSList *group);
#                                     GtkToolItem* gtk_radio_tool_button_new_from_stock
#                                                                                 (GSList *group,
#                                                                                  const gchar *stock_id);
#                                     GtkToolItem* gtk_radio_tool_button_new_from_widget
#                                                                                 (GtkRadioToolButton *group);
#                                     GtkToolItem* gtk_radio_tool_button_new_with_stock_from_widget
#                                                                                 (GtkRadioToolButton *group,
#                                                                                  const gchar *stock_id);
#                                     GSList*     gtk_radio_tool_button_get_group (GtkRadioToolButton *button);
#                                     void        gtk_radio_tool_button_set_group (GtkRadioToolButton *button,
#                                                                                  GSList *group);

                        class GtkSeparatorToolItem:
                            set_draw = [ArgSetting]
                            get_draw = []

                    class GtkScrolledWindow:
                        __new__ = Function(opt_arg=[(IsGtkAdjustment, 'hadj'), (IsGtkAdjustment, 'vadj')])
                        get_hadjustment = []
                        get_vadjustment = []
                        get_hscrollbar = []
                        get_vscrollbar = []
                        set_policy = [GtkPolicyType, GtkPolicyType]
                        add_with_viewport = [IsGtkWidget]
                        set_placement = [GtkCornerType]
                        set_shadow_type = [GtkShadowType]
                        set_hadjustment = [IsGtkAdjustment]
                        set_vadjustment = [IsGtkAdjustment]
                        get_placement = []
                        get_policy = []
                        get_shadow_type = []

                    class GtkViewport:
                        __new__ = Function(opt_arg=[(IsGtkAdjustment, 'hadj'), (IsGtkAdjustment, 'vadj')])
                        get_hadjustment = []
                        get_vadjustment = []
                        set_hadjustment = [IsGtkAdjustment]
                        set_vadjustment = [IsGtkAdjustment]
                        set_shadow_type = [GtkShadowType]
                        get_shadow_type = []

                class GtkBox:
                    __no_constructor__ = True
                    pack_start = [IsGtkWidget, IsBool, IsBool, guint]
                    pack_end = [IsGtkWidget, IsBool, IsBool, guint]
                    pack_start_defaults = [IsGtkWidget]
                    pack_end_defaults = [IsGtkWidget]
                    get_homogeneous = []
                    set_homogeneous = [ArgSetting]
                    get_spacing = []
                    set_spacing = [IsInt]
                    reorder_child = [IsGtkWidget, IsInt]
                    query_child_packing = [(IsGtkWidget, 'child')]
                    set_child_packing = [(IsGtkWidget, 'child'), BoolArg('expand'), BoolArg('fill'), IntArg('padding'), GtkPackType]

                    class GtkButtonBox:
                        get_layout = []
                        get_child_size = []
                        get_child_ipadding = []
                        get_child_secondary = [IsGtkWidget]
                        set_layout = [GtkButtonBoxStyle]
                        set_child_secondary = [IsGtkWidget, IsBool]

                        class GtkHButtonBox:
                            pass
                        class GtkVButtonBox:
                            pass

                    class GtkVBox:
                        __new__ = Function(opt_agrs=[BoolArg('homogeneous'), IntArg('spasing')])

                        class GtkColorSelection:
                            pass
#                             void        gtk_color_selection_set_update_policy
#                                                                         (GtkColorSelection *colorsel,
#                                                                          GtkUpdateType policy);
#                             void        gtk_color_selection_set_has_opacity_control
#                                                                         (GtkColorSelection *colorsel,
#                                                                          IsBool has_opacity);
#                             IsBool    gtk_color_selection_get_has_opacity_control
#                                                                         (GtkColorSelection *colorsel);
#                             void        gtk_color_selection_set_has_palette
#                                                                         (GtkColorSelection *colorsel,
#                                                                          IsBool has_palette);
#                             IsBool    gtk_color_selection_get_has_palette
#                                                                         (GtkColorSelection *colorsel);
#                             guint16     gtk_color_selection_get_current_alpha
#                                                                         (GtkColorSelection *colorsel);
#                             void        gtk_color_selection_set_current_alpha
#                                                                         (GtkColorSelection *colorsel,
#                                                                          guint16 alpha);
#                             void        gtk_color_selection_get_current_color
#                                                                         (GtkColorSelection *colorsel,
#                                                                          GdkColor *color);
#                             void        gtk_color_selection_set_current_color
#                                                                         (GtkColorSelection *colorsel,
#                                                                          const GdkColor *color);
#                             guint16     gtk_color_selection_get_previous_alpha
#                                                                         (GtkColorSelection *colorsel);
#                             void        gtk_color_selection_set_previous_alpha
#                                                                         (GtkColorSelection *colorsel,
#                                                                          guint16 alpha);
#                             void        gtk_color_selection_get_previous_color
#                                                                         (GtkColorSelection *colorsel,
#                                                                          GdkColor *color);
#                             void        gtk_color_selection_set_previous_color
#                                                                         (GtkColorSelection *colorsel,
#                                                                          const GdkColor *color);
#                             IsBool    gtk_color_selection_is_adjusting
#                                                                         (GtkColorSelection *colorsel);
#                             IsBool    gtk_color_selection_palette_from_string
#                                                                         (const gchar *str,
#                                                                          GdkColor **colors,
#                                                                          IsInt *n_colors);
#                             gchar*      gtk_color_selection_palette_to_string
#                                                                         (const GdkColor *colors,
#                                                                          IsInt n_colors);
#                             GtkColorSelectionChangePaletteFunc gtk_color_selection_set_change_palette_hook
#                                                                         (GtkColorSelectionChangePaletteFunc func);
#                             void        (*GtkColorSelectionChangePaletteFunc)
#                                                                         (const GdkColor *colors,
#                                                                          IsInt n_colors);
#                             GtkColorSelectionChangePaletteWithScreenFunc gtk_color_selection_set_change_palette_with_screen_hook
#                                                                         (GtkColorSelectionChangePaletteWithScreenFunc func);
#                             void        (*GtkColorSelectionChangePaletteWithScreenFunc)
#                                                                         (GdkScreen *screen,
#                                                                          const GdkColor *colors,
#                                                                          IsInt n_colors);
#                             void        gtk_color_selection_set_color   (GtkColorSelection *colorsel,
#                                                                          gdouble *color);
#                             void        gtk_color_selection_get_color   (GtkColorSelection *colorsel,
#                                                                          gdouble *color);
                        class GtkFileChooserWidget:
                            __new__ = [GtkFileChooserAction]
                        class GtkFontSelection:
                            get_font_name = []
                            set_font_name = [StringArg('font')]
                            get_preview_text = []
                            set_preview_text = [ArgText]

                    class GtkHBox:
                        __new__ = Function(opt_agrs=[BoolArg('homogeneous'), IntArg('spasing')])

                        class GtkFileChooserButton: pass
                            # TODO
                        class GtkStatusbar: pass
                            # TODO

                class GtkFixed:
                    # TODO
                    pass
                class GtkPaned:
                    class GtkHPaned:
                        # TODO
                        pass
                    class GtkVPaned:
                        # TODO
                        pass
                class GtkIconView:
                    # TODO
                    pass
                class GtkLayout:
                    # TODO
                    class FooCanvas:
                        root = Function(gap_name='GetRoot')
                        set_scroll_region = [FloatArg('x1'), FloatArg('y1'), FloatArg('x2'), FloatArg('y2')]
                        get_scroll_region = []
                        set_pixels_per_unit = [FloatArg('n')]
                        set_center_scroll_region = [BoolArg('center_scroll_region')]
                        scroll_to = [IntArg('cx'), IntArg('cy')]
                        get_scroll_offsets = []
                        update_now = []
                        get_item_at = [FloatArg('x'), FloatArg('y')]
                        w2c_rect = [FloatArg('x1'), FloatArg('y1'), FloatArg('x2'), FloatArg('y2')]
                        w2c = [FloatArg('wx'), FloatArg('wy')]
                        c2w = [IntArg('cx'), IntArg('cy')]
#                         set_stipple_origin = [('IsGdkGC', 'gc')]
                class GtkMenuShell:
                    class GtkMenuBar:
                        # TODO
                        pass
                    class GtkMenu:
                        # TODO
                        pass
                class GtkNotebook:
                    # TODO
                    pass
                class GtkSocket:
                    # TODO
                    pass
                class GtkTable:
                    # TODO
                    pass
                class GtkTextView:
                    __new__ = Function(opt_args=[IsGtkTextBuffer])
                    set_buffer = ['IsGtkTextBuffer']
                    get_buffer = []
                    set_text = Function(py_name='gap.text_view_set_text', args=[IsGtkTextView, ArgText], is_meth=False)
                    get_text = Function(py_name='gap.text_view_get_text', args=[IsGtkTextView], is_meth=False)
                    set_markup = Function(py_name='moo.app.text_view_set_markup',
                                          args=[IsGtkTextView, StringArg('markup')], is_meth=False)
#                     void        gtk_text_view_scroll_to_mark    (GtkTextView *text_view,
#                                                                  GtkTextMark *mark,
#                                                                  gdouble within_margin,
#                                                                  IsBool use_align,
#                                                                  gdouble xalign,
#                                                                  gdouble yalign);
#                     IsBool    gtk_text_view_scroll_to_iter    (GtkTextView *text_view,
#                                                                  GtkTextIter *iter,
#                                                                  gdouble within_margin,
#                                                                  IsBool use_align,
#                                                                  gdouble xalign,
#                                                                  gdouble yalign);
#                     void        gtk_text_view_scroll_mark_onscreen
#                                                                 (GtkTextView *text_view,
#                                                                  GtkTextMark *mark);
#                     IsBool    gtk_text_view_move_mark_onscreen
#                                                                 (GtkTextView *text_view,
#                                                                  GtkTextMark *mark);
#                     IsBool    gtk_text_view_place_cursor_onscreen
#                                                                 (GtkTextView *text_view);
#                     void        gtk_text_view_get_visible_rect  (GtkTextView *text_view,
#                                                                  GdkRectangle *visible_rect);
#                     void        gtk_text_view_get_iter_location (GtkTextView *text_view,
#                                                                  const GtkTextIter *iter,
#                                                                  GdkRectangle *location);
#                     void        gtk_text_view_get_line_at_y     (GtkTextView *text_view,
#                                                                  GtkTextIter *target_iter,
#                                                                  IsInt y,
#                                                                  IsInt *line_top);
#                     void        gtk_text_view_get_line_yrange   (GtkTextView *text_view,
#                                                                  const GtkTextIter *iter,
#                                                                  IsInt *y,
#                                                                  IsInt *height);
#                     void        gtk_text_view_get_iter_at_location
#                                                                 (GtkTextView *text_view,
#                                                                  GtkTextIter *iter,
#                                                                  IsInt x,
#                                                                  IsInt y);
#                     void        gtk_text_view_get_iter_at_position
#                                                                 (GtkTextView *text_view,
#                                                                  GtkTextIter *iter,
#                                                                  IsInt *trailing,
#                                                                  IsInt x,
#                                                                  IsInt y);
#                     void        gtk_text_view_buffer_to_window_coords
#                                                                 (GtkTextView *text_view,
#                                                                  GtkTextWindowType win,
#                                                                  IsInt buffer_x,
#                                                                  IsInt buffer_y,
#                                                                  IsInt *window_x,
#                                                                  IsInt *window_y);
#                     void        gtk_text_view_window_to_buffer_coords
#                                                                 (GtkTextView *text_view,
#                                                                  GtkTextWindowType win,
#                                                                  IsInt window_x,
#                                                                  IsInt window_y,
#                                                                  IsInt *buffer_x,
#                                                                  IsInt *buffer_y);
#                     GdkWindow*  gtk_text_view_get_window        (GtkTextView *text_view,
#                                                                  GtkTextWindowType win);
#                     GtkTextWindowType gtk_text_view_get_window_type
#                                                                 (GtkTextView *text_view,
#                                                                  GdkWindow *window);
#                     void        gtk_text_view_set_border_window_size
#                                                                 (GtkTextView *text_view,
#                                                                  GtkTextWindowType type,
#                                                                  IsInt size);
#                     IsInt        gtk_text_view_get_border_window_size
#                                                                 (GtkTextView *text_view,
#                                                                  GtkTextWindowType type);
#                     IsBool    gtk_text_view_forward_display_line
#                                                                 (GtkTextView *text_view,
#                                                                  GtkTextIter *iter);
#                     IsBool    gtk_text_view_backward_display_line
#                                                                 (GtkTextView *text_view,
#                                                                  GtkTextIter *iter);
#                     IsBool    gtk_text_view_forward_display_line_end
#                                                                 (GtkTextView *text_view,
#                                                                  GtkTextIter *iter);
#                     IsBool    gtk_text_view_backward_display_line_start
#                                                                 (GtkTextView *text_view,
#                                                                  GtkTextIter *iter);
#                     IsBool    gtk_text_view_starts_display_line
#                                                                 (GtkTextView *text_view,
#                                                                  const GtkTextIter *iter);
#                     IsBool    gtk_text_view_move_visually     (GtkTextView *text_view,
#                                                                  GtkTextIter *iter,
#                                                                  IsInt count);
#                     void        gtk_text_view_add_child_at_anchor
#                                                                 (GtkTextView *text_view,
#                                                                  GtkWidget *child,
#                                                                  GtkTextChildAnchor *anchor);
#                                 GtkTextChildAnchor;
#                     GtkTextChildAnchor* gtk_text_child_anchor_new
#                                                                 (void);
#                     GList*      gtk_text_child_anchor_get_widgets
#                                                                 (GtkTextChildAnchor *anchor);
#                     IsBool    gtk_text_child_anchor_get_deleted
#                                                                 (GtkTextChildAnchor *anchor);
#                     void        gtk_text_view_add_child_in_window
#                                                                 (GtkTextView *text_view,
#                                                                  GtkWidget *child,
#                                                                  GtkTextWindowType which_window,
#                                                                  IsInt xpos,
#                                                                  IsInt ypos);
#                     void        gtk_text_view_move_child        (GtkTextView *text_view,
#                                                                  GtkWidget *child,
#                                                                  IsInt xpos,
#                                                                  IsInt ypos);
#                     void        gtk_text_view_set_wrap_mode     (GtkTextView *text_view,
#                                                                  GtkWrapMode wrap_mode);
#                     GtkWrapMode gtk_text_view_get_wrap_mode     (GtkTextView *text_view);
#                     void        gtk_text_view_set_editable      (GtkTextView *text_view,
#                                                                  IsBool setting);
#                     IsBool    gtk_text_view_get_editable      (GtkTextView *text_view);
#                     void        gtk_text_view_set_cursor_visible
#                                                                 (GtkTextView *text_view,
#                                                                  IsBool setting);
#                     IsBool    gtk_text_view_get_cursor_visible
#                                                                 (GtkTextView *text_view);
#                     void        gtk_text_view_set_overwrite     (GtkTextView *text_view,
#                                                                  IsBool overwrite);
#                     IsBool    gtk_text_view_get_overwrite     (GtkTextView *text_view);
#                     void        gtk_text_view_set_pixels_above_lines
#                                                                 (GtkTextView *text_view,
#                                                                  IsInt pixels_above_lines);
#                     IsInt        gtk_text_view_get_pixels_above_lines
#                                                                 (GtkTextView *text_view);
#                     void        gtk_text_view_set_pixels_below_lines
#                                                                 (GtkTextView *text_view,
#                                                                  IsInt pixels_below_lines);
#                     IsInt        gtk_text_view_get_pixels_below_lines
#                                                                 (GtkTextView *text_view);
#                     void        gtk_text_view_set_pixels_inside_wrap
#                                                                 (GtkTextView *text_view,
#                                                                  IsInt pixels_inside_wrap);
#                     IsInt        gtk_text_view_get_pixels_inside_wrap
#                                                                 (GtkTextView *text_view);
#                     void        gtk_text_view_set_justification (GtkTextView *text_view,
#                                                                  GtkJustification justification);
#                     GtkJustification gtk_text_view_get_justification
#                                                                 (GtkTextView *text_view);
#                     void        gtk_text_view_set_left_margin   (GtkTextView *text_view,
#                                                                  IsInt left_margin);
#                     IsInt        gtk_text_view_get_left_margin   (GtkTextView *text_view);
#                     void        gtk_text_view_set_right_margin  (GtkTextView *text_view,
#                                                                  IsInt right_margin);
#                     IsInt        gtk_text_view_get_right_margin  (GtkTextView *text_view);
#                     void        gtk_text_view_set_indent        (GtkTextView *text_view,
#                                                                  IsInt indent);
#                     IsInt        gtk_text_view_get_indent        (GtkTextView *text_view);
#                     void        gtk_text_view_set_tabs          (GtkTextView *text_view,
#                                                                  PangoTabArray *tabs);
#                     PangoTabArray* gtk_text_view_get_tabs       (GtkTextView *text_view);
#                     void        gtk_text_view_set_accepts_tab   (GtkTextView *text_view,
#                                                                  IsBool accepts_tab);
#                     IsBool    gtk_text_view_get_accepts_tab   (GtkTextView *text_view);
#                     GtkTextAttributes* gtk_text_view_get_default_attributes
#                                                                 (GtkTextView *text_view);
                    class MooTextView:
                        __py_name__ = 'moo.edit.TextView'
                        can_redo = []
                        can_undo = []
                        redo = []
                        undo = []
                        begin_not_undoable_action = []
                        end_not_undoable_action = []
                        set_lang_by_id = [IsString]

                class GtkToolbar:
                    insert = [IsGtkToolItem, IsInt]
                    get_item_index = [IsGtkToolItem]
                    get_n_items = []
                    get_nth_item = [IsInt]
                    set_show_arrow = [ArgSetting]
                    get_drop_index = [IsInt, IsInt]
                    set_drop_highlight_item = [IsGtkToolItem, IsInt]
                    set_orientation = [GtkOrientation]
                    set_tooltips = [ArgSetting]
                    unset_icon_size = []
                    get_show_arrow = []
                    get_orientation = []
                    # get_style matches GtkWidget method
                    get_style = Function(gap_name='GetToolbarStyle')
                    get_icon_size = []
                    get_tooltips = []
                    get_relief_style = []
                    set_style = [GtkToolbarStyle]
                    set_icon_size = [GtkIconSize]
                    unset_style = []
                class GtkTreeView:
                    __new__ = Function(opt_args=[(IsGtkTreeModel, 'model')])
                    get_model = []
                    set_model = [(IsGtkTreeModel, 'model')]
                    get_selection = []
                    get_hadjustment = []
                    set_hadjustment = [IsGtkAdjustment]
                    get_vadjustment = []
                    set_vadjustment = [IsGtkAdjustment]
                    get_headers_visible = []
                    set_headers_visible = [ArgSetting]
                    columns_autosize = []
                    set_headers_clickable = [ArgSetting]
                    set_rules_hint = [ArgSetting]
                    get_rules_hint = []
                    append_column = [IsGtkTreeViewColumn]
                    remove_column = [IsGtkTreeViewColumn]
                    insert_column = [IsGtkTreeViewColumn, IsInt]
#                     IsInt        gtk_tree_view_insert_column_with_attributes
#                                                                 (GtkTreeView *tree_view,
#                                                                  IsInt position,
#                                                                  const gchar *title,
#                                                                  GtkCellRenderer *cell,
#                                                                  ...);
#                     IsInt        gtk_tree_view_insert_column_with_data_func
#                                                                 (GtkTreeView *tree_view,
#                                                                  IsInt position,
#                                                                  const gchar *title,
#                                                                  GtkCellRenderer *cell,
#                                                                  GtkTreeCellDataFunc func,
#                                                                  gpointer data,
#                                                                  GDestroyNotify dnotify);
                    get_column = [IsInt]
                    get_columns = []
                    move_column_after = [(IsGtkTreeViewColumn, 'column'), (IsGtkTreeViewColumn, 'base_column')]
                    set_expander_column = [IsGtkTreeViewColumn]
                    get_expander_column = []
                    scroll_to_point = [IsInt, IsInt]

                    scroll_to_cell = Function(args=[TreePathArg()], opt_args=[IsGtkTreeViewColumn, BoolArg('use_align'),
                                                                              FloatArg('row_align'), FloatArg('col_align')])

                    set_cursor = Function(args=[TreePathArg()], opt_args=[(IsGtkTreeViewColumn, 'focus_column'), BoolArg('start_editing')])
                    set_cursor_on_cell = Function(args=[TreePathArg()], opt_args=[(IsGtkTreeViewColumn, 'focus_column'), (IsGtkCellRenderer, 'focus_cell'), BoolArg('start_editing')])
                    get_cursor = Function(ret_transform='[GTK_TREE_PATH_P2G(%(retval)s[1]), %(retval)s[2]]')
                    row_activated = [TreePathArg(), IsGtkTreeViewColumn]
                    expand_all = []
                    collapse_all = []
                    expand_to_path = [TreePathArg()]
                    expand_row = [TreePathArg(), BoolArg('open_all')]
                    collapse_row = [TreePathArg()]
                    row_expanded = [TreePathArg()]
                    set_reorderable = [ArgSetting]
                    get_reorderable = []
                    get_path_at_pos = 'doc_stub'
                    get_cell_area = [TreePathArg(), IsGtkTreeViewColumn]
                    get_background_area = [TreePathArg(), IsGtkTreeViewColumn]
                    get_visible_rect = []
                    get_visible_range = 'doc_stub'
                    get_bin_window = []
                    widget_to_tree_coords = [IsInt, IsInt]
                    tree_to_widget_coords = [IsInt, IsInt]
                    set_enable_search = [ArgSetting]
                    get_enable_search = []
                    get_search_column = []
                    set_search_column = [IsInt]
                    get_fixed_height_mode = []
                    set_fixed_height_mode = [ArgSetting]
                    get_hover_selection = []
                    set_hover_selection = [ArgSetting]
                    get_hover_expand = []
                    set_hover_expand = [ArgSetting]
            class GtkMisc:
                __no_constructor__ = True
                class GtkLabel:
                    __new__ = Function(opt_args=[ArgText])
#                     GtkWidget*  gtk_label_new                   (const gchar *str);
#                     void        gtk_label_set_text              (GtkLabel *label,
#                                                                  const gchar *str);
#                     void        gtk_label_set_attributes        (GtkLabel *label,
#                                                                  PangoAttrList *attrs);
#                     void        gtk_label_set_markup            (GtkLabel *label,
#                                                                  const gchar *str);
#                     void        gtk_label_set_markup_with_mnemonic
#                                                                 (GtkLabel *label,
#                                                                  const gchar *str);
#                     void        gtk_label_set_pattern           (GtkLabel *label,
#                                                                  const gchar *pattern);
#                     void        gtk_label_set_justify           (GtkLabel *label,
#                                                                  GtkJustification jtype);
#                     void        gtk_label_set_ellipsize         (GtkLabel *label,
#                                                                  PangoEllipsizeMode mode);
#                     void        gtk_label_set_width_chars       (GtkLabel *label,
#                                                                  gint n_chars);
#                     void        gtk_label_set_max_width_chars   (GtkLabel *label,
#                                                                  gint n_chars);
#                     void        gtk_label_get                   (GtkLabel *label,
#                                                                  gchar **str);
#                     guint       gtk_label_parse_uline           (GtkLabel *label,
#                                                                  const gchar *string);
#                     void        gtk_label_set_line_wrap         (GtkLabel *label,
#                                                                  gboolean wrap);
#                     #define     gtk_label_set
#                     void        gtk_label_get_layout_offsets    (GtkLabel *label,
#                                                                  gint *x,
#                                                                  gint *y);
#                     guint       gtk_label_get_mnemonic_keyval   (GtkLabel *label);
#                     gboolean    gtk_label_get_selectable        (GtkLabel *label);
#                     const gchar* gtk_label_get_text             (GtkLabel *label);
#                     GtkWidget*  gtk_label_new_with_mnemonic     (const gchar *str);
#                     void        gtk_label_select_region         (GtkLabel *label,
#                                                                  gint start_offset,
#                                                                  gint end_offset);
#                     void        gtk_label_set_mnemonic_widget   (GtkLabel *label,
#                                                                  GtkWidget *widget);
#                     void        gtk_label_set_selectable        (GtkLabel *label,
#                                                                  gboolean setting);
#                     void        gtk_label_set_text_with_mnemonic
#                                                                 (GtkLabel *label,
#                                                                  const gchar *str);
#                     PangoAttrList* gtk_label_get_attributes     (GtkLabel *label);
#                     GtkJustification gtk_label_get_justify      (GtkLabel *label);
#                     PangoEllipsizeMode gtk_label_get_ellipsize  (GtkLabel *label);
#                     gint        gtk_label_get_width_chars       (GtkLabel *label);
#                     gint        gtk_label_get_max_width_chars   (GtkLabel *label);
#                     const gchar* gtk_label_get_label            (GtkLabel *label);
#                     PangoLayout* gtk_label_get_layout           (GtkLabel *label);
#                     gboolean    gtk_label_get_line_wrap         (GtkLabel *label);
#                     GtkWidget*  gtk_label_get_mnemonic_widget   (GtkLabel *label);
#                     gboolean    gtk_label_get_selection_bounds  (GtkLabel *label,
#                                                                  gint *start,
#                                                                  gint *end);
#                     gboolean    gtk_label_get_use_markup        (GtkLabel *label);
#                     gboolean    gtk_label_get_use_underline     (GtkLabel *label);
#                     gboolean    gtk_label_get_single_line_mode  (GtkLabel *label);
#                     gdouble     gtk_label_get_angle             (GtkLabel *label);
#                     void        gtk_label_set_label             (GtkLabel *label,
#                                                                  const gchar *str);
#                     void        gtk_label_set_use_markup        (GtkLabel *label,
#                                                                  gboolean setting);
#                     void        gtk_label_set_use_underline     (GtkLabel *label,
#                                                                  gboolean setting);
#                     void        gtk_label_set_single_line_mode  (GtkLabel *label,
#                                                                  gboolean single_line_mode);
#                     void        gtk_label_set_angle             (GtkLabel *label,
#                                                                  gdouble angle);
                    class GtkAccelLabel: pass
                class GtkArrow: pass
                    # TODO
            class GtkCalendar: pass
            class GtkDrawingArea:
                class GtkCurve: pass
            class GtkEntry:
                __new__ = Function(opt_args=[IntArg('maxlen')])
                set_text = [ArgText]
                append_text = [ArgText]
                prepend_text = [ArgText]
                set_position = [ArgPos]
                get_text = []
                select_region = [ArgStart, ArgEnd]
                set_visibility = [BoolArg('visible')]
#                 set_invisible_char    (GtkEntry *entry,
#                                                              gunichar ch);
                set_editable = [BoolArg('editable')]
                set_max_length = [IntArg('max_len')]
                get_activates_default = []
                get_has_frame = []
                get_width_chars = []
                set_activates_default = [BoolArg('activates_default')]
                set_has_frame = [BoolArg('has_frame')]
                set_width_chars = [IntArg('n_chars')]
                get_invisible_char = []
                set_alignment = [FloatArg('xalign')]
                get_alignment = []
                get_max_length = []
                get_visibility = []
                class GtkSpinButton: pass
                    # TODO
            class GtkRuler:
                    # TODO
                class GtkHRuler: pass
                    # TODO
                class GtkVRuler: pass
                    # TODO
            class GtkRange:
                    # TODO
                class GtkScale:
                    # TODO
                    class GtkHScale: pass
                    # TODO
                    class GtkVScale: pass
                    # TODO
                class GtkScrollbar:
                    # TODO
                    class GtkHScrollbar: pass
                    # TODO
                    class GtkVScrollbar: pass
                    # TODO
            class GtkSeparator:
                    # TODO
                class GtkHSeparator: pass
                    # TODO
                class GtkVSeparator: pass
                    # TODO
            class GtkProgress:
                    # TODO
                class GtkProgressBar: pass
                    # TODO
        class GtkAdjustment: pass
                    # TODO
        class GtkCellRenderer:
                    # TODO
            __no_constructor__ = True
            class GtkCellRendererText:
                    # TODO
                class GtkCellRendererCombo: pass
                    # TODO
            class GtkCellRendererPixbuf: pass
                    # TODO
            class GtkCellRendererProgress: pass
                    # TODO
            class GtkCellRendererToggle: pass
                    # TODO
        class FooCanvasItem:
            __no_constructor__ = True
            move = [FloatArg('dx'), FloatArg('dy')]
            raise_ = Function(gap_name='Raise', args=[IntArg('positions')])
            lower = [IntArg('positions')]
            raise_to_top = []
            lower_to_bottom = []
            send_behind = [('IsFooCanvasItem', 'behind_item')]
            show = []
            hide = []
            grab = 'doc_stub'
            ungrab = [IntArg('etime')]
            w2i = [FloatArg('x'), FloatArg('y')]
            i2w = [FloatArg('x'), FloatArg('y')]
            reparent = [('IsFooCanvasGroup', 'new_group')]
            grab_focus = []
            get_bounds = []
            request_update = []
            request_redraw = []
            class FooCanvasGroup:
                __new__ = 'doc_stub'
            class FooCanvasLine:
                __new__ = 'doc_stub'
            class FooCanvasRE:
                __no_constructor__ = True
                class FooCanvasRect:
                    __new__ = 'doc_stub'
                class FooCanvasEllipse:
                    __new__ = 'doc_stub'
            class FooCanvasPolygon:
                __new__ = 'doc_stub'
            class FooCanvasPixbuf:
                __new__ = 'doc_stub'
            class FooCanvasWidget:
                __new__ = 'doc_stub'
    class GtkTooltips: pass
                    # TODO

    class GtkTreeViewColumn:
        __implements__ = ['GtkCellLayout']
        __new__ = Function(opt_args=[ArgTitle, IsGtkCellRenderer])
#         GtkTreeViewColumn* gtk_tree_view_column_new_with_attributes
#                                                     (const gchar *title,
#                                                      GtkCellRenderer *cell,
#                                                      ...);
        get_cell_renderers = []
#         set_attributes
#                                                     (GtkTreeViewColumn *tree_column,
#                                                      GtkCellRenderer *cell_renderer,
#                                                      ...);
        set_spacing = [IntArg('spasing')]
        get_spacing = []
        set_visible = [BoolArg('visible')]
        get_visible = []
        set_resizable = [BoolArg('resizable')]
        get_resizable = []
        set_sizing = [IntArg('mode')]
        get_sizing = []
        get_width = []
        get_fixed_width = []
        set_fixed_width = [IntArg('width')]
        set_min_width = [IntArg('min_width')]
        get_min_width = []
        set_max_width = [IntArg('max_width')]
        get_max_width = []
        clicked = []
        set_title = [ArgTitle]
        get_title = []
        set_expand = [BoolArg('setting')]
        get_expand = []
        set_clickable = [BoolArg('setting')]
        get_clickable = []
        set_widget = [IsGtkWidget]
        get_widget = []
        set_alignment = [FloatArg('align')]
        get_alignment = []
        set_reorderable = [BoolArg('setting')]
        get_reorderable = []
        set_sort_column_id = [IntArg('id')]
        get_sort_column_id = []
        set_sort_indicator = [BoolArg('show')]
        get_sort_indicator = []
        set_sort_order = [IntArg('order')]
        get_sort_order = []
#         cell_set_cell_data
#                                                     (GtkTreeViewColumn *tree_column,
#                                                      GtkTreeModel *tree_model,
#                                                      GtkTreeIter *iter,
#                                                      gboolean is_expander,
#                                                      gboolean is_expanded);
#         cell_get_size
#                                                     (GtkTreeViewColumn *tree_column,
#                                                      GdkRectangle *cell_area,
#                                                      gint *x_offset,
#                                                      gint *y_offset,
#                                                      gint *width,
#                                                      gint *height);
#         cell_get_position
#                                                     (GtkTreeViewColumn *tree_column,
#                                                      GtkCellRenderer *cell_renderer,
#                                                      gint *start_pos,
#                                                      gint *width);
        cell_is_visible = []
        focus_cell = [IsGtkCellRenderer]
        queue_resize = []

    class GtkListStore:
        __implements__ = ['GtkTreeSortable']
        __new__ = 'doc_stub'
        # TODO
    class GtkTextBuffer:
        # TODO
        pass
    class GtkTreeSelection:
        __no_constructor__ = True
        set_mode = [GtkSelectionMode]
        get_mode = []
        get_tree_view = []
        get_selected = Function(args=[IsGtkTreeSelection], is_meth=False,
                                py_name='gap.tree_selection_get_selected',
                                ret_transform='GTK_TREE_PATH_P2G(%(retval)s[2])')
        get_selected_rows = Function(args=[IsGtkTreeSelection], is_meth=False,
                                     py_name='gap.tree_selection_get_selected_rows',
                                     ret_transform='List(%(retval)s[2], GTK_TREE_PATH_P2G)')
        count_selected_rows = []
        select_row = Function(args=[TreePathArg()], py_name='select_path')
        unselect_row = Function(args=[TreePathArg()], py_name='unselect_path')
        row_is_selected = Function(args=[TreePathArg()], py_name='path_is_selected')
        select_all = []
        unselect_all = []
        select_range = Function(args=[TreePathArg('start'), TreePathArg('end')])
        unselect_range = Function(args=[TreePathArg('start'), TreePathArg('end')])
    class GtkTreeStore:
        __implements__ = ['GtkTreeSortable']
        __new__ = 'doc_stub'
        # TODO

    class MooGladeXML:
        __no_constructor__ = True
        __py_name__ = 'moo.utils.GladeXML'
        __gap_name__ = 'IsGladeXML'
        __new__ = 'doc_stub'
        __arg_name__ = 'xml'
        get_widget = [ArgName]
        get_root = []


top_classes.append(GObject)


functions = [
    Function(py_name='gtk.window_list_toplevels', gap_name='WindowListToplevels'),
    Function(py_name='gtk.window_set_default_icon_from_file', gap_name='WindowSetDefaultIconFromFile',
             args=[ArgFilename]),
    Function(py_name='gtk.window_set_default_icon_name', gap_name='WindowSetDefaultIconName',
             args=[StringArg('icon_name')]),
    Function(py_name='gtk.gdk.screen_get_default', gap_name='ScreenGetDefault'),
    Function(py_name='gtk.gdk.display_get_default', gap_name='DisplayGetDefault'),
    Function(py_name='gtk.gdk.pixbuf_new_from_file', gap_name='GdkPixbuf', args=[ArgFilename]),
    Function(py_name='gtk.gdk.pixbuf_new_from_file', gap_name='GdkPixbuf',
             args=[ArgFilename, ArgWidth, ArgHeight]),
]


top_classes = [ClassInfo(c, None) for c in top_classes]
functions = [Function(f) for f in functions]
for f in functions:
    f.is_meth = False


constants = [
    ['GTK_TREE_SORTABLE_DEFAULT_SORT_COLUMN_ID', -1],
    ['GTK_TREE_SORTABLE_UNSORTED_SORT_COLUMN_ID', -2],
]

constants = [Constant(*c) for c in constants]
def _add_stock():
    import gtk
    for name in dir(gtk):
        if name.startswith('STOCK_'):
            constants.append(Constant('GTK_' + name, getattr(gtk, name)))
_add_stock()
