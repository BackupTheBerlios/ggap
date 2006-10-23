import sys
import psyco
psyco.full()

GtkStateType = 'IsInt'
GtkResizeMode = 'IsInt'
GdkWindowTypeHint = 'IsInt'
GtkShadowType = 'IsInt'
GtkReliefStyle = 'IsInt'

IsGtkWidget = 'IsGtkWidget'
IsString = 'IsString'
IsInt = 'IsInt'
gboolean = 'IsBool'
gint = 'IsInt'
guint = 'IsInt'
gfloat = 'IsRat'

install_meth_tmpl = """
%(install_func)s(%(op)s, "Default method", %(arg_types)s,
function(obj%(other_args)s)
  return _GGAP_CALL_METH(obj, "%(meth)s"%(other_args)s);
end);"""

class Method:
    def __init__(self, *args, **kwargs):
        if kwargs.has_key('name'):
            self.name = kwargs['name']
        if kwargs.has_key('other'):
            self.other = kwargs['other']
        else:
            self.other = False
        if args:
            if isinstance(args[0], str):
                self.args = list(args)
            elif isinstance(args[0], list) or isinstance(args[0], tuple):
                self.args = list(args[0])
            else:
                raise RuntimeError()
        else:
            self.args = []

    def __str__(self):
        return '<Method %s, args %s>' % (self.name, self.args)

    def get_gap_name(self):
        if not hasattr(self, 'gap_name'):
            self.gap_name = ''.join([c.capitalize() for c in self.name.split('_')])
        return self.gap_name

    def print_arg_list(self):
        return

    def declare(self, fp):
        if not self.other:
            args = '[' + ', '.join([self.obj_type] + self.args) + ']'
            print >> fp, 'DeclareOperation("%s", %s);' % (self.get_gap_name(), args)

    def install(self, fp):
        arg_types = '[' + ', '.join([self.obj_type] + self.args) + ']'

        if self.args:
            other_args = ', ' + ', '.join(['arg' + str(i) for i in range(1, len(self.args) + 1)])
        else:
            other_args = ''

        if self.other:
            install_func = 'InstallOtherMethod'
        else:
            install_func = 'InstallMethod'

        print >> fp, install_meth_tmpl % {'op': self.get_gap_name(), 'arg_types' : arg_types,
                                          'meth' : self.name, 'other_args' : other_args,
                                          'install_func' : install_func}

class GObject:
    set_property = ['IsString', 'IsObject']
    get_property = ['IsString']

    class GtkObject:
        destroy = []

        class GtkWidget:
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
            set_name = Method([IsString], other=True)
            get_name = []
            set_state = [GtkStateType]
            set_sensitive = [gboolean]
            set_parent = Method([IsGtkWidget], other=True)
            set_parent_window = ['IsGdkWindow']
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
                add = Method([IsGtkWidget], other=True)
                remove = Method([IsGtkWidget], other=True)
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
                    get_child = []

                    class GtkWindow:
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
#                         set_position         (GtkWindow *window,
#                                                                      GtkWindowPosition position);
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
#                             GtkWidget*  gtk_dialog_new                  (void);
#                             GtkWidget*  gtk_dialog_new_with_buttons     (const gchar *title,
#                                                                          GtkWindow *parent,
#                                                                          GtkDialogFlags flags,
#                                                                          const gchar *first_button_text,
#                                                                          ...);
                            run = []
                            response = [gint]
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
#                             GtkWidget*  gtk_font_button_new             (void);
#                             GtkWidget*  gtk_font_button_new_with_font   (const gchar *fontname);
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

#                         class GtkOptionMenu: pass

                    class GtkItem:
                        class GtkMenuItem:
#                             GtkWidget*  gtk_menu_item_new               (void);
#                             GtkWidget*  gtk_menu_item_new_with_label    (const gchar *label);
#                             GtkWidget*  gtk_menu_item_new_with_mnemonic (const gchar *label);
                            set_right_justified = [gboolean]
                            set_submenu = [IsGtkWidget]
                            set_accel_path = [IsString]
                            remove_submenu = []
                            select = []
                            deselect = []
#                             activate = []
#                             toggle_size_request
#                                                                         (GtkMenuItem *menu_item,
#                                                                          gint *requisition);
#                             toggle_size_allocate
#                                                                         (GtkMenuItem *menu_item,
#                                                                          gint allocation);
                            get_right_justified = []
                            get_submenu = []

                            class GtkCheckMenuItem:
