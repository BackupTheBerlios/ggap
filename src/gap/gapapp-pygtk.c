/* -*- Mode: C; c-basic-offset: 4 -*- */

#include <Python.h>



#line 3 "/home/muntyan/Projects/ggap/src/gap/gapapp-pygtk.override"
#include <Python.h>
#define NO_IMPORT_PYGOBJECT
#include "pygobject.h"
#include "gap/gapapp.h"
#include "gap/gapeditwindow.h"
#include "gap/gaptermwindow.h"
#line 15 "/home/muntyan/Projects/ggap/src/gap/gapapp-pygtk.c"


/* ---------- types from other modules ---------- */
static PyTypeObject *_PyGObject_Type;
#define PyGObject_Type (*_PyGObject_Type)
static PyTypeObject *_PyGtkWindow_Type;
#define PyGtkWindow_Type (*_PyGtkWindow_Type)
static PyTypeObject *_PyMooApp_Type;
#define PyMooApp_Type (*_PyMooApp_Type)
static PyTypeObject *_PyMooEditWindow_Type;
#define PyMooEditWindow_Type (*_PyMooEditWindow_Type)
static PyTypeObject *_PyMooTermWindow_Type;
#define PyMooTermWindow_Type (*_PyMooTermWindow_Type)


/* ---------- forward type declarations ---------- */
PyTypeObject PyGapApp_Type;
PyTypeObject PyGapEditWindow_Type;
PyTypeObject PyGapOutChannel_Type;
PyTypeObject PyGapTermWindow_Type;


/* ----------- GapApp ----------- */

static int
pygobject_no_constructor(PyObject *self, PyObject *args, PyObject *kwargs)
{
    gchar buf[512];

    g_snprintf(buf, sizeof(buf), "%s is an abstract widget", self->ob_type->tp_name);
    PyErr_SetString(PyExc_NotImplementedError, buf);
    return -1;
}

static PyObject *
_wrap_gap_app_feed_gap(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "text", "len", NULL };
    char *text;
    int len;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "si:GapApp.feed_gap", kwlist, &text, &len))
        return NULL;
    gap_app_feed_gap(GAP_APP(self->obj), text, len);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gap_app_start_gap(PyGObject *self)
{
    gap_app_start_gap(GAP_APP(self->obj));
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gap_app_stop_gap(PyGObject *self)
{
    gap_app_stop_gap(GAP_APP(self->obj));
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gap_app_restart_gap(PyGObject *self)
{
    gap_app_restart_gap(GAP_APP(self->obj));
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gap_app_get_out_channel(PyGObject *self)
{
    GapOutChannel *ret;

    ret = gap_app_get_out_channel(GAP_APP(self->obj));
    /* pygobject_new handles NULL checking */
    return pygobject_new((GObject *)ret);
}

static PyObject *
_wrap_gap_app_write_out(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "data", "len", NULL };
    char *data;
    int len = -1, ret;
    PyObject *py_ret;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|i:GapApp.write_out", kwlist, &data, &len))
        return NULL;
    ret = gap_app_write_out(GAP_APP(self->obj), data, len);
    py_ret = ret ? Py_True : Py_False;
    Py_INCREF(py_ret);
    return py_ret;
}

static PyMethodDef _PyGapApp_methods[] = {
    { "feed_gap", (PyCFunction)_wrap_gap_app_feed_gap, METH_VARARGS|METH_KEYWORDS },
    { "start_gap", (PyCFunction)_wrap_gap_app_start_gap, METH_NOARGS },
    { "stop_gap", (PyCFunction)_wrap_gap_app_stop_gap, METH_NOARGS },
    { "restart_gap", (PyCFunction)_wrap_gap_app_restart_gap, METH_NOARGS },
    { "get_out_channel", (PyCFunction)_wrap_gap_app_get_out_channel, METH_NOARGS },
    { "write_out", (PyCFunction)_wrap_gap_app_write_out, METH_VARARGS|METH_KEYWORDS },
    { NULL, NULL, 0 }
};

PyTypeObject PyGapApp_Type = {
    PyObject_HEAD_INIT(NULL)
    0,					/* ob_size */
    "ggap.gap.App",			/* tp_name */
    sizeof(PyGObject),	        /* tp_basicsize */
    0,					/* tp_itemsize */
    /* methods */
    (destructor)0,			/* tp_dealloc */
    (printfunc)0,			/* tp_print */
    (getattrfunc)0,	/* tp_getattr */
    (setattrfunc)0,	/* tp_setattr */
    (cmpfunc)0,		/* tp_compare */
    (reprfunc)0,		/* tp_repr */
    0,			/* tp_as_number */
    0,		/* tp_as_sequence */
    0,			/* tp_as_mapping */
    (hashfunc)0,		/* tp_hash */
    (ternaryfunc)0,		/* tp_call */
    (reprfunc)0,		/* tp_str */
    (getattrofunc)0,			/* tp_getattro */
    (setattrofunc)0,			/* tp_setattro */
    0,					/* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
    NULL, 				/* Documentation string */
    (traverseproc)0,			/* tp_traverse */
    (inquiry)0,			/* tp_clear */
    (richcmpfunc)0,	/* tp_richcompare */
    offsetof(PyGObject, weakreflist),             /* tp_weaklistoffset */
    (getiterfunc)0,		/* tp_iter */
    (iternextfunc)0,	/* tp_iternext */
    _PyGapApp_methods,			/* tp_methods */
    0,					/* tp_members */
    0,		       	/* tp_getset */
    NULL,				/* tp_base */
    NULL,				/* tp_dict */
    (descrgetfunc)0,	/* tp_descr_get */
    (descrsetfunc)0,	/* tp_descr_set */
    offsetof(PyGObject, inst_dict),                 /* tp_dictoffset */
    (initproc)pygobject_no_constructor,		/* tp_init */
};



/* ----------- GapEditWindow ----------- */

static int
_wrap_gap_edit_window_new(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { NULL };

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, ":GapEditWindow.__init__", kwlist))
        return -1;
    self->obj = (GObject *)gap_edit_window_new();

    if (!self->obj) {
        PyErr_SetString(PyExc_RuntimeError, "could not create GapEditWindow object");
        return -1;
    }
    pygobject_register_wrapper((PyObject *)self);
    return 0;
}

