import gobject
import gtk

_transform_funcs = {}

def _register(func, typ, signal):
    funcs = _transform_funcs.get(signal)
    if funcs:
        funcs.append([typ, func])
    else:
        funcs = [[typ, func]]
        _transform_funcs[signal] = funcs
        norm_sig = signal.replace('_', '-')
        if norm_sig != signal:
            _transform_funcs[norm_sig] = funcs


def _transform_delete_event(args):
    return [args[0]]
_register(_transform_delete_event, gtk.Widget, 'delete_event')


def _get_transform_func(obj, signal):
    funcs = _transform_funcs.get(signal)
    if not funcs:
        return None
    for f in funcs:
        if isinstance(obj, f[0]):
            return f[1]
    return None

class Callback:
    def __init__(self, obj, signal, func):
        id = obj.connect(signal, self.callback, func)
        self.obj = obj
        self.id = id
        self.__transform_func = _get_transform_func(obj, signal)

    def callback(self, obj, *args):
        func = args[-1]
        args = self.__transform_args(args[:-1])
        return func(obj, self.id, args)

    def disconnect(self):
        self.obj.disconnect(self.id)
        self.obj = None

    def __transform_args(self, args):
        if self.__transform_func:
            return self.__transform_func(args)
        else:
            return args

if __name__ == '__main__':
    for f in _transform_funcs:
        print f, _transform_funcs[f]
