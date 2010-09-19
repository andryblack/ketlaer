// This contains the definitions for the implementation of pyqtProperty.
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


#ifndef _QPYCORE_PYQTPROPERTY_H
#define _QPYCORE_PYQTPROPERTY_H


#include <Python.h>


class Chimera;


extern "C" {

// This defines the structure of a PyQt property.
typedef struct {
    PyObject_HEAD

    // The following members must match the propertyobject structure defined in
    // Objects/descrobject.c of the Python source because we choose to
    // sub-class from the standard Python property (even though Python doesn't
    // publish this structure).  Alternatively we could implement pyqtProperty
    // as a completely unrelated type that supported the descriptor protocol in
    // the same way.
    PyObject *prop_get;
    PyObject *prop_set;
    PyObject *prop_del;
    PyObject *prop_doc;

    // The freset callable.
    PyObject *pyqtprop_reset;

    // The property type.
    PyObject *pyqtprop_type;

    // The parsed type information.
    const Chimera *pyqtprop_parsed_type;

    // The DESIGNABLE, SCIPTABLE, STORED, USER, CONSTANT and FINAL flags.
    unsigned pyqtprop_flags;

    // The property's sequence number that determines the position of the
    // property in the QMetaObject.
    uint pyqtprop_sequence;
} qpycore_pyqtProperty;


// This implements the PyQt version of the standard Python property type.
extern PyTypeObject qpycore_pyqtProperty_Type;

}


#endif
