from defs import Function, ClassInfo

IsGtkWidget = 'IsGtkWidget'
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

top_classes = []
functions = []

class GtkCellEditable:
    __abstract__ = True
    # TODO
top_classes.append(GtkCellEditable)
class GtkCellLayout:
    __abstract__ = True
    # TODO
top_classes.append(GtkCellLayout)
class GtkEditable:
    __abstract__ = True
    # TODO
top_classes.append(GtkEditable)
class GtkFileChooser:
    __abstract__ = True
    # TODO
top_classes.append(GtkFileChooser)
class GtkTreeModel:
    __abstract__ = True
    # TODO
top_classes.append(GtkTreeModel)
class GtkTreeDragSource:
    __abstract__ = True
    # TODO
top_classes.append(GtkTreeDragSource)
class GtkTreeDragDest:
    __abstract__ = True
    # TODO
top_classes.append(GtkTreeDragDest)
class GtkTreeSortable:
    __abstract__ = True
    __implements__ = ['GtkTreeModel']
    # TODO
top_classes.append(GtkTreeSortable)

class GObject:
    __abstract__ = True

    set_property = ['IsString', 'IsObject']
    get_property = ['IsString']

    class GtkObject:
        __abstract__ = True

        destroy = []

        class GtkWidget:
            __abstract__ = True

            unparent = []
            show = []
            show_now = []
            hide = []
            show_all = []
            hide_all = []
            map = []
            unmap = []
            realize = []
            unrealize = []
            queue_draw = []
            queue_resize = []
            queue_resize_no_redraw = []
#             draw                 (GtkWidget *widget,
#                                                          GdkRectangle *area);
#             size_request         (GtkWidget *widget,
#                                                          GtkRequisition *requisition);
#             get_child_requisition
#                                                         (GtkWidget *widget,
#                                                          GtkRequisition *requisition);
#             size_allocate        (GtkWidget *widget,
#                                                          GtkAllocation *allocation);
#             add_accelerator      (GtkWidget *widget,
#                                                          const gchar *accel_signal,
#                                                          GtkAccelGroup *accel_group,
#                                                          guint accel_key,
#                                                          GdkModifierType accel_mods,
#                                                          GtkAccelFlags accel_flags);
#             gboolean    gtk_widget_remove_accelerator   (GtkWidget *widget,
#                                                          GtkAccelGroup *accel_group,
#                                                          guint accel_key,
#                                                          GdkModifierType accel_mods);
#             set_accel_path       (GtkWidget *widget,
#                                                          const gchar *accel_path,
#                                                          GtkAccelGroup *accel_group);
#             GList*      gtk_widget_list_accel_closures = []
#             gboolean    gtk_widget_can_activate_accel   (GtkWidget *widget,
#                                                          guint signal_id);
#             gboolean    gtk_widget_event                (GtkWidget *widget,
#                                                          GdkEvent *event);
            activate = []
            reparent = [IsGtkWidget]
#             gboolean    gtk_widget_intersect            (GtkWidget *widget,
#                                                          GdkRectangle *area,
#                                                          GdkRectangle *intersection);
            is_focus = []
            grab_focus = []
            grab_default = []
            set_name = Function([IsString], other=True)
            get_name = []
            set_state = [GtkStateType]
            set_sensitive = [gboolean]
            set_parent = Function([IsGtkWidget], other=True)
#             set_parent_window = ['IsGdkWindow']
            get_parent_window = []
            set_events = [gint]
            add_events = [gint]
#             set_extension_events (GtkWidget *widget,
#                                                          GdkExtensionMode mode);
#             GdkExtensionMode gtk_widget_get_extension_events
            get_toplevel = []
#             get_ancestor         (GtkWidget *widget,
#                                                          GType widget_type);
#             GdkColormap* gtk_widget_get_colormap = []
#             set_colormap         (GtkWidget *widget,
#                                                          GdkColormap *colormap);
#             GdkVisual*  gtk_widget_get_visual = []
            get_events = []
#             get_pointer          (GtkWidget *widget,
#                                                          gint *x,
#                                                          gint *y);
            is_ancestor = [IsGtkWidget]
