from defs import Function, ClassInfo

IsGtkTreeViewColumn = 'IsGtkTreeViewColumn'
IsGtkWidget = 'IsGtkWidget'
IsGtkWindow = 'IsGtkWindow'
IsGtkTreeModel = 'IsGtkTreeModel'
IsGtkTooltips = 'IsGtkTooltips'
IsGtkAdjustment = 'IsGtkAdjustment'
IsGtkTextBuffer = 'IsGtkTextBuffer'
IsGtkToolItem = 'IsGtkToolItem'
IsGdkScreen = 'IsGdkScreen'
IsGdkWindow = 'IsGdkWindow'

IsObject = 'IsObject'
IsString = 'IsString'
IsInt = 'IsInt'
IsList = 'IsList'

gboolean = 'IsBool'
gint = 'IsInt'
guint = 'IsInt'
gfloat = 'IsRat'

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

top_classes = []

class GtkCellLayout:
    __no_constructor__ = True
    # TODO
top_classes.append(GtkCellLayout)
class GtkEditable:
    __no_constructor__ = True
    # TODO
top_classes.append(GtkEditable)
class GtkFileChooser:
    __no_constructor__ = True
    # TODO
top_classes.append(GtkFileChooser)
class GtkTreeModel:
    __no_constructor__ = True
    # TODO
top_classes.append(GtkTreeModel)
class GtkTreeSortable:
    __no_constructor__ = True
    __implements__ = ['GtkTreeModel']
    # TODO
top_classes.append(GtkTreeSortable)

class GObject:
    __no_constructor__ = True

    set_property = [IsString, IsObject]
    get_property = [IsString]

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
        get_monitor_geometry = [gint]
        get_monitor_at_point = [gint, gint]
        get_monitor_at_window = [IsGdkWindow]

    class GdkDisplay:
        __new__ = Function(gap_name='GdkDisplay', py_name='gtk.gdk.Display', args=[IsString])
        get_name = Function(gap_name='GetDisplayName')
        get_n_screens = []
        get_screen = [gint]
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
            reparent = [IsGtkWidget]
            is_focus = []
            grab_focus = []
            grab_default = []
            set_name = Function([IsString], gap_name='SetWidgetName')
            get_name = Function(gap_name='GetWidgetName')
            set_state = [GtkStateType]
            set_sensitive = [gboolean]
            set_events = [gint]
            add_events = [gint]
            get_toplevel = []
            get_events = []
            get_pointer = []
            is_ancestor = [IsGtkWidget]
#             modify_fg = [GtkStateType, GdkColor]
#             modify_bg = [GtkStateType, GdkColor]
#             modify_text = [GtkStateType, GdkColor]
#             modify_base = [GtkStateType, GdkColor]
            modify_font = Function(py_name='gap.widget_modify_font', gap_name='ModifyFont', args=[IsString])
            get_parent = []
            get_settings = []
            get_clipboard = []
            get_display = []
            get_root_window = []
            get_screen = []
            has_screen = []

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
                child_get_property = [IsGtkWidget, IsString]
                child_set_property = [IsGtkWidget, IsString, IsObject]
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
                        __new__ = Function(py_name='gtk.Window', opt_args=[GtkWindowType])
                        set_title = [IsString]
                        set_resizable = [gboolean]
                        get_resizable = []
                        activate_focus = []
                        activate_default = []
                        set_modal = [gboolean]
                        set_default_size = [gint, gint]
                        set_position = [GtkWindowPosition]
                        set_transient_for = [IsGtkWindow]
                        set_destroy_with_parent = [gboolean]
                        set_screen = [IsGdkScreen]
                        is_active = []
                        has_toplevel_focus = []
                        get_focus = []
                        set_focus = [IsGtkWidget]
                        set_default = [IsGtkWidget]
                        present = []
                        present_with_time = [gint]
                        iconify = []
                        deiconify = []
                        stick = []
                        unstick = []
                        maximize = []
                        unmaximize = []
                        fullscreen = []
                        unfullscreen = []
                        set_keep_above = [gboolean]
                        set_keep_below = [gboolean]
                        set_decorated = [gboolean]
                        set_has_frame = [gboolean]
                        set_role = [IsString]
                        set_type_hint = [GdkWindowTypeHint]
                        set_skip_taskbar_hint = [gboolean]
                        set_skip_pager_hint = [gboolean]
                        set_urgency_hint = [gboolean]
                        set_accept_focus = [gboolean]
                        set_focus_on_map = [gboolean]
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
                        move = [gint, gint]
                        parse_geometry = [IsString]
                        reshow_with_initial_size = []
                        resize = [gint, gint]
                        set_icon_from_file = [IsString]
                        set_icon_name = [IsString]

                        class GtkDialog:
                            __new__ = Function(py_name="gtk.Dialog",
                                               opt_args=[IsString, IsGtkWindow, GtkDialogFlags, IsList])
                            run = []
                            response = [(gint, 'response')]
                            add_button = [IsString, gint]
#                             add_buttons          (GtkDialog *dialog,
#                                                                          const gchar *first_button_text,
#                                                                          ...);
                            add_action_widget = [IsGtkWidget, gint]
                            get_has_separator = []
                            set_default_response = [gint]
                            set_has_separator = [gboolean]
                            set_response_sensitive = [gint, gboolean]
                            get_response_for_widget = [IsGtkWidget]

