#############################################################################
##
#W  ggap.py                     ggap package                   Yevgen Muntyan
#W
#Y  Copyright (C) 2004-2007 by Yevgen Muntyan <muntyan@math.tamu.edu>
##
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation. See COPYING file that comes with this
##  distribution.
##

import moo
import gtk
import gap
import gap_cb
import gap_types
import gobject
import pango
import sys
import os
import traceback
import string as stringmod

__session__ = None

CMD_NOTHING         = 0
CMD_RETURN          = 1
CMD_ERROR           = 2
CMD_OBJ_DESTROYED   = 3
CMD_GC              = 4
CMD_CALLBACK        = 5
CMD_CALLFUNC        = 6

DATA_NONE           = 0
DATA_BOOL           = 1
DATA_SMALL_INT      = 2
DATA_STRING         = 3
DATA_LIST           = 4
DATA_OBJECT         = 5
DATA_DICT           = 6

RET_OK              = 0
RET_ERR             = 1

class GAPError(RuntimeError):
    pass

class Wrapper:
    def __init__(self, obj, id, destroy_cb, type_name=None):
        self.obj = obj
        self.id = id
        self.type_name = type_name or type(obj).__gtype__.name
        self.callbacks = {}
        self.gap_ref = 1
        self.destroy_handler = 0

        if isinstance(obj, gtk.Object):
            self.destroy_handler = obj.connect('destroy', self.destroyed, destroy_cb)

    def destroyed(self, obj, func):
        self.callbacks = {}
        func(obj)
        self.destroy_handler = 0

    def disconnect(self):
        for id in self.callbacks:
            self.callbacks[id].disconnect()
        if self.destroy_handler:
            self.obj.disconnect(self.destroy_handler)
        self.destroy_handler = 0
        self.callbacks = {}
        self.obj = None

class FuncStack:
    class Entry:
        def __init__(self, stamp):
            self.stamp = stamp

    def __init__(self):
        self.entries = []
        self.retvals = {}

    def push(self, stamp):
        assert stamp not in [e.stamp for e in self.entries]
        assert stamp not in self.retvals
        self.entries.append(FuncStack.Entry(stamp))

    def pop(self, stamp):
        if stamp not in self.retvals:
            raise RuntimeError("did not receive return value for stamp %s yet" % (stamp,))
        return self.retvals.pop(stamp)

    def do_return(self, stamp, status, value):
        if not self.entries or stamp != self.entries[-1].stamp:
            print >> sys.stderr, "got unexpected return value for stamp %s" % (stamp,)
            return
        e = self.entries.pop()
        self.retvals[stamp] = [status, value]
        if gtk.main_level() <= 1:
            print >> sys.stderr, "got return value outside main loop!" % (stamp,)
            return
        gtk.main_quit()

    def shutdown(self):
        while self.entries:
            stamp = self.entries[-1].stamp
            self.do_return(stamp, RET_OK, None)