#             gboolean    gtk_widget_translate_coordinates
#                                                         (GtkWidget *src_widget,
#                                                          GtkWidget *dest_widget,
#                                                          gint src_x,
#                                                          gint src_y,
#                                                          gint *dest_x,
#                                                          gint *dest_y);
#             set_style            (GtkWidget *widget,
#                                                          GtkStyle *style);
            #define     gtk_widget_set_rc_style         (widget)
            ensure_style = []
            get_style = []
            reset_rc_styles = []
#             push_colormap        (GdkColormap *cmap);
#             pop_colormap         (void);
#             set_default_colormap (GdkColormap *colormap);
#             GtkStyle*   gtk_widget_get_default_style    (void);
#             GdkColormap* gtk_widget_get_default_colormap
#                                                         (void);
#             GdkVisual*  gtk_widget_get_default_visual   (void);
#             set_direction        (GtkWidget *widget,
#                                                          GtkTextDirection dir);
#             enum        GtkTextDirection;
#             GtkTextDirection gtk_widget_get_direction = []
#             set_default_direction
#                                                         (GtkTextDirection dir);
#             GtkTextDirection gtk_widget_get_default_direction
#                                                         (void);
#             shape_combine_mask   (GtkWidget *widget,
#                                                          GdkBitmap *shape_mask,
#                                                          gint offset_x,
#                                                          gint offset_y);
#             path                 (GtkWidget *widget,
#                                                          guint *path_length,
#                                                          gchar **path,
#                                                          gchar **path_reversed);
#             class_path           (GtkWidget *widget,
#                                                          guint *path_length,
#                                                          gchar **path,
#                                                          gchar **path_reversed);
#             gchar*      gtk_widget_get_composite_name = []
#             modify_style         (GtkWidget *widget,
#                                                          GtkRcStyle *style);
#             GtkRcStyle* gtk_widget_get_modifier_style = []
#             modify_fg = [GtkStateType, GdkColor]
#             modify_bg = [GtkStateType, GdkColor]
#             modify_text = [GtkStateType, GdkColor]
#             modify_base = [GtkStateType, GdkColor]
#             modify_font          (GtkWidget *widget,
#                                                          PangoFontDescription *font_desc);
#             PangoContext* gtk_widget_create_pango_context
#             PangoContext* gtk_widget_get_pango_context = []
#             PangoLayout* gtk_widget_create_pango_layout (GtkWidget *widget,
#                                                          const gchar *text);
#             GdkPixbuf*  gtk_widget_render_icon          (GtkWidget *widget,
#                                                          const gchar *stock_id,
#                                                          GtkIconSize size,
#                                                          const gchar *detail);
#             pop_composite_child  (void);
#             push_composite_child (void);
#             queue_clear = []
#             queue_clear_area     (GtkWidget *widget,
#                                                          gint x,
#                                                          gint y,
#                                                          gint width,
#                                                          gint height);
#             queue_draw_area      (GtkWidget *widget,
#                                                          gint x,
#                                                          gint y,
#                                                          gint width,
#                                                          gint height);
#             reset_shapes = []
#             set_app_paintable    (GtkWidget *widget,
#                                                          gboolean app_paintable);
#             set_double_buffered  (GtkWidget *widget,
#                                                          gboolean double_buffered);
#             set_redraw_on_allocate
#                                                         (GtkWidget *widget,
#                                                          gboolean redraw_on_allocate);
#             set_composite_name   (GtkWidget *widget,
#                                                          const gchar *name);
#             gboolean    gtk_widget_set_scroll_adjustments
#                                                         (GtkWidget *widget,
#                                                          GtkAdjustment *hadjustment,
#                                                          GtkAdjustment *vadjustment);
#             gboolean    gtk_widget_mnemonic_activate    (GtkWidget *widget,
#                                                          gboolean group_cycling);
#             class_install_style_property
#                                                         (GtkWidgetClass *klass,
#                                                          GParamSpec *pspec);
#             class_install_style_property_parser
#                                                         (GtkWidgetClass *klass,
#                                                          GParamSpec *pspec,
#                                                          GtkRcPropertyParser parser);
#             GParamSpec* gtk_widget_class_find_style_property
#                                                         (GtkWidgetClass *klass,
#                                                          const gchar *property_name);
#             GParamSpec** gtk_widget_class_list_style_properties
#                                                         (GtkWidgetClass *klass,
#                                                          guint *n_properties);
#             GdkRegion*  gtk_widget_region_intersect     (GtkWidget *widget,
#                                                          GdkRegion *region);
#             gint        gtk_widget_send_expose          (GtkWidget *widget,
#                                                          GdkEvent *event);
#             style_get            (GtkWidget *widget,
#                                                          const gchar *first_property_name,
#                                                          ...);
#             style_get_property   (GtkWidget *widget,
#                                                          const gchar *property_name,
#                                                          GValue *value);
#             style_get_valist     (GtkWidget *widget,
#                                                          const gchar *first_property_name,
#                                                          va_list var_args);
#             AtkObject*  gtk_widget_get_accessible = []
#             gboolean    gtk_widget_child_focus          (GtkWidget *widget,
#                                                          GtkDirectionType direction);
#             child_notify         (GtkWidget *widget,
#                                                          const gchar *child_property);
#             freeze_child_notify = []
            get_child_visible = []
            get_parent = []
            get_settings = []