#                             set_alternative_button_order
#                                                                         (GtkDialog *dialog,
#                                                                          gint first_response_id,
#                                                                          ...);

                            class GtkAboutDialog:
                                __new__ = Function(py_name='gtk.AboutDialog')
#                                 get_name      (GtkAboutDialog *about);
#                                 set_name       (GtkAboutDialog *about,
#                                                                              const gchar *name);
                                get_version = []
                                set_version = [IsString]
                                get_copyright = []
                                set_copyright = [IsString]
                                get_comments = []
                                set_comments = [IsString]
                                get_license = []
                                set_license = [IsString]
                                get_wrap_license = []
                                set_wrap_license = [gboolean]
                                get_website = []
                                set_website = [IsString]
                                get_website_label = []
                                set_website_label = [IsString]
                                get_authors = []
                                set_authors = [IsList]
                                get_artists = []
                                set_artists = [IsList]
                                get_documenters = []
                                set_documenters = [IsList]
                                get_translator_credits = []
                                set_translator_credits = [IsString]
#                                 GdkPixbuf*  gtk_about_dialog_get_logo       (GtkAboutDialog *about);
#                                 set_logo       (GtkAboutDialog *about,
#                                                                              GdkPixbuf *logo);
                                get_logo_icon_name = []
                                set_logo_icon_name = [IsString]
#                                 void        gtk_show_about_dialog           (GtkWindow *parent,
#                                                                              const gchar *first_property_name,
#                                                                              ...);
                            class GtkColorSelectionDialog: pass
#                                 GtkWidget*  gtk_color_selection_dialog_new  (const gchar *title);
                            class GtkFileChooserDialog: pass
#                                 GtkWidget*  gtk_file_chooser_dialog_new     (const gchar *title,
#                                                                              GtkWindow *parent,
#                                                                              GtkFileChooserAction action,
#                                                                              const gchar *first_button_text,
#                                                                              ...);
#                                 GtkWidget*  gtk_file_chooser_dialog_new_with_backend
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
#                                 gboolean    gtk_font_selection_dialog_set_font_name
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
#                         GtkWidget*  gtk_alignment_new               (gfloat xalign,
#                                                                      gfloat yalign,
#                                                                      gfloat xscale,
#                                                                      gfloat yscale);
                        set = [gfloat, gfloat, gfloat, gfloat]
                        get_padding = []
                        set_padding = [guint, guint, guint, guint]
                    class GtkFrame:
#                         GtkWidget*  gtk_frame_new                   (const gchar *label);
                        set_label = [IsString]
                        set_label_widget = [IsGtkWidget]
                        set_label_align = [gfloat, gfloat]
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
#                                                                          gboolean obey_child);
                            set = [gfloat, gfloat, gfloat, gboolean]

                    class GtkButton:
                        __new__ = Function(py_name='gtk.Button', opt_args=[IsString, IsString, gboolean])
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
                        set_label = [IsString]
                        get_use_stock = []
                        set_use_stock = [gboolean]
                        get_use_underline = []
                        set_use_underline = [gboolean]
                        set_focus_on_click = [gboolean]
                        get_focus_on_click = []
                        set_alignment = [gfloat, gfloat]
                        get_alignment = []
                        set_image = [IsGtkWidget]
                        get_image = []

                        class GtkToggleButton:
                            __new__ = Function(py_name='gtk.ToggleButton', opt_args=[IsString, gboolean])
#                             GtkWidget*  gtk_toggle_button_new           (void);
#                             GtkWidget*  gtk_toggle_button_new_with_label
#                                                                         (const gchar *label);
#                             GtkWidget*  gtk_toggle_button_new_with_mnemonic
#                                                                         (const gchar *label);
                            set_mode = [gboolean]
                            get_mode = []
                            toggled = []
                            get_active = []
                            set_active = [gboolean]
                            get_inconsistent = []
                            set_inconsistent = [gboolean]

                            class GtkCheckButton:
                                __new__ = Function(py_name='gtk.CheckButton', opt_args=[IsString, gboolean])
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
#                             __new__ = Function(py_name='gtk.ColorButton', opt_args=[IsGdkColor])
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
#                                                                          gboolean use_alpha);
#                             gboolean    gtk_color_button_get_use_alpha  (GtkColorButton *color_button);
                            set_title = [IsString]
                            get_title = []

                        class GtkFontButton:
                            __new__ = Function(py_name="gtk.FontButton", opt_args=[IsString])
                            set_font_name = [IsString]
                            get_font_name = []
                            set_show_style = [gboolean]
                            get_show_style = []
                            set_show_size = [gboolean]
                            get_show_size = []
                            set_use_font = [gboolean]
                            get_use_font = []
                            set_use_size = [gboolean]
                            get_use_size = []
                            set_title = [IsString]
                            get_title = []

                    class GtkItem:
                        __no_constructor__ = True
                        class GtkMenuItem:
                            __new__ = Function(py_name="gtk.MenuItem", opt_args=[IsString, gboolean])
                            set_right_justified = [gboolean]
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
#                                                                          gint *requisition);
#                             toggle_size_allocate
#                                                                         (GtkMenuItem *menu_item,
#                                                                          gint allocation);

                            class GtkCheckMenuItem:
                                __new__ = Function(py_name="gtk.CheckMenuItem", opt_args=[IsString, gboolean])
                                get_active = []
                                set_active = [gboolean]
                                set_show_toggle = [gboolean]
                                toggled = []
                                get_inconsistent = []
                                set_inconsistent = [gboolean]
                                set_draw_as_radio = [gboolean]
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
                        __new__ = Function(gap_name='GtkComboBox', py_name='gtk.combo_box_new_text')
