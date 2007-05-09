classes = {}

class FuncBase(object):
    def __init__(self, args=[], opt_args=[], py_name=None, gap_name=None, other=False, is_meth=True):
        object.__init__(self)
        self.is_meth = is_meth
        if py_name is not None:
            self.py_name = py_name
        if gap_name is not None:
            self.gap_name = gap_name
        self.other = other

        i = 1
        self.args = []
        self.opt_args = []
        for a in args:
            if isinstance(a, str):
                self.args.append([a, 'arg' + str(i)])
            else:
                self.args.append(list(a))
            i += 1
        for a in opt_args:
            if isinstance(a, str):
                self.opt_args.append([a, 'arg' + str(i)])
            else:
                self.opt_args.append(list(a))
            i += 1
        for a in self.args + self.opt_args:
            if a[1] in ['end']:
                a[1] = a[1] + '_';

    def set_obj_type(self, typ):
        if self.is_meth:
            self.obj_type = typ
            self.args = [(typ, self)] + self.args

    def get_gap_name(self):
        if not self.py_name:
            raise RuntimeError(self)
        if not hasattr(self, 'gap_name'):
            def cap(s):
                return s[0].title() + s[1:]
            self.gap_name = ''.join([cap(c) for c in self.py_name.replace('.', '_').split('_')])
        return self.gap_name

def format_func(func, args=None):
    if args is None:
        args = func.args
    n_args = len(args)
    if func.is_meth:
        assert n_args > 0
        tmpl = '' \
            'function(%(args)s)\n' \
            '    return _GGAP_CALL_METH(self, "%(py_name)s"%(rest_args)s);\n' \
            'end'
        dic = {'py_name' : func.py_name, 'rest_args' : ''}
        args = [a[1] for a in args]
        dic['args'] = ', '.join(args)
        if n_args > 1:
            dic['rest_args'] = ', ' + ', '.join(args[1:])
        return tmpl % dic
    else:
        tmpl = '' \
            'function(%(args)s)\n' \
            '    return _GGAP_CALL_FUNC("%(py_name)s"%(rest_args)s);\n' \
            'end'
        dic = {'py_name' : func.py_name, 'args' : '', 'rest_args' : ''}
        if n_args > 0:
            args = [a[1] for a in args]
            dic['args'] = ', '.join(args)
            dic['rest_args'] = ', ' + dic['args']
        return tmpl % dic

def format_func_opt_args(func):
    n_args = len(func.args)
    if func.is_meth:
        assert n_args > 1
        tmpl = '' \
            'function(%(args)s)\n' \
            '    return _GGAP_CALL_METH_OPTARG(self, "%(py_name)s"%(rest_args)s, optarg);\n' \
            'end'
        dic = {'py_name' : func.py_name, 'rest_args' : ''}

        if n_args == 1:
            dic['args'] = 'self, optarg'
            dic['rest_args'] = ''
        else:
            args = [a[1] for a in func.args] + ['optarg']
            dic['args'] = ', '.join(args)
            dic['rest_args'] = ', ' + ', '.join(args[1:-1])
        return tmpl % dic
    else:
        tmpl = '' \
            'function(%(args)s)\n' \
            '    return _GGAP_CALL_FUNC_OPTARG("%(py_name)s"%(rest_args)s, optarg);\n' \
            'end'
        dic = {'py_name' : func.py_name, 'args' : 'optarg', 'rest_args' : ''}
        if n_args > 0:
            args = [a[1] for a in func.args]
            dic['args'] = ', '.join(args) + ', optarg'
            dic['rest_args'] = ', ' + ', '.join(args)
        return tmpl % dic


class GlobalFunction(FuncBase):
    def __str__(self):
        return '<GlobalFunction %s for %s, %s>' % (self.py_name, self.args, self.opt_args)

    def declare(self, fp):
        print >> fp, 'DeclareGlobalFunction("%s");' % (self.get_gap_name(),)

    def install(self, fp):
        assert not self.opt_args
        n_args = len(self.args)
        print >> fp, \
            'InstallGlobalFunction(%s,\n%s);' % (self.get_gap_name(),
                                                 format_func(self))