#             get_clipboard      (GtkWidget *widget,
#                                                          GdkAtom selection);
            get_display = []
            get_root_window = []
            get_screen = []
            has_screen = []
            get_size_request = []
            set_child_visible = [gboolean]
            set_size_request = [gint, gint]
#             thaw_child_notify = []
#             set_no_show_all      (GtkWidget *widget,
#                                                          gboolean no_show_all);
#             gboolean    gtk_widget_get_no_show_all = []
#             GList*      gtk_widget_list_mnemonic_labels = []
#             add_mnemonic_label   (GtkWidget *widget,
#                                                          GtkWidget *label);
#             remove_mnemonic_label
#                                                         (GtkWidget *widget,
#                                                          GtkWidget *label);

            class GtkContainer:
                __abstract__ = True

                add = Function([IsGtkWidget], other=True)
                remove = Function([IsGtkWidget], other=True)
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
#                 set_reallocate_redraws
#                                                             (GtkContainer *container,
#                                                              gboolean needs_redraws);
#                 set_focus_child   (GtkContainer *container,
#                                                              GtkWidget *child);
#                 get_focus_vadjustment = []
#                 set_focus_vadjustment
#                                                             (GtkContainer *container,
#                                                              GtkAdjustment *adjustment);
#                 get_focus_hadjustment = []
#                 set_focus_hadjustment
#                                                             (GtkContainer *container,
#                                                              GtkAdjustment *adjustment);
                resize_children = []
#                 child_type = []
#                 child_get         (GtkContainer *container,
#                                                              GtkWidget *child,
#                                                              const gchar *first_prop_name,
#                                                              ...);
#                 child_set         (GtkContainer *container,
#                                                              GtkWidget *child,
#                                                              const gchar *first_prop_name,
#                                                              ...);
#                 child_get_property
#                                                             (GtkContainer *container,
#                                                              GtkWidget *child,
#                                                              const gchar *property_name,
#                                                              GValue *value);
#                 child_set_property
#                                                             (GtkContainer *container,
#                                                              GtkWidget *child,
#                                                              const gchar *property_name,
#                                                              const GValue *value);
#                 forall            (GtkContainer *container,
#                                                              GtkCallback callback,
#                                                              gpointer callback_data);
                get_border_width = []
                set_border_width = [guint]
#                 propagate_expose  (GtkContainer *container,
#                                                              GtkWidget *child,
#                                                              GdkEventExpose *event);
#                 get_focus_chain   (GtkContainer *container,
#                                                              GList **focusable_widgets);
#                 set_focus_chain   (GtkContainer *container,
#                                                              GList *focusable_widgets);
#                 unset_focus_chain = []
#                 GParamSpec* gtk_container_class_find_child_property
#                                                             (GObjectClass *cclass,
#                                                              const gchar *property_name);
#                 class_install_child_property
#                                                             (GtkContainerClass *cclass,
#                                                              guint property_id,
#                                                              GParamSpec *pspec);
#                 GParamSpec** gtk_container_class_list_child_properties
#                                                             (GObjectClass *cclass,
#                                                              guint *n_properties);

                class GtkBin:
                    __abstract__ = True

                    get_child = []

                    class GtkWindow:
                        __new__ = Function(py_name='gtk.Window', opt_args=[GtkWindowType])