#                         __new__ = Function(py_name='gtk.ComboBox', opt_args=['IsGtkTreeModel'])
#                         GtkWidget*  gtk_combo_box_new_text          (void);
                        get_wrap_width = []
                        set_wrap_width = [gint]
                        get_row_span_column = []
                        set_row_span_column = [gint]
                        get_column_span_column = []
                        set_column_span_column = [gint]
                        get_active = []
                        set_active = [gint]
#                         gboolean    gtk_combo_box_get_active_iter   (GtkComboBox *combo_box,
#                                                                      GtkTreeIter *iter);
#                         set_active_iter   (GtkComboBox *combo_box,
#                                                                      GtkTreeIter *iter);
                        get_model = []
                        set_model = [IsGtkTreeModel]
                        append_text = [IsString]
                        insert_text = [gint, IsString]
                        prepend_text = [IsString]
                        remove_text = [gint]
                        get_active_text = []
                        popup = []
                        popdown = []
                        set_focus_on_click = [gboolean]
                        get_focus_on_click = []

                        class GtkComboBoxEntry:
                            __new__ = Function(gap_name='GtkComboBoxEntry', py_name='gtk.combo_box_entry_new_text')
#                             GtkWidget*  gtk_combo_box_entry_new         (void);
#                             GtkWidget*  gtk_combo_box_entry_new_with_model
#                                                                         (GtkTreeModel *model,
#                                                                          gint text_column);
#                             GtkWidget*  gtk_combo_box_entry_new_text    (void);
                            set_text_column = [gint]
                            get_text_column = []

                    class GtkEventBox:
                        __new__ = Function(py_name='gtk.EventBox')
                        set_above_child = [gboolean]
                        get_above_child = []
                        set_visible_window = [gboolean]
                        get_visible_window = []

                    class GtkExpander:
                        __new__ = Function(py_name='gtk.Expander', opt_args=[IsString])
                        set_expanded = [gboolean]
                        get_expanded = []
                        set_spacing = [gint]
                        get_spacing = []
                        set_label = [IsString]
                        get_label = []
                        set_use_underline = [gboolean]
                        get_use_underline = []
                        set_use_markup = [gboolean]
                        get_use_markup = []
                        set_label_widget = [IsGtkWidget]
                        get_label_widget = []

                    class GtkHandleBox:
                        __new__ = Function(py_name='gtk.HandleBox')
                        set_shadow_type = [GtkShadowType]
                        set_handle_position = [GtkPositionType]
                        set_snap_edge = [GtkPositionType]
                        get_handle_position = []
                        get_shadow_type = []
                        get_snap_edge = []

                    class GtkToolItem:
                        __new__ = Function(py_name="gtk.ToolItem")
                        set_homogeneous = [gboolean]
                        get_homogeneous = []
                        set_expand = [gboolean]
                        get_expand = []
                        set_tooltip = [IsGtkTooltips, IsString, IsString]
                        set_use_drag_window = [gboolean]
                        get_use_drag_window = []
                        set_visible_horizontal = [gboolean]
                        get_visible_horizontal = []
                        set_visible_vertical = [gboolean]
                        get_visible_vertical = []
                        set_is_important = [gboolean]
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
                            __new__ = Function(py_name='gtk.ToolButton', opt_args=[IsGtkWidget, IsString])
#                             GtkToolItem* gtk_tool_button_new            (GtkWidget *icon_widget,
#                                                                          const gchar *label);
#                             GtkToolItem* gtk_tool_button_new_from_stock (const gchar *stock_id);
                            set_label = [IsString]
                            get_label = []
                            set_use_underline = [gboolean]
                            get_use_underline = []
                            set_stock_id = [IsString]
                            get_stock_id = []
                            set_icon_name = [IsString]
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
                                set_arrow_tooltip = ['IsGtkTooltips', IsString, IsString]

                            class GtkToggleToolButton:
                                __new__ = Function(py_name='gtk.ToggleToolButton', opt_args=[IsString])
                                set_active = [gboolean]
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
                            __new__ = Function(py_name='gtk.SeparatorToolItem')
                            set_draw = [gboolean]
                            get_draw = []

                    class GtkScrolledWindow:
                        __new__ = Function(py_name='gtk.ScrolledWindow',
                                           args=[IsGtkAdjustment, IsGtkAdjustment])
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
                        __new__ = Function(py_name='gtk.Viewport', args=[IsGtkAdjustment, IsGtkAdjustment])
                        get_hadjustment = []
                        get_vadjustment = []
                        set_hadjustment = [IsGtkAdjustment]
                        set_vadjustment = [IsGtkAdjustment]
                        set_shadow_type = [GtkShadowType]
                        get_shadow_type = []

                class GtkBox:
                    __no_constructor__ = True
                    pack_start = [IsGtkWidget, gboolean, gboolean, guint]
                    pack_end = [IsGtkWidget, gboolean, gboolean, guint]
                    pack_start_defaults = [IsGtkWidget]
                    pack_end_defaults = [IsGtkWidget]
                    get_homogeneous = []
                    set_homogeneous = [gboolean]
                    get_spacing = []
                    set_spacing = [gint]
                    reorder_child = [IsGtkWidget, gint]