PyTypeObject PyGapEditWindow_Type = {
    PyObject_HEAD_INIT(NULL)
    0,					/* ob_size */
    "ggap.gap.EditWindow",			/* tp_name */
    sizeof(PyGObject),	        /* tp_basicsize */
    0,					/* tp_itemsize */
    /* methods */
    (destructor)0,			/* tp_dealloc */
    (printfunc)0,			/* tp_print */
    (getattrfunc)0,	/* tp_getattr */
    (setattrfunc)0,	/* tp_setattr */
    (cmpfunc)0,		/* tp_compare */
    (reprfunc)0,		/* tp_repr */
    0,			/* tp_as_number */
    0,		/* tp_as_sequence */
    0,			/* tp_as_mapping */
    (hashfunc)0,		/* tp_hash */
    (ternaryfunc)0,		/* tp_call */
    (reprfunc)0,		/* tp_str */
    (getattrofunc)0,			/* tp_getattro */
    (setattrofunc)0,			/* tp_setattro */
    0,					/* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
    NULL, 				/* Documentation string */
    (traverseproc)0,			/* tp_traverse */
    (inquiry)0,			/* tp_clear */
    (richcmpfunc)0,	/* tp_richcompare */
    offsetof(PyGObject, weakreflist),             /* tp_weaklistoffset */
    (getiterfunc)0,		/* tp_iter */
    (iternextfunc)0,	/* tp_iternext */
    NULL,			/* tp_methods */
    0,					/* tp_members */
    0,		       	/* tp_getset */
    NULL,				/* tp_base */
    NULL,				/* tp_dict */
    (descrgetfunc)0,	/* tp_descr_get */
    (descrsetfunc)0,	/* tp_descr_set */
    offsetof(PyGObject, inst_dict),                 /* tp_dictoffset */
    (initproc)_wrap_gap_edit_window_new,		/* tp_init */
};



/* ----------- GapOutChannel ----------- */

static int
_wrap_gap_out_channel_new(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "fifo_basename", NULL };
    char *fifo_basename;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s:GapOutChannel.__init__", kwlist, &fifo_basename))
        return -1;
    self->obj = (GObject *)gap_out_channel_new(fifo_basename);

    if (!self->obj) {
        PyErr_SetString(PyExc_RuntimeError, "could not create GapOutChannel object");
        return -1;
    }
    pygobject_register_wrapper((PyObject *)self);
    return 0;
}

static PyObject *
_wrap_gap_out_channel_start(PyGObject *self)
{
    int ret;
    PyObject *py_ret;

    ret = gap_out_channel_start(GAP_OUT_CHANNEL(self->obj));
    py_ret = ret ? Py_True : Py_False;
    Py_INCREF(py_ret);
    return py_ret;
}

