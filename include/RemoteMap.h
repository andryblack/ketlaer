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
#ifndef REMOTEMAPFILE_H
#define REMOTEMAPFILE_H

#define _QTFileName "remotes/QTEvent.qtm"
class IrMapFile
{
   public:
           IrMapFile(const char*aRemoteType="EKAH110");
           ~IrMapFile();
           
           bool            IRFileLoaded, QTFileLoaded;
           
           
           int    GetQtKey(int IrCode);
           void   PrintTable();
   private:
   
           typedef struct 
           {
              int          code;
              char         event[20];
           }sIR_Table;
           sIR_Table       *pIR_Table;
           sIR_Table       **IR_Table;
           int             IR_Table_Elem;
           
           typedef struct 
           {
              char        key[20];
              int         value;
           }sQT_Table;
           sQT_Table       **QT_Table;
           int             QT_Table_Elem;
           
           char            RemoteType[21];
           int             IR_Protocol;
           char            FileName[255];

           int    GetQtValueKey(char *Key);
           bool   ReadIRFile();
           bool   CreateIRDefaultFile();
           bool   ReadQTFile();
           bool   CreateQTDefaultFile();
           bool   LoadFiles();
           
};
#endif //REMOTEMAPFILE_H