#                     query_child_packing (GtkBox *box,
#                                                                  GtkWidget *child,
#                                                                  gboolean *expand,
#                                                                  gboolean *fill,
#                                                                  guint *padding,
#                                                                  GtkPackType *pack_type);
#                     set_child_packing       (GtkBox *box,
#                                                                  GtkWidget *child,
#                                                                  gboolean expand,
#                                                                  gboolean fill,
#                                                                  guint padding,
#                                                                  GtkPackType pack_type);

                    class GtkButtonBox:
                        get_layout = []
                        get_child_size = []
                        get_child_ipadding = []
                        get_child_secondary = [IsGtkWidget]
                        set_layout = [GtkButtonBoxStyle]
                        set_child_secondary = [IsGtkWidget, gboolean]

                        class GtkHButtonBox:
                            __new__ = Function(py_name='gtk.HButtonBox')
                        class GtkVButtonBox:
                            __new__ = Function(py_name='gtk.VButtonBox')

                    class GtkVBox:
                        __new__ = Function(py_name='gtk.VBox', args=[gboolean, gint])

                        class GtkColorSelection:
                            __new__ = Function(py_name='gtk.ColorSelection')
#                             void        gtk_color_selection_set_update_policy
#                                                                         (GtkColorSelection *colorsel,
#                                                                          GtkUpdateType policy);
#                             void        gtk_color_selection_set_has_opacity_control
#                                                                         (GtkColorSelection *colorsel,
#                                                                          gboolean has_opacity);
#                             gboolean    gtk_color_selection_get_has_opacity_control
#                                                                         (GtkColorSelection *colorsel);
#                             void        gtk_color_selection_set_has_palette
#                                                                         (GtkColorSelection *colorsel,
#                                                                          gboolean has_palette);
#                             gboolean    gtk_color_selection_get_has_palette
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
#                             gboolean    gtk_color_selection_is_adjusting
#                                                                         (GtkColorSelection *colorsel);
#                             gboolean    gtk_color_selection_palette_from_string
#                                                                         (const gchar *str,
#                                                                          GdkColor **colors,
#                                                                          gint *n_colors);
#                             gchar*      gtk_color_selection_palette_to_string
#                                                                         (const GdkColor *colors,
#                                                                          gint n_colors);
#                             GtkColorSelectionChangePaletteFunc gtk_color_selection_set_change_palette_hook
#                                                                         (GtkColorSelectionChangePaletteFunc func);
#                             void        (*GtkColorSelectionChangePaletteFunc)
#                                                                         (const GdkColor *colors,
#                                                                          gint n_colors);
#                             GtkColorSelectionChangePaletteWithScreenFunc gtk_color_selection_set_change_palette_with_screen_hook
#                                                                         (GtkColorSelectionChangePaletteWithScreenFunc func);
#                             void        (*GtkColorSelectionChangePaletteWithScreenFunc)
#                                                                         (GdkScreen *screen,
#                                                                          const GdkColor *colors,
#                                                                          gint n_colors);
#                             void        gtk_color_selection_set_color   (GtkColorSelection *colorsel,
#                                                                          gdouble *color);
#                             void        gtk_color_selection_get_color   (GtkColorSelection *colorsel,
#                                                                          gdouble *color);
                        class GtkFileChooserWidget:
                            __new__ = Function(py_name='gtk.FileChooserWidget', args=[GtkFileChooserAction])
                        class GtkFontSelection:
                            __new__ = Function(py_name='gtk.FontSelection')
                            get_font_name = []
                            set_font_name = [IsString]
                            get_preview_text = []
                            set_preview_text = [IsString]

                    class GtkHBox:
                        __new__ = Function(py_name='gtk.HBox', gap_name='GtkHBox', args=[gboolean, gint])

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
                    pass
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
                    __new__ = Function(py_name='gtk.TextView', opt_args=[IsGtkTextBuffer])
                    set_buffer = ['IsGtkTextBuffer']
                    get_buffer = []
                    set_text = Function(py_name='gap.text_view_set_text', gap_name='SetText', args=[IsString])
                    get_text = Function(py_name='gap.text_view_get_text', gap_name='GetText')
