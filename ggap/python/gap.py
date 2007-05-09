import gtk
import moo

def glade_xml(filename, root=None):
    def signal_func(xml, widget_id, widget, signal, handler, object, siglist):
        siglist.append({'widget_id': widget_id, 'widget': widget,
                        'signal': signal, 'handler': handler, 'object': object})
        return True

    signals = []
    xml = moo.utils.GladeXML()
    xml.set_signal_func(signal_func, signals)
    xml.parse_file(filename, root)

    return {'xml': xml, 'signals': signals}
