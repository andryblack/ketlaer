// This is the implementation of pyqtProperty.
//
// Copyright (c) 2010 Riverbank Computing Limited <info@riverbankcomputing.com>
// 
// This file is part of PyQt.
// 
// This file may be used under the terms of the GNU General Public
// License versions 2.0 or 3.0 as published by the Free Software
// Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
// included in the packaging of this file.  Alternatively you may (at
// your option) use any later version of the GNU General Public
// License if such license has been publicly approved by Riverbank
// Computing Limited (or its successors, if any) and the KDE Free Qt
// Foundation. In addition, as a special exception, Riverbank gives you
// certain additional rights. These rights are described in the Riverbank
// GPL Exception version 1.1, which can be found in the file
// GPL_EXCEPTION.txt in this package.
// 
// Please review the following information to ensure GNU General
// Public Licensing requirements will be met:
// http://trolltech.com/products/qt/licenses/licensing/opensource/. If
// you are unsure which license is appropriate for your use, please
// review the following information:
// http://trolltech.com/products/qt/licenses/licensing/licensingoverview
// or contact the sales department at sales@riverbankcomputing.com.
// 
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.


#include <Python.h>
#include <structmember.h>

#include "qpycore_chimera.h"
#include "qpycore_pyqtproperty.h"


// Forward declarations.
extern "C" {
static void pyqtProperty_dealloc(PyObject *self);
static int pyqtProperty_init(PyObject *self, PyObject *args, PyObject *kwds);
static int pyqtProperty_traverse(PyObject *self, visitproc visit, void *arg);
}


// Doc-strings.
PyDoc_STRVAR(pyqtProperty_doc,
"pyqtProperty(type, fget=None, fset=None, freset=None, fdel=None, doc=None,\n"
"        designable=True, scriptable=True, stored=True, user=False,\n"
"        constant=False, final=False) -> property attribute\n"
"\n"
"type is the type of the property.  It is either a type object or a string\n"
"that is the name of a C++ type.\n"
"freset is a function for resetting an attribute to its default value.\n"
"designable sets the DESIGNABLE flag (the default is True for writable\n"
"properties and False otherwise).\n"
"scriptable sets the SCRIPTABLE flag.\n"
"stored sets the STORED flag.\n"
"user sets the USER flag.\n"
"constant sets the CONSTANT flag.\n"
"final sets the FINAL flag.\n"
"The other parameters are the same as those required by the standard Python\n"
"property type.  Properties defined using pyqtProperty behave as both Python\n"
"and Qt properties.");


// Define the attributes.
static PyMemberDef pyqtProperty_members[] = {
    {const_cast<char *>("freset"), T_OBJECT,
            offsetof(qpycore_pyqtProperty, pyqtprop_reset), READONLY, 0},
    {const_cast<char *>("type"), T_OBJECT,
            offsetof(qpycore_pyqtProperty, pyqtprop_type), READONLY, 0},
    {0, 0, 0, 0, 0}
};


// This implements the PyQt version of the standard Python property type.
PyTypeObject qpycore_pyqtProperty_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    SIP_TPNAME_CAST("PyQt4.QtCore.pyqtProperty"),
    sizeof (qpycore_pyqtProperty),
    0,
    pyqtProperty_dealloc,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    Py_TPFLAGS_DEFAULT|Py_TPFLAGS_HAVE_GC|Py_TPFLAGS_BASETYPE,
    pyqtProperty_doc,
    pyqtProperty_traverse,
    0,
    0,
    0,
    0,
    0,
    0,
    pyqtProperty_members,
    0,
    0,
    0,
    0,
    0,
    0,
    pyqtProperty_init,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
#if PY_VERSION_HEX >= 0x02060000
    0,
#endif
};


// This is the sequence number to allocate to the next PyQt property to be
// defined.
static uint pyqtprop_sequence_nr = 0;