#                     void        gtk_text_view_scroll_to_mark    (GtkTextView *text_view,
#                                                                  GtkTextMark *mark,
#                                                                  gdouble within_margin,
#                                                                  gboolean use_align,
#                                                                  gdouble xalign,
#                                                                  gdouble yalign);
#                     gboolean    gtk_text_view_scroll_to_iter    (GtkTextView *text_view,
#                                                                  GtkTextIter *iter,
#                                                                  gdouble within_margin,
#                                                                  gboolean use_align,
#                                                                  gdouble xalign,
#                                                                  gdouble yalign);
#                     void        gtk_text_view_scroll_mark_onscreen
#                                                                 (GtkTextView *text_view,
#                                                                  GtkTextMark *mark);
#                     gboolean    gtk_text_view_move_mark_onscreen
#                                                                 (GtkTextView *text_view,
#                                                                  GtkTextMark *mark);
#                     gboolean    gtk_text_view_place_cursor_onscreen
#                                                                 (GtkTextView *text_view);
#                     void        gtk_text_view_get_visible_rect  (GtkTextView *text_view,
#                                                                  GdkRectangle *visible_rect);
#                     void        gtk_text_view_get_iter_location (GtkTextView *text_view,
#                                                                  const GtkTextIter *iter,
#                                                                  GdkRectangle *location);
#                     void        gtk_text_view_get_line_at_y     (GtkTextView *text_view,
#                                                                  GtkTextIter *target_iter,
#                                                                  gint y,
#                                                                  gint *line_top);
#                     void        gtk_text_view_get_line_yrange   (GtkTextView *text_view,
#                                                                  const GtkTextIter *iter,
#                                                                  gint *y,
#                                                                  gint *height);
#                     void        gtk_text_view_get_iter_at_location
#                                                                 (GtkTextView *text_view,
#                                                                  GtkTextIter *iter,
#                                                                  gint x,
#                                                                  gint y);
#                     void        gtk_text_view_get_iter_at_position
#                                                                 (GtkTextView *text_view,
#                                                                  GtkTextIter *iter,
#                                                                  gint *trailing,
#                                                                  gint x,
#                                                                  gint y);
#                     void        gtk_text_view_buffer_to_window_coords
#                                                                 (GtkTextView *text_view,
#                                                                  GtkTextWindowType win,
#                                                                  gint buffer_x,
#                                                                  gint buffer_y,
#                                                                  gint *window_x,
#                                                                  gint *window_y);
#                     void        gtk_text_view_window_to_buffer_coords
#                                                                 (GtkTextView *text_view,
#                                                                  GtkTextWindowType win,
#                                                                  gint window_x,
#                                                                  gint window_y,
#                                                                  gint *buffer_x,
#                                                                  gint *buffer_y);
#                     GdkWindow*  gtk_text_view_get_window        (GtkTextView *text_view,
#                                                                  GtkTextWindowType win);
#                     GtkTextWindowType gtk_text_view_get_window_type
#                                                                 (GtkTextView *text_view,
#                                                                  GdkWindow *window);
#                     void        gtk_text_view_set_border_window_size
#                                                                 (GtkTextView *text_view,
#                                                                  GtkTextWindowType type,
#                                                                  gint size);
#                     gint        gtk_text_view_get_border_window_size
#                                                                 (GtkTextView *text_view,
#                                                                  GtkTextWindowType type);
#                     gboolean    gtk_text_view_forward_display_line
#                                                                 (GtkTextView *text_view,
#                                                                  GtkTextIter *iter);
#                     gboolean    gtk_text_view_backward_display_line
#                                                                 (GtkTextView *text_view,
#                                                                  GtkTextIter *iter);
#                     gboolean    gtk_text_view_forward_display_line_end
#                                                                 (GtkTextView *text_view,
#                                                                  GtkTextIter *iter);
#                     gboolean    gtk_text_view_backward_display_line_start
#                                                                 (GtkTextView *text_view,
#                                                                  GtkTextIter *iter);
#                     gboolean    gtk_text_view_starts_display_line
#                                                                 (GtkTextView *text_view,
#                                                                  const GtkTextIter *iter);
#                     gboolean    gtk_text_view_move_visually     (GtkTextView *text_view,
#                                                                  GtkTextIter *iter,
#                                                                  gint count);
#                     void        gtk_text_view_add_child_at_anchor
#                                                                 (GtkTextView *text_view,
#                                                                  GtkWidget *child,
#                                                                  GtkTextChildAnchor *anchor);
#                                 GtkTextChildAnchor;
#                     GtkTextChildAnchor* gtk_text_child_anchor_new
#                                                                 (void);
#                     GList*      gtk_text_child_anchor_get_widgets
#                                                                 (GtkTextChildAnchor *anchor);
#                     gboolean    gtk_text_child_anchor_get_deleted
#                                                                 (GtkTextChildAnchor *anchor);
#                     void        gtk_text_view_add_child_in_window
#                                                                 (GtkTextView *text_view,
#                                                                  GtkWidget *child,
#                                                                  GtkTextWindowType which_window,
#                                                                  gint xpos,
#                                                                  gint ypos);
#                     void        gtk_text_view_move_child        (GtkTextView *text_view,
#                                                                  GtkWidget *child,
#                                                                  gint xpos,
#                                                                  gint ypos);
#                     void        gtk_text_view_set_wrap_mode     (GtkTextView *text_view,
#                                                                  GtkWrapMode wrap_mode);
#                     GtkWrapMode gtk_text_view_get_wrap_mode     (GtkTextView *text_view);
#                     void        gtk_text_view_set_editable      (GtkTextView *text_view,
#                                                                  gboolean setting);
#                     gboolean    gtk_text_view_get_editable      (GtkTextView *text_view);
#                     void        gtk_text_view_set_cursor_visible
#                                                                 (GtkTextView *text_view,
#                                                                  gboolean setting);
#                     gboolean    gtk_text_view_get_cursor_visible
#                                                                 (GtkTextView *text_view);
#                     void        gtk_text_view_set_overwrite     (GtkTextView *text_view,
#                                                                  gboolean overwrite);
#                     gboolean    gtk_text_view_get_overwrite     (GtkTextView *text_view);
#                     void        gtk_text_view_set_pixels_above_lines
#                                                                 (GtkTextView *text_view,
#                                                                  gint pixels_above_lines);
#                     gint        gtk_text_view_get_pixels_above_lines
#                                                                 (GtkTextView *text_view);
#                     void        gtk_text_view_set_pixels_below_lines
#                                                                 (GtkTextView *text_view,
#                                                                  gint pixels_below_lines);
#                     gint        gtk_text_view_get_pixels_below_lines
#                                                                 (GtkTextView *text_view);
#                     void        gtk_text_view_set_pixels_inside_wrap
#                                                                 (GtkTextView *text_view,
#                                                                  gint pixels_inside_wrap);
#                     gint        gtk_text_view_get_pixels_inside_wrap
#                                                                 (GtkTextView *text_view);
#                     void        gtk_text_view_set_justification (GtkTextView *text_view,
#                                                                  GtkJustification justification);
#                     GtkJustification gtk_text_view_get_justification
#                                                                 (GtkTextView *text_view);
#                     void        gtk_text_view_set_left_margin   (GtkTextView *text_view,
#                                                                  gint left_margin);
#                     gint        gtk_text_view_get_left_margin   (GtkTextView *text_view);
#                     void        gtk_text_view_set_right_margin  (GtkTextView *text_view,
#                                                                  gint right_margin);
#                     gint        gtk_text_view_get_right_margin  (GtkTextView *text_view);
#                     void        gtk_text_view_set_indent        (GtkTextView *text_view,
#                                                                  gint indent);
#                     gint        gtk_text_view_get_indent        (GtkTextView *text_view);
#                     void        gtk_text_view_set_tabs          (GtkTextView *text_view,
#                                                                  PangoTabArray *tabs);
#                     PangoTabArray* gtk_text_view_get_tabs       (GtkTextView *text_view);
#                     void        gtk_text_view_set_accepts_tab   (GtkTextView *text_view,
#                                                                  gboolean accepts_tab);
#                     gboolean    gtk_text_view_get_accepts_tab   (GtkTextView *text_view);
#                     GtkTextAttributes* gtk_text_view_get_default_attributes
#                                                                 (GtkTextView *text_view);

                class GtkToolbar:
                    __new__ = Function(py_name='gtk.Toolbar')
                    insert = [IsGtkToolItem, gint]
                    get_item_index = [IsGtkToolItem]
                    get_n_items = []
                    get_nth_item = [gint]
                    set_show_arrow = [gboolean]
                    get_drop_index = [gint, gint]
                    set_drop_highlight_item = [IsGtkToolItem, gint]
                    set_orientation = [GtkOrientation]
                    set_tooltips = [gboolean]
                    unset_icon_size = []
                    get_show_arrow = []
                    get_orientation = []
                    # get_style matches GtkWidget method
                    get_style = Function(py_name='get_style', gap_name='GetToolbarStyle')
                    get_icon_size = []
                    get_tooltips = []
                    get_relief_style = []
                    set_style = [GtkToolbarStyle]
                    set_icon_size = [GtkIconSize]
                    unset_style = []
                class GtkTreeView:
                    __new__ = Function(py_name='gtk.TreeView', opt_args=[(IsGtkTreeModel, 'model')])
                    get_model = []
                    set_model = [(IsGtkTreeModel, 'model')]
                    get_selection = []
                    get_hadjustment = []
                    set_hadjustment = [IsGtkAdjustment]
                    get_vadjustment = []
                    set_vadjustment = [IsGtkAdjustment]
                    get_headers_visible = []
                    set_headers_visible = [gboolean]
                    columns_autosize = []
                    set_headers_clickable = [gboolean]
                    set_rules_hint = [gboolean]
                    get_rules_hint = []
                    append_column = [IsGtkTreeViewColumn]
                    remove_column = [IsGtkTreeViewColumn]
                    insert_column = [IsGtkTreeViewColumn, gint]