#                                 GtkWidget*  gtk_check_menu_item_new         (void);
#                                 GtkWidget*  gtk_check_menu_item_new_with_label
#                                                                             (const gchar *label);
#                                 GtkWidget*  gtk_check_menu_item_new_with_mnemonic
#                                                                             (const gchar *label);
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
                        class GtkComboBoxEntry: pass
                    class GtkEventBox: pass
                    class GtkExpander: pass
                    class GtkHandleBox: pass
                    class GtkToolItem:
                        class GtkToolButton:
                            class GtkMenuToolButton: pass
                            class GtkToggleToolButton:
                                class GtkRadioToolButton: pass
                        class GtkSeparatorToolItem: pass
                    class GtkScrolledWindow: pass
                    class GtkViewport: pass
                class GtkBox:
                    class GtkButtonBox:
                        class GtkHButtonBox: pass
                        class GtkVButtonBox: pass
                    class GtkVBox:
                        class GtkColorSelection: pass
                        class GtkFileChooserWidget: pass
                        class GtkFontSelection: pass
                        class GtkGammaCurve: pass
                    class GtkHBox:
                        class GtkCombo: pass
                        class GtkFileChooserButton: pass
                        class GtkStatusbar: pass
                class GtkFixed: pass
                class GtkPaned:
                    class GtkHPaned: pass
                    class GtkVPaned: pass
                class GtkIconView: pass
                class GtkLayout: pass
                class GtkMenuShell:
                    class GtkMenuBar: pass
                    class GtkMenu: pass
                class GtkNotebook: pass
                class GtkSocket: pass
                class GtkTable: pass
                class GtkTextView: pass
                class GtkToolbar: pass
                class GtkTreeView: pass
            class GtkMisc:
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
    class GtkListStore: pass
    class GtkRcStyle: pass
    class GtkSettings: pass
    class GtkSizeGroup: pass
    class GtkStyle: pass
    class GtkTextBuffer: pass
    class GtkTextChildAnchor: pass
    class GtkTextMark: pass
    class GtkTextTag: pass
    class GtkTextTagTable: pass
    class GtkTreeModelFilter: pass
    class GtkTreeModelSort: pass
    class GtkTreeSelection: pass
    class GtkTreeStore: pass
    class GtkUIManager: pass
    class GtkWindowGroup: pass

    class MooGladeXML: pass


def isclass(name):
    return name.startswith('Gtk') or name.startswith('Moo')

def normalize(cls):
    for k in dir(cls):
        a = getattr(cls, k)
        if isclass(k):
            normalize(a)
        elif isinstance(a, list) or isinstance(a, tuple):
            if a:
                setattr(cls, k, Method(name=k))
            else:
                setattr(cls, k, Method(a, name=k))
        elif isinstance(a, Method) and not hasattr(a, 'name'):
            setattr(a, 'name', k)
        a = getattr(cls, k)
        if isinstance(a, Method):
            setattr(a, 'obj_type', 'Is' + cls.__name__)
normalize(GObject)


def catname(c):
    if c is None:
        return 'IsObject'
    else:
        return 'Is' + c.__name__

def typename(c):
    return c.__name__

def printcats(c, pc, fp):
    if c.__name__ not in ["GObject"]:
        print >> fp, 'DeclareCategory("%s", %s);' % (catname(c), catname(pc))

    for a in c.__dict__.keys():
        if isclass(a):
            printcats(c.__dict__[a], c, fp)

def convert_method_name(n):
    return ''.join([c.capitalize() for c in n.split('_')])

def print_args(c, m):
    return '[' + ', '.join([catname(c)] + m) + ']'

def printops(c, fp):
    for k in c.__dict__.keys():
        m = c.__dict__[k]
        if isinstance(m, Method):
            m.declare(fp)

    for k in c.__dict__.keys():
        if isclass(k):
            printops(c.__dict__[k], fp)

def printmeths(c, fp):
    for k in c.__dict__.keys():
        m = c.__dict__[k]
        if isinstance(m, Method):
            m.install(fp)

    for k in c.__dict__.keys():
        if isclass(k):
            printmeths(c.__dict__[k], fp)

def printtypes(c, fp):
    if c.__name__ not in ["GObject"]:
        print >> fp, '_GGAP_REGISTER_TYPE("%s", %s);' % (typename(c), catname(c))

    for a in c.__dict__.keys():
        if isclass(a):
            printtypes(c.__dict__[a], fp)

def write_gd(fp):
    print >> fp, "## This file is autogenerated"
    print >> fp, ''
    printcats(GObject, None, fp)
    print >> fp, ''
    printops(GObject, fp)
    print >> fp, ''
    print >> fp, 'DeclareGlobalFunction("_GGAP_REGISTER_WIDGETS");'

def write_gi(fp):
    print >> fp, "## This file is autogenerated"
    printmeths(GObject, fp);
    print >> fp, ''
    print >> fp, "InstallGlobalFunction(_GGAP_REGISTER_WIDGETS,"
    print >> fp, "function()"
    printtypes(GObject, fp);
    print >> fp, "end);"

if __name__ == '__main__':
    if sys.argv[1:]:
        gd = open(sys.argv[1], "w")
        gi = open(sys.argv[2], "w")
    else:
        gd = sys.stdout
        gi = sys.stdout
    write_gd(gd)
    write_gi(gi)