class Session:
    def __init__(self, session_id=0, log=None):
        global __session__
        __session__ = self

        self.by_obj = {}
        self.by_id = {}
        self.last_id = 0
        self.log = log
        self.stack = FuncStack()
        self.last_stamp = 0
        self.session_id = session_id

        self.debug = False
        self.log_input = []
        self.log_output = []

        self.locals = {
            "__name__" : "__ggap__",
            "__doc__" : None,
            "__session__" : self,
            "OBJECT" : self.get_object,
            "GET_TYPE_INFO" : self.get_type_info,
            "UNREF_OBJECTS" : self.unref_objects,
            "gtk": gtk, "sys": sys, "gap": gap, "pango": pango, "gobject": gobject, "os": os,
        }

        try:
            import moo
            self.locals['moo'] = moo
        except ImportError:
            pass

    def write_output(self, string):
        if self.debug:
            self.log_output.append(string)
        _app_output_write(string)

    def print_error(self, error=None):
        s = self.format_error(error)
        print >> sys.stderr, s
        self.__log(s)

    def format_error(self, error=None):
        if error:
            return str(error) + "\n" + \
                "".join(traceback.format_exception(*sys.exc_info()))
        else:
            return "".join(traceback.format_exception(*sys.exc_info()))

    def new_stamp(self):
        self.last_stamp += 1
        stamp = (self.last_stamp << 8) + self.session_id
        if stamp >= 10**8:
            self.last_stamp = 1
            stamp = (self.last_stamp << 8) + self.session_id
        return stamp
    def get_stamp(self, string):
        val = int(string[:8])
        if val & 255 != self.session_id:
            self.__log('got data for session %d, ignoring' % (val & 255,))
            return 0, None
        else:
            return val, string[8:]

    def __log(self, what):
        if self.log is not None and self.debug:
            print >> self.log, what

    def shutdown(self):
        global __session__
        __session__ = None

        self.stack.shutdown()
        del self.stack
        ids = self.by_id.keys()
        for i in ids:
            w = self.by_id[i]
            w.disconnect()
        self.by_obj = {}
        self.by_id = {}

    def __wrap(self, obj):
        try:
            w = self.by_obj[obj]
            w.gap_ref = 1
            return w
        except KeyError:
            self.last_id += 1
            w = Wrapper(obj, self.last_id, self.__gtk_object_destroyed)
            self.by_obj[obj] = w
            self.by_id[self.last_id] = w
            return w

    def __gtk_object_destroyed(self, obj):
        obj_id = self.by_obj[obj].id
        self.__log("object %s destroyed" % (obj_id,))
        string = '%c%s' % (CMD_OBJ_DESTROYED, self.serialize(obj_id))
        self.write_output(string)

    def __get_wrapper(self, id):
        try:
            return self.by_id[id]
        except KeyError:
            return None

    def __unref(self, id):
        w = self.__get_wrapper(id)
        if w is None:
            self.__log("Asked to unref nonexisting object %s" % (id,))
        else:
            w.gap_ref = 0
            if not w.callbacks:
                self.__log("Removing object %s" % (id,))
                del self.by_obj[w.obj]
                del self.by_id[id]
                w.disconnect()

    def get_object(self, obj_id):
        w = self.__get_wrapper(obj_id)
        if w is None:
            raise RuntimeError("No object with id %s" % (obj_id,))
        return w.obj
    def get_type_info(self, typename):
        typ = gobject.type_from_name(typename)
        parent = gobject.type_parent(typ)
        return {'parent': parent.name}

    def unref_objects(self, session_id, ids):
        if session_id != self.session_id:
            self.__log("asked to unref objects for session %s, ignoring" % (session_id,))
            return
        self.__log("removing objects %s" % (ids,))
        for obj_id in ids:
            try:
                self.__unref(obj_id)
            except Exception, e:
                print >> sys.stderr, e

    def execute(self, string):
        if self.debug:
            self.log_input.append(string)
        try:
            cmd = string[0]
            string = string[1:]

            if cmd == 'f':
                stamp, string = self.get_stamp(string)
                if not stamp:
                    return

                try:
                    func_and_args = eval(string, self.locals)
                    func = func_and_args[0]
                    args = func_and_args[1]
                    kwargs = {}
                    if len(func_and_args) > 2:
                        kwargs = func_and_args[2]
                except Exception, e:
                    msg = self.format_error(e)
                    self.print_error(e)
                    self.send_error(stamp, msg)
                else:
                    self.call_func(stamp, func, *args, **kwargs)

            elif cmd == 'r':
                stamp, string = self.get_stamp(string)
                if not stamp:
                    return

                try:
                    value = eval(string, self.locals)
                except Exception, e:
                    print >> sys.stderr, e
                    value = None
                self.stack.do_return(stamp, RET_OK, value)

            elif cmd == 'e':
                stamp, string = self.get_stamp(string)
                if not stamp:
                    return

                try:
                    msg = eval(string, self.locals)
                except Exception, e:
                    print >> sys.stderr, e
                    msg = ""
                self.stack.do_return(stamp, RET_ERR, msg)

            elif cmd == 'c':
                exec string in self.locals

            else:
                raise RuntimeError("unknown command type %s in: %s" % (cmd, string))
        except Exception, e:
            print >> sys.stderr, e
            self.__log(e)

    def call_func(self, stamp, func, *args, **kwargs):
        try:
            self.__log("%03d func: %s, %s, %s" % (stamp >> 8, func, args, kwargs))
            ret = func(*args, **kwargs)
            self.__log("%03d func done: %s" % (stamp >> 8, ret))
            self.send_result(stamp, ret)
        except Exception, e:
            self.__log("%03d func error" % (stamp >> 8,))
            msg = self.format_error(e)
            self.print_error(e)
            self.send_error(stamp, "Error in '%s': %s" % (func, msg))

    def send_result(self, stamp, val):
        self.__log("%03d retval: %s" % (stamp >> 8, val))
        if val is None:
            self.send_return(stamp, None)
        elif isinstance(val, gobject.GObject):
            w = self.__wrap(val)
            self.send_return(stamp, w)
        else:
            self.send_return(stamp, val)

    def send_error(self, stamp, msg):
        self.__log("%03d error: %s" % (stamp >> 8, msg))
        string = '%c%s%s' % (CMD_ERROR, self.serialize(stamp), self.serialize(str(msg)))
        self.write_output(string)

    def send_return(self, stamp, value):
        string = '%c%s%s' % (CMD_RETURN, self.serialize(stamp), self.serialize(value))
        self.write_output(string)

    def send_gc(self, call_gasman=False):
        string = '%c%s' % (CMD_GC, self.serialize(call_gasman))
        self.write_output(string)

    def serialize(self, val):
        val = gap_types.transform(val)

        if val is None:
            return '%c' % (DATA_NONE,)

        elif val is True:
            return '%c\1' % (DATA_BOOL,)

        elif val is False:
            return '%c\0' % (DATA_BOOL,)

        elif isinstance(val, int) or isinstance(val, long):
            if val <= -2**16 or val >= 2**16:
                raise RuntimeError("integer too big")
            string = '%c%c' % (DATA_SMALL_INT, val < 0)
            if val < 0:
                val = -val
            string += '%c%c' % ((val & 0xff00) >> 8, val & 0xff)
            return string

        elif isinstance(val, str):
            length = len(val)
            if length > 2**16:
                raise RuntimeError("string too long")
            return '%c%c%c%s' % (DATA_STRING, (length & 0xff00) >> 8, length & 0xff, val)

        elif isinstance(val, list) or isinstance(val, tuple):
            length = len(val)
            if length > 2**16:
                raise RuntimeError("list too long")
            string = '%c%c%c' % (DATA_LIST, (length & 0xff00) >> 8, length & 0xff)
            for elm in val:
                string += self.serialize(elm)
            return string

        elif isinstance(val, dict):
            def checkletters(s):
                for l in s:
                    if not l in stringmod.ascii_letters + stringmod.digits + '_':
                        return False
                return True
            for k in val:
                if not isinstance(k, str) or not len(k) or \
                   not k[0] in stringmod.ascii_letters + '_' or \
                   not checkletters(k[1:]):
                    raise RuntimeError("could not serialize dict '%s': bad key '%s'" % \
                                        (val, k))
            length = len(val)
            if length > 2**15:
                raise RuntimeError("list too long")
            string = '%c%c%c' % (DATA_DICT, (length & 0xff00) >> 8, length & 0xff)
            for k in val:
                string += self.serialize(k)
                string += self.serialize(val[k])
            return string

        elif isinstance(val, Wrapper):
            return '%c%s%s' % (DATA_OBJECT,
                               self.serialize(val.id),
                               self.serialize(val.type_name))

        elif isinstance(val, gobject.GObject):
            return self.serialize(self.__wrap(val))

        else:
            raise RuntimeError("could not serialize value %s of type %s" % \
                                (val, type(val)))

    def get_return_value(self, stamp):
        self.stack.push(stamp)
        self.__log("waiting for return for stamp %s" % (stamp >> 8,))
        gtk.main()
        self.__log("got return for stamp %s" % (stamp >> 8,))
        return self.stack.pop(stamp)

    def _callback(self, obj, handler_id, args):
        w = self.by_obj[obj]
        stamp = self.new_stamp()
        self.__log("%03d signal: %s, %s" % (stamp >> 8, obj, args))
        string = '%c%s%s%s%s' % (CMD_CALLBACK, self.serialize(stamp),
                                 self.serialize(w.id), self.serialize(handler_id),
                                 self.serialize(args))
        self.write_output(string)
        status, value = self.get_return_value(stamp)
        self.__log("%03d signal done: %s" % (stamp >> 8, (status, value),))
        if status == RET_OK:
            return value
        else:
            raise GAPError(value)

    def connect(self, obj_id, signal):
        w = self.by_id[obj_id]
        if w is None:
            raise RuntimeError("no object with id %s" % (obj_id,))

        q = gobject.signal_query(signal, type(w.obj))
        if q is None:
            raise RuntimeError("no signal %s in object %s" % (signal, obj_id,))

        cb = gap_cb.Callback(w.obj, signal, self._callback)
        w.callbacks[cb.id] = cb
        return [cb.id, q[4] == gobject.TYPE_NONE]

    def disconnect(self, obj_id, callback_id):
        try:
            w = self.by_id[obj_id]
            if w is None:
                return False
            cb = w.callbacks[callback_id]
            del w.callbacks[callback_id]
            cb.disconnect()
            if not w.callbacks and not w.gap_ref:
                self.__unref(obj_id)
            return True
        except Exception, e:
            print >> sys.stderr, e
            raise e

    def call_gap(self, func, args=[], void=True):
        stamp = self.new_stamp()
        string = '%c%s%s%s%s' % (CMD_CALLFUNC, self.serialize(stamp),
                                 self.serialize(func), self.serialize(args),
                                 self.serialize(void))
        self.write_output(string)
        status, value = self.get_return_value(stamp)
        if status == RET_OK:
            return value
        else:
            raise GAPError(value)

if __name__ == '__main__':
    def _app_output_write(string):
        pass
    locals()['_app_output_write'] = _app_output_write
    s = Session(log=sys.stderr)
    s.execute('f%08d[gtk.Window,[]]' % (1,))
    s.execute('f%08d[OBJECT(1).show,[]]' % (2,))
#     s.execute('f%08d[__session__.connect, [1, "destroy"]]' % (4,))
#     s.execute('f%08d[OBJECT(1).destroy,[]]' % (3,))
#     s.execute('r%08dNone' % (1))
#     s.execute('call_meth(2, get_object(1), "show")')
#     s.execute('call_meth(3, get_object(1), "destroy")')
    s.shutdown()
    gtk.main()