#                     gint        gtk_tree_view_insert_column_with_attributes
#                                                                 (GtkTreeView *tree_view,
#                                                                  gint position,
#                                                                  const gchar *title,
#                                                                  GtkCellRenderer *cell,
#                                                                  ...);
#                     gint        gtk_tree_view_insert_column_with_data_func
#                                                                 (GtkTreeView *tree_view,
#                                                                  gint position,
#                                                                  const gchar *title,
#                                                                  GtkCellRenderer *cell,
#                                                                  GtkTreeCellDataFunc func,
#                                                                  gpointer data,
#                                                                  GDestroyNotify dnotify);
                    get_column = [gint]
                    get_columns = []
                    move_column_after = [IsGtkTreeViewColumn, IsGtkTreeViewColumn]
                    set_expander_column = [IsGtkTreeViewColumn]
                    get_expander_column = []
#                     void        gtk_tree_view_set_column_drag_function
#                                                                 (GtkTreeView *tree_view,
#                                                                  GtkTreeViewColumnDropFunc func,
#                                                                  gpointer user_data,
#                                                                  GtkDestroyNotify destroy);
                    scroll_to_point = [gint, gint]
#                     void        gtk_tree_view_scroll_to_cell    (GtkTreeView *tree_view,
#                                                                  GtkTreePath *path,
#                                                                  GtkTreeViewColumn *column,
#                                                                  gboolean use_align,
#                                                                  gfloat row_align,
#                                                                  gfloat col_align);
#                     void        gtk_tree_view_set_cursor        (GtkTreeView *tree_view,
#                                                                  GtkTreePath *path,
#                                                                  GtkTreeViewColumn *focus_column,
#                                                                  gboolean start_editing);
#                     void        gtk_tree_view_set_cursor_on_cell
#                                                                 (GtkTreeView *tree_view,
#                                                                  GtkTreePath *path,
#                                                                  GtkTreeViewColumn *focus_column,
#                                                                  GtkCellRenderer *focus_cell,
#                                                                  gboolean start_editing);
#                     void        gtk_tree_view_get_cursor        (GtkTreeView *tree_view,
#                                                                  GtkTreePath **path,
#                                                                  GtkTreeViewColumn **focus_column);
#                     void        gtk_tree_view_row_activated     (GtkTreeView *tree_view,
#                                                                  GtkTreePath *path,
#                                                                  GtkTreeViewColumn *column);
                    expand_all = []
                    collapse_all = []