#                         GtkWidget*  gtk_window_new                  (GtkWindowType type);
                        set_title = [IsString]
#                         set_wmclass          (GtkWindow *window,
#                                                                      const gchar *wmclass_name,
#                                                                      const gchar *wmclass_class);
                        set_resizable = [gboolean]
                        get_resizable = []
#                         add_accel_group      (GtkWindow *window,
#                                                                      GtkAccelGroup *accel_group);
#                         remove_accel_group   (GtkWindow *window,
#                                                                      GtkAccelGroup *accel_group);
                        activate_focus = []
                        activate_default = []
                        set_modal = [gboolean]
                        set_default_size = [gint, gint]
#                         set_geometry_hints   (GtkWindow *window,
#                                                                      GtkWidget *geometry_widget,
#                                                                      GdkGeometry *geometry,
#                                                                      GdkWindowHints geom_mask);
#                         set_gravity          (GtkWindow *window,
#                                                                      GdkGravity gravity);
#                         GdkGravity  gtk_window_get_gravity = []
                        set_position = [GtkWindowPosition]
                        set_transient_for = ['IsGtkWindow']
                        set_destroy_with_parent = [gboolean]
#                         set_screen           (GtkWindow *window,
#                                                                      GdkScreen *screen);
                        # same as gtk_widget_get_screen?
                        # get_screen = []
                        is_active = []
                        has_toplevel_focus = []
#                         GList*      gtk_window_list_toplevels       (void);
#                         add_mnemonic         (GtkWindow *window,
#                                                                      guint keyval,
#                                                                      GtkWidget *target);
#                         remove_mnemonic      (GtkWindow *window,
#                                                                      guint keyval,
#                                                                      GtkWidget *target);
#                         mnemonic_activate    (GtkWindow *window,
#                                                                      guint keyval,
#                                                                      GdkModifierType modifier);
#                         activate_key         (GtkWindow *window,
#                                                                      GdkEventKey *event);
#                         propagate_key_event  (GtkWindow *window,
#                                                                      GdkEventKey *event);
                        get_focus = []
                        set_focus = [IsGtkWidget]
                        set_default = [IsGtkWidget]
                        present = []
#                         present_with_time    (GtkWindow *window,
#                                                                      guint32 timestamp);
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
#                         begin_resize_drag    (GtkWindow *window,
#                                                                      GdkWindowEdge edge,
#                                                                      gint button,
#                                                                      gint root_x,
#                                                                      gint root_y,
#                                                                      guint32 timestamp);
#                         begin_move_drag      (GtkWindow *window,
#                                                                      gint button,
#                                                                      gint root_x,
#                                                                      gint root_y,
#                                                                      guint32 timestamp);
                        set_decorated = [gboolean]
#                         set_frame_dimensions (GtkWindow *window,
#                                                                      gint left,
#                                                                      gint top,
#                                                                      gint right,
#                                                                      gint bottom);
                        set_has_frame = [gboolean]
#                         set_mnemonic_modifier
#                                                                     (GtkWindow *window,
#                                                                      GdkModifierType modifier);
                        set_role = [IsString]
                        set_type_hint = [GdkWindowTypeHint]
                        set_skip_taskbar_hint = [gboolean]
                        set_skip_pager_hint = [gboolean]
                        set_urgency_hint = [gboolean]
                        set_accept_focus = [gboolean]
                        set_focus_on_map = [gboolean]
                        get_decorated = []
#                         GList*      gtk_window_get_default_icon_list
#                                                                     (void);
                        get_default_size = []
                        get_destroy_with_parent = []