// The pyqtProperty dealloc function.
static void pyqtProperty_dealloc(PyObject *self)
{
    qpycore_pyqtProperty *pp = (qpycore_pyqtProperty *)self;

    Py_XDECREF(pp->pyqtprop_reset);
    Py_XDECREF(pp->pyqtprop_type);

    delete pp->pyqtprop_parsed_type;

    PyProperty_Type.tp_dealloc(self);
}


// The pyqtProperty traverse function.
static int pyqtProperty_traverse(PyObject *self, visitproc visit, void *arg)
{
    qpycore_pyqtProperty *pp = (qpycore_pyqtProperty *)self;
    int vret;

    if (pp->pyqtprop_reset)
    {
        vret = visit(pp->pyqtprop_reset, arg);

        if (vret != 0)
            return vret;
    }

    if (pp->pyqtprop_type)
    {
        vret = visit(pp->pyqtprop_type, arg);

        if (vret != 0)
            return vret;
    }

    return PyProperty_Type.tp_traverse(self, visit, arg);
}


// The pyqtProperty init function.
static int pyqtProperty_init(PyObject *self, PyObject *args, PyObject *kwds)
{
    PyObject *type, *get = 0, *set = 0, *reset = 0, *del = 0, *doc = 0;
    int designable = -1, scriptable = 1, stored = 1, user = 0, constant = 0,
            final = 0;
    static const char *kwlist[] = {"type", "fget", "fset", "freset", "fdel",
            "doc", "designable", "scriptable", "stored", "user", "constant",
            "final", 0};
    qpycore_pyqtProperty *pp = (qpycore_pyqtProperty *)self;

    pp->pyqtprop_sequence = pyqtprop_sequence_nr++;

    if (!PyArg_ParseTupleAndKeywords(args, kwds,
#if PY_VERSION_HEX >= 0x02050000
            "O|OOOOOiiiiii:pyqtProperty",
#else
            const_cast<char *>("O|OOOOOiiiiii:pyqtProperty"),
#endif
            const_cast<char **>(kwlist), &type, &get, &set, &reset, &del, &doc,
            &designable, &scriptable, &stored, &user, &constant, &final))
        return -1;

    if (get == Py_None)
        get = 0;

    if (set == Py_None)
        set = 0;

    if (del == Py_None)
        del = 0;

    if (reset == Py_None)
        reset = 0;

    // The default value of designable depends on whether the property is
    // writable.
    if (designable < 0)
        designable = (set != 0);

    // Parse the type.
    const Chimera *ptype = Chimera::parse(type);

    if (!ptype)
    {
        Chimera::raiseParseException(type, "a property");
        return -1;
    }

    pp->pyqtprop_parsed_type = ptype;

    Py_XINCREF(get);
    Py_XINCREF(set);
    Py_XINCREF(del);
    Py_XINCREF(doc);
    Py_XINCREF(reset);
    Py_INCREF(type);

    /* If no docstring was given and the getter has one, then use it. */
    if ((!doc || doc == Py_None) && get)
    {
#if PY_VERSION_HEX >= 0x02050000
        PyObject *get_doc = PyObject_GetAttrString(get, "__doc__");
#else
        PyObject *get_doc = PyObject_GetAttrString(get, const_cast<char *>("__doc__"));
#endif

        if (get_doc)
        {
            Py_XDECREF(doc);
            doc = get_doc;
        }
        else
            PyErr_Clear();
    }

    pp->prop_get = get;
    pp->prop_set = set;
    pp->prop_del = del;
    pp->prop_doc = doc;
    pp->pyqtprop_reset = reset;
    pp->pyqtprop_type = type;

    unsigned flags = 0;

    if (designable)
        flags |= 0x00001000;

    if (scriptable)
        flags |= 0x00004000;

    if (stored)
        flags |= 0x00010000;

    if (user)
        flags |= 0x00100000;

    if (constant)
        flags |= 0x00000400;

    if (final)
        flags |= 0x00000800;

    pp->pyqtprop_flags = flags;

    return 0;
}
