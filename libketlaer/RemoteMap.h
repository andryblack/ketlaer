/****************************************************************************
**
** Class declaration of remote control management
** It will get easy access on different remote on realtek based hardware
** and olso make interface with Qt_event
**
**
** Licence
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
**
****************************************************************************/
#ifndef _h_libketlaer_RemoteMap_h
#define _h_libketlaer_RemoteMap

#define DEFAULT_DIR  "/usr/local/etc/ketlaer"

class IrMapFile
{
 public:

  IrMapFile(const char *aRemoteType="EKAH110");
  ~IrMapFile();
           
  bool   IRFileLoaded;
  bool   QTFileLoaded;
           
  int    GetQtKey(int IrCode);
  int    GetProtocol();
  void   PrintTable();

 private:
   
  typedef struct 
  {
    int          code;
    char         event[20];
  }sIR_Table;
  sIR_Table  *pIR_Table;
  sIR_Table **IR_Table;
  int         IR_Table_Elem;
           
  typedef struct 
  {
    char        key[20];
    int         value;
  }sQT_Table;
  sQT_Table **QT_Table;
  int         QT_Table_Elem;
           
  char RemoteType[21];
  int  IR_Protocol;

  int    GetQtValueKey(char *Key);
  bool   LoadFiles(char *dir);
  bool   ReadIRFile(char *name);
  bool   CreateIRDefaultFile(char *name);
  bool   ReadQTFile(char *name);
  bool   CreateQTDefaultFile(char *name);           
};
#endif //REMOTEMAPFILE_H
