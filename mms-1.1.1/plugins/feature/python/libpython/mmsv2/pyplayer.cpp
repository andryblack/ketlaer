#include <Python.h>
#include "structmember.h"
#include "pyplayer.hpp"
#include "pyplaylist.hpp"
#include "movie.hpp"

using std::string;
using std::pair;
using std::vector;

#ifdef __cplusplus
extern "C" {
#endif

namespace pymms {

namespace python {

static pymms::player::PythonPlayer* getPlayer(string strPath, bool bPlayer)
{
  MovieConfig* pMovieConfig = S_MovieConfig::get_instance();

  const MyPair movieType = check_type(strPath, pMovieConfig->p_filetypes_m());
  
  if(movieType != emptyMyPair || bPlayer)
    return S_PythonVideoPlayer::get_instance();
  else
    return S_PythonAudioPlayer::get_instance();
}

static void runCallback(PyObject *pCallback, int iPos)
{
  if(pCallback)
  {
    PyObject *arglist;
    arglist = Py_BuildValue("(i)", iPos);

    PyObject* result = PyEval_CallObject(pCallback, arglist);
    Py_DECREF(arglist);

    if(result)
      Py_DECREF(result);
  }
}

static PyObject* Player_New(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  Player *self;
  bool bPlayer = false;

  self = (Player*)type->tp_alloc(type, 0);
  if (!self)
    return NULL;

  static char *kwlist[] = {"player", NULL};

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|b", kwlist, &bPlayer))
    return NULL;

  self->pPlayer = 0;
  self->pThread = 0;
  self->bRepeat = false;
  self->bPlayer = bPlayer;
  self->pCallback = NULL;

  self->pyPlayList = (PlayList*)PlayList_Type.tp_new(&PlayList_Type, args, kwds);
  if (!self->pyPlayList)
    return NULL;  

  return (PyObject*)self;
}

static PyObject* Player_setCallback(Player *self, PyObject *args)
{
  PyObject* pCallback = NULL;

  if (!PyArg_ParseTuple(args, "O", &pCallback))
    return NULL;

  if(pCallback)
  {
    if(PyCallable_Check(pCallback))
    {
      Py_XINCREF(pCallback);
      Py_XDECREF(self->pCallback);

      self->pCallback = pCallback;
    }
    else
    {
      PyErr_SetString(PyExc_TypeError, "parameter must be callable");
      return NULL;
    }
  }

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Player_play(Player *self, PyObject *args)
{
  char *strTitle = "";
  PyObject* pObject = NULL;

  if (!PyArg_ParseTuple(args, "|Os", &pObject, &strTitle))	
    return NULL;

  if(pObject && PlayList_Check(pObject))
  {
    if(self->pThread)
    {
      self->pThread->closeThread();
      delete self->pThread;
      self->pThread = 0;
    }

    PlayList* pyPlayList = (PlayList*)pObject;
      
    self->pyPlayList->pPlayList->clear();
    self->pyPlayList->pPlayList->copy(*(pyPlayList->pPlayList));

  }
  else if(pObject && PyString_Check(pObject))
  {
    if(self->pThread)
    {
      self->pThread->closeThread();
      delete self->pThread;
      self->pThread = 0;
    }

    char* strPath = PyString_AsString(pObject);
    pair<string, string> item(strPath, strTitle);

    self->pyPlayList->pPlayList->clear();
    self->pyPlayList->pPlayList->add(item);
  }
  else if(pObject && PyInt_Check(pObject))
  {
    int iItem = PyInt_AsLong(pObject);  
    self->pyPlayList->pPlayList->set(iItem);
  }

  if(!self->pyPlayList->pPlayList->empty())
  {
    self->pPlayer = getPlayer(self->pyPlayList->pPlayList->get().first, self->bPlayer);

    if(self->pPlayer)
    {
      runCallback(self->pCallback, self->pyPlayList->pPlayList->getCurrentPos());

      self->pPlayer->play(self->pyPlayList->pPlayList->get().first, 
			  self->pyPlayList->pPlayList->get().second);

      if(!self->pThread)
      {
        self->pThread = new PlayListThread((PyObject*)self);
        self->pThread->start();
      }
    }
  }

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Player_stop(Player *self, PyObject *args)
{
  if(self->pPlayer)
  {
    if(self->pThread)
    {
      self->pThread->closeThread();
      delete self->pThread;
      self->pThread = 0;
    }

    self->pPlayer->stop();
  }

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Player_pause(Player *self, PyObject *args)
{
  if(self->pPlayer)
    self->pPlayer->pause();

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Player_mute(Player *self, PyObject *args)
{
  if(self->pPlayer)
    self->pPlayer->mute();

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Player_volup(Player *self, PyObject *args)
{
  if(self->pPlayer)
    self->pPlayer->volup();
  
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Player_voldown(Player *self, PyObject *args)
{
  if(self->pPlayer)
    self->pPlayer->voldown();

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Player_ff(Player *self, PyObject *args)
{
  if(self->pPlayer)
    self->pPlayer->ff();

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Player_fb(Player *self, PyObject *args)
{
  if(self->pPlayer)
    self->pPlayer->fb();

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Player_next(Player* self, PyObject* args)
{
  self->pyPlayList->pPlayList->next();

  if(self->pPlayer && self->pPlayer->isPlaying() && !self->pyPlayList->pPlayList->empty())
  {
    self->pPlayer = getPlayer(self->pyPlayList->pPlayList->get().first, self->bPlayer);

    if(self->pPlayer)
    {
      runCallback(self->pCallback, self->pyPlayList->pPlayList->getCurrentPos());

      self->pPlayer->play(self->pyPlayList->pPlayList->get().first,
                          self->pyPlayList->pPlayList->get().second);
    }
  }

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Player_prev(Player* self, PyObject* args)
{
  self->pyPlayList->pPlayList->prev();

  if(self->pPlayer &&  self->pPlayer->isPlaying() && !self->pyPlayList->pPlayList->empty())
  {
    self->pPlayer = getPlayer(self->pyPlayList->pPlayList->get().first, self->bPlayer);

    if(self->pPlayer)
    {
      runCallback(self->pCallback, self->pyPlayList->pPlayList->getCurrentPos());

      self->pPlayer->play(self->pyPlayList->pPlayList->get().first,
                          self->pyPlayList->pPlayList->get().second);
    }
  }

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Player_getPlayList(Player *self, PyObject *args)
{
  if(self->pyPlayList->pPlayList->size())
  {
    PyObject* pPyList = PyList_New(0);
    vector<pair<string, string> >  titles = self->pyPlayList->pPlayList->toList();

    for(vector<pair<string, string> >::iterator i = titles.begin(), end = titles.end(); i != end; i++)
      PyList_Append(pPyList, (PyObject*)Py_BuildValue("(ss)", (*i).first.c_str(), (*i).second.c_str()));

    return (PyObject*)pPyList;
  }
  else
  {
    Py_INCREF(Py_None);
    return Py_None;
  }
}

static PyObject* Player_getPlayListAt(Player *self, PyObject *args)
{
  int iPos = -1;

  if (!PyArg_ParseTuple(args, "i", &iPos))
    return NULL;

  pair<string, string> title = self->pyPlayList->pPlayList->get(iPos);

  if(title != emptyMyPair)
  {
    return (PyObject*)Py_BuildValue("(ss)", title.first.c_str(), title.second.c_str());
  }
  else
  {
    Py_INCREF(Py_None);
    return Py_None;
  }
}

static PyObject* Player_setRepeat(Player* self, PyObject* args)
{
  bool bRepeat;

  if (!PyArg_ParseTuple(args, "b", &bRepeat))
    return NULL;

  self->bRepeat = bRepeat;

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Player_playListNext(Player* self, PyObject* args)
{
  if(self->bRepeat || !self->pyPlayList->pPlayList->isLastTrack())
  {
    self->pyPlayList->pPlayList->next();

    if(!self->pyPlayList->pPlayList->empty())
    {
      self->pPlayer = getPlayer(self->pyPlayList->pPlayList->get().first, self->bPlayer);

      if(self->pPlayer)
      {
        runCallback(self->pCallback, self->pyPlayList->pPlayList->getCurrentPos());

        self->pPlayer->play(self->pyPlayList->pPlayList->get().first,
                            self->pyPlayList->pPlayList->get().second);
      }
    }
  }
  else
  {
    if(self->pThread)
    {
      self->pThread->closeThread();
      delete self->pThread;
      self->pThread = 0;
    }    
  }

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Player_clearPlayList(Player* self, PyObject* args)
{
  if(self->pThread)
  {
    self->pThread->closeThread();
    delete self->pThread;
    self->pThread = 0;
  }

  self->pyPlayList->pPlayList->clear();

  Py_INCREF(Py_None);
  return Py_None;
}

static void Player_Dealloc(Player* self)
{
  if(self->pThread)
  {
    self->pThread->closeThread();
    delete self->pThread;
    self->pThread = 0;
  }

  Py_XDECREF(self->pCallback);

  Py_DECREF(self->pyPlayList);
  self->ob_type->tp_free((PyObject*)self);
}

static PyObject* Player_loadPlayList(Player* self, PyObject* args, PyObject* kwds)
{
  char* strPath = NULL;

  static char* kwlist[] = {"path", NULL};

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|", kwlist, &strPath))
    return NULL;

  if(strPath)
    self->pyPlayList->pPlayList->load(strPath);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Player_addToPlayList(Player* self, PyObject* args, PyObject* kwds)
{
  char* strPath = NULL;
  char* strTitle = "";

  static char* kwlist[] = {"path", "title", NULL};

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|s", kwlist, &strPath, &strTitle))
    return NULL;

  if(strPath)
  {
    pair<string, string> item(strPath, strTitle);
    self->pyPlayList->pPlayList->add(item);
  }

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Player_removeFromPlayList(Player* self, PyObject* args, PyObject* kwds)
{
  int iItem = -1;

  static char* kwlist[] = {"pos", NULL};

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &iItem))
    return NULL;

  self->pyPlayList->pPlayList->remove(iItem);

  if(self->pyPlayList->pPlayList->empty())
  {
    if(self->pThread)
    {
      self->pThread->closeThread();
      delete self->pThread;
      self->pThread = 0;
    }
  }

  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef Player_methods[] = {
  {"play", (PyCFunction)Player_play, METH_VARARGS, 0},
  {"setCallback", (PyCFunction)Player_setCallback, METH_VARARGS, 0},
  {"stop", (PyCFunction)Player_stop, METH_VARARGS, 0},
  {"pause", (PyCFunction)Player_pause, METH_VARARGS, 0},
  {"mute", (PyCFunction)Player_mute, METH_VARARGS, 0},
  {"volup", (PyCFunction)Player_volup, METH_VARARGS, 0},
  {"voldown", (PyCFunction)Player_voldown, METH_VARARGS, 0},
  {"ff", (PyCFunction)Player_ff, METH_VARARGS, 0},
  {"fb", (PyCFunction)Player_fb, METH_VARARGS, 0},
  {"next", (PyCFunction)Player_next, METH_VARARGS, 0},
  {"prev", (PyCFunction)Player_prev, METH_VARARGS, 0},
  {"getPlayList", (PyCFunction)Player_getPlayList, METH_VARARGS, 0},
  {"getPlayListAt", (PyCFunction)Player_getPlayListAt, METH_VARARGS, 0},
  {"setRepeat", (PyCFunction)Player_setRepeat, METH_VARARGS, 0},
  {"clearPlayList", (PyCFunction)Player_clearPlayList, METH_VARARGS, 0},
  {"loadPlayList", (PyCFunction)Player_loadPlayList, METH_VARARGS | METH_KEYWORDS, 0},
  {"addToPlayList", (PyCFunction)Player_addToPlayList, METH_VARARGS | METH_KEYWORDS, 0},
  {"removeFromPlayList", (PyCFunction)Player_removeFromPlayList, METH_VARARGS | METH_KEYWORDS, 0},  
  {"_playListNext", (PyCFunction)Player_playListNext, METH_VARARGS, 0},
  {NULL, NULL, 0, NULL}
};

PyTypeObject Player_Type = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/
	"mmsv2.Player",    /*tp_name*/
	sizeof(Player),      /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	(destructor)Player_Dealloc, 	/*tp_dealloc*/
	0,                         /*tp_print*/
	0,                         /*tp_getattr*/
	0,                         /*tp_setattr*/
	0,                         /*tp_compare*/
	0,                         /*tp_repr*/
	0,                         /*tp_as_number*/
	0,                         /*tp_as_sequence*/
	0,                         /*tp_as_mapping*/
	0,                         /*tp_hash */
	0,                         /*tp_call*/
	0,                         /*tp_str*/
	0,                         /*tp_getattro*/
	0,                         /*tp_setattro*/
	0,                         /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
	0,       /* tp_doc */
	0,		                     /* tp_traverse */
	0,		                     /* tp_clear */
	0,		                     /* tp_richcompare */
	0,		                     /* tp_weaklistoffset */
	0,		                     /* tp_iter */
	0,		                     /* tp_iternext */
	Player_methods,      /* tp_methods */
	0,                         /* tp_members */
	0,                         /* tp_getset */
	0,             /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	0,                         /* tp_init */
	0,                         /* tp_alloc */
	Player_New,          /* tp_new */
	0,		     /* tp_free */
};

}

}

#ifdef __cplusplus
}
#endif