#                     void        gtk_tree_view_expand_to_path    (GtkTreeView *tree_view,
#                                                                  GtkTreePath *path);
#                     gboolean    gtk_tree_view_expand_row        (GtkTreeView *tree_view,
#                                                                  GtkTreePath *path,
#                                                                  gboolean open_all);
#                     gboolean    gtk_tree_view_collapse_row      (GtkTreeView *tree_view,
#                                                                  GtkTreePath *path);
#                     void        gtk_tree_view_map_expanded_rows (GtkTreeView *tree_view,
#                                                                  GtkTreeViewMappingFunc func,
#                                                                  gpointer data);
#                     gboolean    gtk_tree_view_row_expanded      (GtkTreeView *tree_view,
#                                                                  GtkTreePath *path);
                    set_reorderable = [gboolean]
                    get_reorderable = []
#                     gboolean    gtk_tree_view_get_path_at_pos   (GtkTreeView *tree_view,
#                                                                  gint x,
#                                                                  gint y,
#                                                                  GtkTreePath **path,
#                                                                  GtkTreeViewColumn **column,
#                                                                  gint *cell_x,
#                                                                  gint *cell_y);
#                     void        gtk_tree_view_get_cell_area     (GtkTreeView *tree_view,
#                                                                  GtkTreePath *path,
#                                                                  GtkTreeViewColumn *column,
#                                                                  GdkRectangle *rect);
#                     void        gtk_tree_view_get_background_area
#                                                                 (GtkTreeView *tree_view,
#                                                                  GtkTreePath *path,
#                                                                  GtkTreeViewColumn *column,
#                                                                  GdkRectangle *rect);
#                     void        gtk_tree_view_get_visible_rect  (GtkTreeView *tree_view,
#                                                                  GdkRectangle *visible_rect);
#                     gboolean    gtk_tree_view_get_visible_range (GtkTreeView *tree_view,
#                                                                  GtkTreePath **start_path,
#                                                                  GtkTreePath **end_path);
                    get_bin_window = []
                    widget_to_tree_coords = [gint, gint]
                    tree_to_widget_coords = [gint, gint]
#                     void        gtk_tree_view_enable_model_drag_dest
#                                                                 (GtkTreeView *tree_view,
#                                                                  const GtkTargetEntry *targets,
#                                                                  gint n_targets,
#                                                                  GdkDragAction actions);
#                     void        gtk_tree_view_enable_model_drag_source
#                                                                 (GtkTreeView *tree_view,
#                                                                  GdkModifierType start_button_mask,
#                                                                  const GtkTargetEntry *targets,
#                                                                  gint n_targets,
#                                                                  GdkDragAction actions);
#                     void        gtk_tree_view_unset_rows_drag_source
#                                                                 (GtkTreeView *tree_view);
#                     void        gtk_tree_view_unset_rows_drag_dest
#                                                                 (GtkTreeView *tree_view);
#                     void        gtk_tree_view_set_drag_dest_row (GtkTreeView *tree_view,
#                                                                  GtkTreePath *path,
#                                                                  GtkTreeViewDropPosition pos);
#                     void        gtk_tree_view_get_drag_dest_row (GtkTreeView *tree_view,
#                                                                  GtkTreePath **path,
#                                                                  GtkTreeViewDropPosition *pos);
#                     gboolean    gtk_tree_view_get_dest_row_at_pos
#                                                                 (GtkTreeView *tree_view,
#                                                                  gint drag_x,
#                                                                  gint drag_y,
#                                                                  GtkTreePath **path,
#                                                                  GtkTreeViewDropPosition *pos);
#                     GdkPixmap*  gtk_tree_view_create_row_drag_icon
#                                                                 (GtkTreeView *tree_view,
#                                                                  GtkTreePath *path);
                    set_enable_search = [gboolean]
                    get_enable_search = []
                    get_search_column = []
                    set_search_column = [gint]
#                     GtkTreeViewSearchEqualFunc gtk_tree_view_get_search_equal_func
#                                                                 (GtkTreeView *tree_view);
#                     void        gtk_tree_view_set_search_equal_func
#                                                                 (GtkTreeView *tree_view,
#                                                                  GtkTreeViewSearchEqualFunc search_equal_func,
#                                                                  gpointer search_user_data,
#                                                                  GtkDestroyNotify search_destroy);
                    get_fixed_height_mode = []
                    set_fixed_height_mode = [gboolean]
                    get_hover_selection = []
                    set_hover_selection = [gboolean]
                    get_hover_expand = []
                    set_hover_expand = [gboolean]