#                         get_frame_dimensions (GtkWindow *window,
#                                                                      gint *left,
#                                                                      gint *top,
#                                                                      gint *right,
#                                                                      gint *bottom);
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
#                         set_default_icon_list
#                                                                     (GList *list);
#                         set_default_icon     (GdkPixbuf *icon);
#                         set_default_icon_from_file
#                                                                     (const gchar *filename,
#                                                                      GError **err);
#                         set_default_icon_name
#                                                                     (const gchar *name);
#                         set_icon             (GtkWindow *window,
#                                                                      GdkPixbuf *icon);
#                         set_icon_list        (GtkWindow *window,
#                                                                      GList *list);
#                         set_icon_from_file   (GtkWindow *window,
#                                                                      const gchar *filename,
#                                                                      GError **err);
#                         set_icon_name        (GtkWindow *window,
#                                                                      const gchar *name);
#                         set_auto_startup_notification
#                                                                     (gboolean setting);

                        class GtkDialog:
                            __new__ = Function(py_name="gtk.Dialog",
                                               opt_args=[IsString, 'IsGtkWindow', GtkDialogFlags, IsList])
#                             GtkWidget*  gtk_dialog_new                  (void);
#                             GtkWidget*  gtk_dialog_new_with_buttons     (const gchar *title,
#                                                                          GtkWindow *parent,
#                                                                          GtkDialogFlags flags,
#                                                                          const gchar *first_button_text,
#                                                                          ...);
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

#                             gboolean    gtk_alternative_dialog_button_order
#                                                                         (GdkScreen *screen);
#                             set_alternative_button_order
#                                                                         (GtkDialog *dialog,
#                                                                          gint first_response_id,
#                                                                          ...);
#                             set_alternative_button_order_from_array
#                                                                         (GtkDialog *dialog,
#                                                                          gint n_params,
#                                                                          gint *new_order);

                            class GtkAboutDialog: pass