static PyObject *
_wrap_gap_out_channel_shutdown(PyGObject *self)
{
    gap_out_channel_shutdown(GAP_OUT_CHANNEL(self->obj));
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gap_out_channel_restart(PyGObject *self)
{
    int ret;
    PyObject *py_ret;

    ret = gap_out_channel_restart(GAP_OUT_CHANNEL(self->obj));
    py_ret = ret ? Py_True : Py_False;
    Py_INCREF(py_ret);
    return py_ret;
}

static PyObject *
_wrap_gap_out_channel_ready(PyGObject *self)
{
    int ret;
    PyObject *py_ret;

    ret = gap_out_channel_ready(GAP_OUT_CHANNEL(self->obj));
    py_ret = ret ? Py_True : Py_False;
    Py_INCREF(py_ret);
    return py_ret;
}

static PyObject *
_wrap_gap_out_channel_get_name(PyGObject *self)
{
    const gchar *ret;

    ret = gap_out_channel_get_name(GAP_OUT_CHANNEL(self->obj));
    if (ret)
        return PyString_FromString(ret);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_gap_out_channel_write(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "data", "len", NULL };
    char *data;
    int len = -1, ret;
    PyObject *py_ret;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|i:GapOutChannel.write", kwlist, &data, &len))
        return NULL;
    ret = gap_out_channel_write(GAP_OUT_CHANNEL(self->obj), data, len);
    py_ret = ret ? Py_True : Py_False;
    Py_INCREF(py_ret);
    return py_ret;
}

static PyMethodDef _PyGapOutChannel_methods[] = {
    { "start", (PyCFunction)_wrap_gap_out_channel_start, METH_NOARGS },
    { "shutdown", (PyCFunction)_wrap_gap_out_channel_shutdown, METH_NOARGS },
    { "restart", (PyCFunction)_wrap_gap_out_channel_restart, METH_NOARGS },
    { "ready", (PyCFunction)_wrap_gap_out_channel_ready, METH_NOARGS },
    { "get_name", (PyCFunction)_wrap_gap_out_channel_get_name, METH_NOARGS },
    { "write", (PyCFunction)_wrap_gap_out_channel_write, METH_VARARGS|METH_KEYWORDS },
    { NULL, NULL, 0 }
};

PyTypeObject PyGapOutChannel_Type = {
    PyObject_HEAD_INIT(NULL)
    0,					/* ob_size */
    "ggap.gap.OutChannel",			/* tp_name */
    sizeof(PyGObject),	        /* tp_basicsize */
    0,					/* tp_itemsize */
    /* methods */
    (destructor)0,			/* tp_dealloc */
    (printfunc)0,			/* tp_print */
    (getattrfunc)0,	/* tp_getattr */
    (setattrfunc)0,	/* tp_setattr */
    (cmpfunc)0,		/* tp_compare */
    (reprfunc)0,		/* tp_repr */
    0,			/* tp_as_number */
    0,		/* tp_as_sequence */
    0,			/* tp_as_mapping */
    (hashfunc)0,		/* tp_hash */
    (ternaryfunc)0,		/* tp_call */
    (reprfunc)0,		/* tp_str */
    (getattrofunc)0,			/* tp_getattro */
    (setattrofunc)0,			/* tp_setattro */
    0,					/* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
    NULL, 				/* Documentation string */
    (traverseproc)0,			/* tp_traverse */
    (inquiry)0,			/* tp_clear */
    (richcmpfunc)0,	/* tp_richcompare */
    offsetof(PyGObject, weakreflist),             /* tp_weaklistoffset */
    (getiterfunc)0,		/* tp_iter */
    (iternextfunc)0,	/* tp_iternext */
    _PyGapOutChannel_methods,			/* tp_methods */
    0,					/* tp_members */
    0,		       	/* tp_getset */
    NULL,				/* tp_base */
    NULL,				/* tp_dict */
    (descrgetfunc)0,	/* tp_descr_get */
    (descrsetfunc)0,	/* tp_descr_set */
    offsetof(PyGObject, inst_dict),                 /* tp_dictoffset */
    (initproc)_wrap_gap_out_channel_new,		/* tp_init */
};



/* ----------- GapTermWindow ----------- */

static PyObject *
_wrap_gap_term_window_apply_settings(PyGObject *self)
{
    gap_term_window_apply_settings(GAP_TERM_WINDOW(self->obj));
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef _PyGapTermWindow_methods[] = {
    { "apply_settings", (PyCFunction)_wrap_gap_term_window_apply_settings, METH_NOARGS },
    { NULL, NULL, 0 }
};

PyTypeObject PyGapTermWindow_Type = {
    PyObject_HEAD_INIT(NULL)
    0,					/* ob_size */
    "ggap.gap.TermWindow",			/* tp_name */
    sizeof(PyGObject),	        /* tp_basicsize */
    0,					/* tp_itemsize */
    /* methods */
    (destructor)0,			/* tp_dealloc */
    (printfunc)0,			/* tp_print */
    (getattrfunc)0,	/* tp_getattr */
    (setattrfunc)0,	/* tp_setattr */
    (cmpfunc)0,		/* tp_compare */
    (reprfunc)0,		/* tp_repr */
    0,			/* tp_as_number */
    0,		/* tp_as_sequence */
    0,			/* tp_as_mapping */
    (hashfunc)0,		/* tp_hash */
    (ternaryfunc)0,		/* tp_call */
    (reprfunc)0,		/* tp_str */
    (getattrofunc)0,			/* tp_getattro */
    (setattrofunc)0,			/* tp_setattro */
    0,					/* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
    NULL, 				/* Documentation string */
    (traverseproc)0,			/* tp_traverse */
    (inquiry)0,			/* tp_clear */
    (richcmpfunc)0,	/* tp_richcompare */
    offsetof(PyGObject, weakreflist),             /* tp_weaklistoffset */
    (getiterfunc)0,		/* tp_iter */
    (iternextfunc)0,	/* tp_iternext */
    _PyGapTermWindow_methods,			/* tp_methods */
    0,					/* tp_members */
    0,		       	/* tp_getset */
    NULL,				/* tp_base */
    NULL,				/* tp_dict */
    (descrgetfunc)0,	/* tp_descr_get */
    (descrsetfunc)0,	/* tp_descr_set */
    offsetof(PyGObject, inst_dict),                 /* tp_dictoffset */
    (initproc)pygobject_no_constructor,		/* tp_init */
};



/* ----------- functions ----------- */

PyMethodDef gap_app_functions[] = {
    { NULL, NULL, 0 }
};

/* intialise stuff extension classes */
void
gap_app_register_classes(PyObject *d)
{
    PyObject *module;

    if ((module = PyImport_ImportModule("gobject")) != NULL) {
        PyObject *moddict = PyModule_GetDict(module);

        _PyGObject_Type = (PyTypeObject *)PyDict_GetItemString(moddict, "GObject");
    } else {
        Py_FatalError("could not import gobject");
        return;
    }
    if ((module = PyImport_ImportModule("gtk")) != NULL) {
        PyObject *moddict = PyModule_GetDict(module);

        _PyGtkWindow_Type = (PyTypeObject *)PyDict_GetItemString(moddict, "Window");
    } else {
        Py_FatalError("could not import gtk");
        return;
    }
    if ((module = PyImport_ImportModule("moo")) != NULL) {
        PyObject *moddict = PyModule_GetDict(module);

        _PyMooEditWindow_Type = (PyTypeObject *)PyDict_GetItemString(moddict, "EditWindow");
        _PyMooTermWindow_Type = (PyTypeObject *)PyDict_GetItemString(moddict, "TermWindow");
    } else {
        Py_FatalError("could not import moo");
        return;
    }
    if ((module = PyImport_ImportModule("ggap")) != NULL) {
        PyObject *moddict = PyModule_GetDict(module);

        _PyMooApp_Type = (PyTypeObject *)PyDict_GetItemString(moddict, "App");
    } else {
        Py_FatalError("could not import ggap");
        return;
    }


#line 479 "/home/muntyan/Projects/ggap/src/gap/gapapp-pygtk.c"
    pygobject_register_class(d, "GapApp", GAP_TYPE_APP, &PyGapApp_Type, Py_BuildValue("(O)", &PyMooApp_Type));
    pygobject_register_class(d, "GapEditWindow", GAP_TYPE_EDIT_WINDOW, &PyGapEditWindow_Type, Py_BuildValue("(O)", &PyMooEditWindow_Type));
    pygobject_register_class(d, "GapOutChannel", GAP_TYPE_OUT_CHANNEL, &PyGapOutChannel_Type, Py_BuildValue("(O)", &PyGObject_Type));
    pygobject_register_class(d, "GapTermWindow", GAP_TYPE_TERM_WINDOW, &PyGapTermWindow_Type, Py_BuildValue("(O)", &PyMooTermWindow_Type));
}
