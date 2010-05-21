#ifndef PYUTIL_HPP
#define PYUTIL_HPP

#include <Python.h>
#include "structmember.h"
#include <cc++/thread.h>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

#if PY_VERSION_HEX < 0x02050000 && !defined(PY_SSIZE_T_MIN)
typedef int Py_ssize_t;
#endif

// Python under Linux for some reason doesn't play nice with Py_AddPendingCall
// and Py_MakePendingCalls, so we have our own versions.

#define Py_AddPendingCall _Py_AddPendingCall
#define Py_MakePendingCalls _Py_MakePendingCalls
  
void _Py_AddPendingCall(int(*func)(void*), void *arg);
void _Py_MakePendingCalls();
void PyInitPendingCalls();

void Py_WaitForActionEvent();
void Py_PulseActionEvent();

void Py_SetWaiting();
void Py_WaitForReturnEvent();
void Py_PulseReturnEvent();

int Py_wCharToChar(std::string& s, PyObject* pObject); 

#ifdef __cplusplus
}
#endif

#endif