#                     gboolean    (*GtkTreeViewRowSeparatorFunc)  (GtkTreeModel *model,
#                                                                  GtkTreeIter *iter,
#                                                                  gpointer data);
#                     GtkTreeViewRowSeparatorFunc gtk_tree_view_get_row_separator_func
#                                                                 (GtkTreeView *tree_view);
#                     void        gtk_tree_view_set_row_separator_func
#                                                                 (GtkTreeView *tree_view,
#                                                                  GtkTreeViewRowSeparatorFunc func,
#                                                                  gpointer data,
#                                                                  GtkDestroyNotify destroy);
            class GtkMisc:
                __no_constructor__ = True
                class GtkLabel:
                    # TODO
                    class GtkAccelLabel: pass
                class GtkArrow: pass
                    # TODO
            class GtkCalendar: pass
            class GtkDrawingArea:
                class GtkCurve: pass
            class GtkEntry:
                __new__ = Function(py_name='gtk.Entry', opt_args=[(gint, 'maxlen')])
                set_text = [(IsString, 'text')]
                append_text = [(IsString, 'text')]
                prepend_text = [(IsString, 'text')]
                set_position = [(gint, 'position')]
                get_text = []
                select_region = [(gint, 'start'), (gint, 'end')]
                set_visibility = [(gboolean, 'visible')]
#                 set_invisible_char    (GtkEntry *entry,
#                                                              gunichar ch);
                set_editable = [(gboolean, 'editable')]
                set_max_length = [(gint, 'max_len')]
                get_activates_default = []
                get_has_frame = []
                get_width_chars = []
                set_activates_default = [(gboolean, 'activates_default')]
                set_has_frame = [(gboolean, 'has_frame')]
                set_width_chars = [(gint, 'n_chars')]
                get_invisible_char = []
                set_alignment = [(gfloat, 'xalign')]
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
    class GtkTooltips: pass
                    # TODO
    class GtkTreeViewColumn: pass
                    # TODO
    class GtkListStore:
        __implements__ = ['GtkTreeSortable']
        __new__ = 'doc_stub'
        # TODO
    class GtkTextBuffer: pass
    class GtkTreeModelSort:
        __implements__ = ['GtkTreeSortable']
        # TODO
    class GtkTreeSelection:
        __no_constructor__ = True
        set_mode = [GtkSelectionMode]
        get_mode = []
#         void        gtk_tree_selection_set_select_function
#                                                     (GtkTreeSelection *selection,
#                                                      GtkTreeSelectionFunc func,
#                                                      gpointer data,
#                                                      GtkDestroyNotify destroy);
#         gpointer    gtk_tree_selection_get_user_data
#                                                     (GtkTreeSelection *selection);
        get_tree_view = []
        get_selected = []
#         void        gtk_tree_selection_selected_foreach
#                                                     (GtkTreeSelection *selection,
#                                                      GtkTreeSelectionForeachFunc func,
#                                                      gpointer data);
        get_selected_rows = []
        count_selected_rows = []
#         void        gtk_tree_selection_select_path  (GtkTreeSelection *selection,
#                                                      GtkTreePath *path);
#         void        gtk_tree_selection_unselect_path
#                                                     (GtkTreeSelection *selection,
#                                                      GtkTreePath *path);
#         gboolean    gtk_tree_selection_path_is_selected
#                                                     (GtkTreeSelection *selection,
#                                                      GtkTreePath *path);
#         void        gtk_tree_selection_select_iter  (GtkTreeSelection *selection,
#                                                      GtkTreeIter *iter);
#         void        gtk_tree_selection_unselect_iter
#                                                     (GtkTreeSelection *selection,
#                                                      GtkTreeIter *iter);
#         gboolean    gtk_tree_selection_iter_is_selected
#                                                     (GtkTreeSelection *selection,
#                                                      GtkTreeIter *iter);
        select_all = []
        unselect_all = []
#         void        gtk_tree_selection_select_range (GtkTreeSelection *selection,
#                                                      GtkTreePath *start_path,
#                                                      GtkTreePath *end_path);
#         void        gtk_tree_selection_unselect_range
#                                                     (GtkTreeSelection *selection,
#                                                      GtkTreePath *start_path,
#                                                      GtkTreePath *end_path);
    class GtkTreeStore:
        __implements__ = ['GtkTreeSortable']
        __new__ = 'doc_stub'
        # TODO

    class MooGladeXML:
        __no_constructor__ = True
        __py_name__ = 'moo.utils.GladeXML'
        __gap_name__ = 'IsGladeXML'
        __new__ = 'doc_stub'
        get_widget = [IsString]
        get_root = []


top_classes.append(GObject)


functions = [
    Function(py_name='gtk.window_list_toplevels', gap_name='WindowListToplevels'),
    Function(py_name='gtk.window_set_default_icon_from_file', gap_name='WindowSetDefaultIconFromFile', args=[IsString]),
    Function(py_name='gtk.window_set_default_icon_name', gap_name='WindowSetDefaultIconName', args=[IsString]),
    Function(py_name='gtk.gdk.screen_get_default', gap_name='ScreenGetDefault'),
    Function(py_name='gtk.gdk.display_get_default', gap_name='DisplayGetDefault'),
]


top_classes = [ClassInfo(c, None) for c in top_classes]
functions = [Function(f) for f in functions]
for f in functions:
    f.is_meth = False