class Operation(FuncBase):
    def __str__(self):
        return '<Operation %s for %s, %s>' % (self.py_name, self.args, self.opt_args)

    def get_args_set(self):
        set = []
        for i in range(len(self.opt_args) + 1):
            set.append(self.args + self.opt_args[:i])
        return set

    def declare(self, fp):
        if not self.other:
            args_set = self.get_args_set()
            if self.opt_args:
                args_set.append(self.args + [('IsRecord', 'optarg')])
            for args in args_set:
                args = '[' + ', '.join([a[0] for a in args]) + ']'
                print >> fp, 'DeclareOperation("%s", %s);' % (self.get_gap_name(), args)

    def install(self, fp):
        args_set = self.get_args_set()
        for args in args_set:
            self._install_one(args, fp)
        if self.opt_args:
            self._install_opt_args(fp)

    def _install_one(self, args, fp):
        n_args = len(args)
        arg_types = '[' + ', '.join([a[0] for a in args]) + ']'
        if self.other:
            install_func = 'InstallOtherMethod'
        else:
            install_func = 'InstallMethod'
        print >> fp, '%s(%s, %s,\n%s);' % (install_func, self.get_gap_name(), arg_types,
                                           format_func(self, args))

    def _install_opt_args(self, fp):
        n_args = len(self.args)
        args = self.args + [('IsRecord', 'optarg')]
        arg_types = '[' + ', '.join([a[0] for a in args]) + ']'
        if self.other:
            install_func = 'InstallOtherMethod'
        else:
            install_func = 'InstallMethod'
        print >> fp, '%s(%s, %s,\n%s);' % (install_func, self.get_gap_name(), arg_types,
                                           format_func_opt_args(self))


def _get_vars(dest, src):
    for key in ['py_name', 'gap_name', 'other', 'is_op', 'is_meth', 'args', 'opt_args']:
        if src.has_key(key):
            dest[key] = src[key]

def Function(*args, **kwargs):
    real_kwargs = {
        'py_name': None, 'gap_name': None, 'other': False, 'is_op': True,
        'is_meth': True, 'opt_args': [], 'args': []
    }
    _get_vars(real_kwargs, kwargs)

    if args:
        if isinstance(args[0], str) or isinstance(args[0], tuple):
            real_kwargs['args'] = list(args)
        elif isinstance(args[0], list):
            real_kwargs['args'] = list(args[0])
        elif isinstance(args[0], dict):
            _get_vars(real_kwargs, args[0])
        elif isinstance(args[0], FuncBase):
            return args[0]
        else:
            raise RuntimeError()

    if real_kwargs['is_op']:
        cls = Operation
    else:
        cls = GlobalFunction
    del real_kwargs['is_op']

    return cls(**real_kwargs)


def _make_class_py_name(name):
    if name == 'GObject':
        return 'gobject.GObject'
    elif name.startswith('Gtk'):
        return 'gtk.' + name[3:]
    else:
        raise RuntimeError()

def _is_class(name):
    return name == 'GObject' or name.startswith('Gtk') or name.startswith('Moo')

class ClassInfo(object):
    def __init__(self, cls, parent_name):
        self.name = cls.__name__

        self.py_name = getattr(cls, '__py_name__', None)
        if not self.py_name:
            self.py_name = _make_class_py_name(self.name)
        self.gap_name = getattr(cls, '__gap_name__', 'Is' + cls.__name__)
        self.abstract = getattr(cls, '__abstract__', not parent_name)
        self.doc = getattr(cls, '__doc__', None)
        self.parents = []
        self.children = []
        self.methods = []

        classes[self.name] = self

        if parent_name:
            self.parents.append(classes[parent_name])
        for n in getattr(cls, '__implements__', []):
            self.parents.append(classes[n])

        if not hasattr(cls, '__new__') and not self.abstract:
            setattr(cls, '__new__', Function(py_name=self.py_name))

        for k in dir(cls):
            a = getattr(cls, k)
            if k == '__new__':
                if not isinstance(a, FuncBase):
                    args = []
                    if len(a) > 1:
                        args = a[1]
                    setattr(cls, k, Function(args, py_name=a[0], is_meth=False))
                else:
                    a.is_meth = False

            if _is_class(k):
                self.children.append(ClassInfo(a, self.name))
            elif k == '__new__' or not k.startswith('__'):
                if isinstance(a, list) or isinstance(a, tuple):
                    setattr(cls, k, Function(a, py_name=k))
                elif isinstance(a, FuncBase) and not hasattr(a, 'py_name'):
                    setattr(a, 'py_name', k)
                a = getattr(cls, k)
                if isinstance(a, FuncBase):
                    if a.is_meth:
                        a.args = [(self.gap_name, 'self')] + a.args
                    self.methods.append([k, getattr(cls, k)])