#                                 GtkWidget*  gtk_about_dialog_new            (void);
#                                 const gchar* gtk_about_dialog_get_name      (GtkAboutDialog *about);
#                                 void        gtk_about_dialog_set_name       (GtkAboutDialog *about,
#                                                                              const gchar *name);
#                                 const gchar* gtk_about_dialog_get_version   (GtkAboutDialog *about);
#                                 void        gtk_about_dialog_set_version    (GtkAboutDialog *about,
#                                                                              const gchar *version);
#                                 const gchar* gtk_about_dialog_get_copyright (GtkAboutDialog *about);
#                                 void        gtk_about_dialog_set_copyright  (GtkAboutDialog *about,
#                                                                              const gchar *copyright);
#                                 const gchar* gtk_about_dialog_get_comments  (GtkAboutDialog *about);
#                                 void        gtk_about_dialog_set_comments   (GtkAboutDialog *about,
#                                                                              const gchar *comments);
#                                 const gchar* gtk_about_dialog_get_license   (GtkAboutDialog *about);
#                                 void        gtk_about_dialog_set_license    (GtkAboutDialog *about,
#                                                                              const gchar *license);
#                                 gboolean    gtk_about_dialog_get_wrap_license
#                                                                             (GtkAboutDialog *about);
#                                 void        gtk_about_dialog_set_wrap_license
#                                                                             (GtkAboutDialog *about,
#                                                                              gboolean wrap_license);
#                                 const gchar* gtk_about_dialog_get_website   (GtkAboutDialog *about);
#                                 void        gtk_about_dialog_set_website    (GtkAboutDialog *about,
#                                                                              const gchar *website);
#                                 const gchar* gtk_about_dialog_get_website_label
#                                                                             (GtkAboutDialog *about);
#                                 void        gtk_about_dialog_set_website_label
#                                                                             (GtkAboutDialog *about,
#                                                                              const gchar *website_label);
#                                 const gchar* const * gtk_about_dialog_get_authors
#                                                                             (GtkAboutDialog *about);
#                                 void        gtk_about_dialog_set_authors    (GtkAboutDialog *about,
#                                                                              const gchar **authors);
#                                 const gchar* const * gtk_about_dialog_get_artists
#                                                                             (GtkAboutDialog *about);
#                                 void        gtk_about_dialog_set_artists    (GtkAboutDialog *about,
#                                                                              const gchar **artists);
#                                 const gchar* const * gtk_about_dialog_get_documenters
#                                                                             (GtkAboutDialog *about);
#                                 void        gtk_about_dialog_set_documenters
#                                                                             (GtkAboutDialog *about,
#                                                                              const gchar **documenters);
#                                 const gchar* gtk_about_dialog_get_translator_credits
#                                                                             (GtkAboutDialog *about);
#                                 void        gtk_about_dialog_set_translator_credits
#                                                                             (GtkAboutDialog *about,
#                                                                              const gchar *translator_credits);
#                                 GdkPixbuf*  gtk_about_dialog_get_logo       (GtkAboutDialog *about);
#                                 void        gtk_about_dialog_set_logo       (GtkAboutDialog *about,
#                                                                              GdkPixbuf *logo);
#                                 const gchar* gtk_about_dialog_get_logo_icon_name
#                                                                             (GtkAboutDialog *about);
#                                 void        gtk_about_dialog_set_logo_icon_name
#                                                                             (GtkAboutDialog *about,
#                                                                              const gchar *icon_name);
#                                 void        (*GtkAboutDialogActivateLinkFunc)
#                                                                             (GtkAboutDialog *about,
#                                                                              const gchar *link,
#                                                                              gpointer data);
#                                 GtkAboutDialogActivateLinkFunc gtk_about_dialog_set_email_hook
#                                                                             (GtkAboutDialogActivateLinkFunc func,
#                                                                              gpointer data,
#                                                                              GDestroyNotify destroy);
#                                 GtkAboutDialogActivateLinkFunc gtk_about_dialog_set_url_hook
#                                                                             (GtkAboutDialogActivateLinkFunc func,
#                                                                              gpointer data,
#                                                                              GDestroyNotify destroy);
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
                            class GtkInputDialog: pass
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
                        __abstract__ = True
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
                        set_model = ['IsGtkTreeModel']
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
                        set_tooltip = ['IsGtkTooltips', IsString, IsString]
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
                                           args=['IsGtkAdjustment', 'IsGtkAdjustment'])
                        get_hadjustment = []
                        get_vadjustment = []
                        get_hscrollbar = []
                        get_vscrollbar = []
                        set_policy = [GtkPolicyType, GtkPolicyType]
                        add_with_viewport = [IsGtkWidget]
                        set_placement = [GtkCornerType]
                        set_shadow_type = [GtkShadowType]
                        set_hadjustment = ['IsGtkAdjustment']
                        set_vadjustment = ['IsGtkAdjustment']
                        get_placement = []
                        get_policy = []
                        get_shadow_type = []

                    class GtkViewport:
                        __new__ = Function(py_name='gtk.Viewport', args=['IsGtkAdjustment', 'IsGtkAdjustment'])
                        get_hadjustment = []
                        get_vadjustment = []
                        set_hadjustment = ['IsGtkAdjustment']
                        set_vadjustment = ['IsGtkAdjustment']
                        set_shadow_type = [GtkShadowType]
                        get_shadow_type = []

                class GtkBox:
                    __abstract__ = True
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
                    __new__ = Function(py_name='gtk.TextView', opt_args=['IsGtkTextBuffer'])
                    set_buffer = ['IsGtkTextBuffer']
                    get_buffer = []
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
                    insert = ['IsGtkToolItem', gint]
                    get_item_index = ['IsGtkToolItem']
                    get_n_items = []
                    get_nth_item = [gint]
                    set_show_arrow = [gboolean]
                    get_drop_index = [gint, gint]
                    set_drop_highlight_item = ['IsGtkToolItem', gint]
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
                class GtkTreeView: pass
            class GtkMisc:
                __abstract__ = True
                class GtkLabel:
                    class GtkAccelLabel: pass
                class GtkArrow: pass
            class GtkCalendar: pass
            class GtkCellView: pass
            class GtkDrawingArea:
                class GtkCurve: pass
            class GtkEntry:
                class GtkSpinButton: pass
            class GtkRuler:
                class GtkHRuler: pass
                class GtkVRuler: pass
            class GtkRange:
                class GtkScale:
                    class GtkHScale: pass
                    class GtkVScale: pass
                class GtkScrollbar:
                    class GtkHScrollbar: pass
                    class GtkVScrollbar: pass
            class GtkSeparator:
                class GtkHSeparator: pass
                class GtkVSeparator: pass
            class GtkInvisible: pass
            class GtkPreview: pass
            class GtkProgress:
                class GtkProgressBar: pass
        class GtkAdjustment: pass
        class GtkCellRenderer:
            __abstract__ = True
            class GtkCellRendererText:
                class GtkCellRendererCombo: pass
            class GtkCellRendererPixbuf: pass
            class GtkCellRendererProgress: pass
            class GtkCellRendererToggle: pass
    class GtkFileFilter: pass
    class GtkTooltips: pass
    class GtkTreeViewColumn: pass
    class GtkAccelGroup: pass
    class GtkAccelMap: pass
    class GtkClipboard: pass
    class GtkEntryCompletion: pass
    class GtkIconFactory: pass
    class GtkIconTheme: pass
    class GtkListStore:
        __implements__ = ['GtkTreeSortable']
        # TODO
    class GtkRcStyle: pass
    class GtkSettings: pass
    class GtkSizeGroup: pass
    class GtkStyle: pass
    class GtkTextBuffer: pass
    class GtkTextChildAnchor: pass
    class GtkTextMark: pass
    class GtkTextTag: pass
    class GtkTextTagTable: pass
    class GtkTreeModelFilter:
        __implements__ = ['GtkTreeModel']
        # TODO
    class GtkTreeModelSort:
        __implements__ = ['GtkTreeSortable']
        # TODO
    class GtkTreeSelection:
        __abstract__ = True
        # TODO
    class GtkTreeStore:
        __implements__ = ['GtkTreeSortable']
        # TODO
    class GtkUIManager: pass
    class GtkWindowGroup: pass

    class MooGladeXML:
        __py_name__ = 'moo.utils.GladeXML'
