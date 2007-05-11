import gtk
import moo

def GladeXML(filename, root=None):
    def signal_func(xml, widget_id, widget, signal, handler, object, siglist):
        siglist.append({'widget_id': widget_id, 'widget': widget,
                        'signal': signal, 'handler': handler, 'object': object})
        return True

    signals = []
    xml = moo.utils.GladeXML()
    xml.set_signal_func(signal_func, signals)
    xml.parse_file(filename, root)

    return {'xml': xml, 'signals': signals}


def list_store_remove(store, pos):
    print pos, len(store)
    store.remove(store.get_iter((pos,)))

store = gtk.ListStore(str)
store.append(['111'])
store.append(['111'])
store.append(['111'])
list_store_remove(store, len(store)-1)

def list_store_insert(store, pos, item):
    store.insert(pos, item)
def list_store_append(store, item):
    store.append(item)
def list_store_set(store, path, item):
    store[path] = item
def list_store_get(store, path):
    return list(store[path])

def list_store_set_list(store, items):
    store.clear()
    for i in items:
        store.append(i)
def list_store_get_list(store):
    return [list(row) for row in store]