#         MooGladeXML *moo_glade_xml_new_empty        (const char     *domain);
        __new__ = Function(py_name='moo.utils.GladeXML', gap_name='MooGladeXML',
                           args=[IsString], opt_args=[IsString])

#         void         moo_glade_xml_map_class        (MooGladeXML    *xml,
#                                                      const char     *class_name,
#                                                      GType           use_type);
#         void         moo_glade_xml_map_id           (MooGladeXML    *xml,
#                                                      const char     *id,
#                                                      GType           use_type);
#         void         moo_glade_xml_map_custom       (MooGladeXML    *xml,
#                                                      const char     *id,
#                                                      MooGladeCreateFunc func,
#                                                      gpointer        data);
#         void         moo_glade_xml_set_signal_func  (MooGladeXML    *xml,
#                                                      MooGladeSignalFunc func,
#                                                      gpointer        data);
#         void         moo_glade_xml_set_prop_func    (MooGladeXML    *xml,
#                                                      MooGladePropFunc func,
#                                                      gpointer        data);
#
#         void         moo_glade_xml_set_property     (MooGladeXML    *xml,
#                                                      const char     *widget,
#                                                      const char     *prop_name,
#                                                      const char     *value);
#
#         gboolean     moo_glade_xml_parse_file       (MooGladeXML    *xml,
#                                                      const char     *file,
#                                                      const char     *root,
#                                                      GError        **error);
#         gboolean     moo_glade_xml_parse_memory     (MooGladeXML    *xml,
#                                                      const char     *buffer,
#                                                      int             size,
#                                                      const char     *root,
#                                                      GError        **error);
#         gboolean     moo_glade_xml_fill_widget      (MooGladeXML    *xml,
#                                                      GtkWidget      *target,
#                                                      const char     *buffer,
#                                                      int             size,
#                                                      const char     *target_name,
#                                                      GError        **error);
#
#         gpointer     moo_glade_xml_get_widget       (MooGladeXML    *xml,
#                                                      const char     *id);
#         GtkWidget   *moo_glade_xml_get_root         (MooGladeXML    *xml);


top_classes.append(GObject)


top_classes = [ClassInfo(c, None) for c in top_classes]
functions = [Function(f) for f in functions]
