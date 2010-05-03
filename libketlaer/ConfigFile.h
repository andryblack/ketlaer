/****************************************************************************
**
** Class declaration of settings management.
** It will initialise some variable from file on realtek based hardware.
** The purpose is to have interface with libconfig and let the class do
** maximum to set correctly initialisation variable.
** Write still need to be implemented.
**
**
**
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
#ifndef CONFIGFILE_H
#define CONFIGFILE_H
#define _QTDefaultBaseFolder "/usr/local/etc/ketlaer"
class ConfigFile
{
        
   public:
        
            ConfigFile(const char* aFileName="settings.conf");
            ~ConfigFile();
              

            bool           FileLoaded;
            int            GetTvSystem();
            char           *GetRemoteType();
            int            GetAspectRatio();
            int            GetVideoStandardPI(); 
            void           SetConfigValues();

   private:
            char           FileName[255];            
            int            ScreenSize;
            char           RemoteType[21];
            int            VideoStandardPI;
            int            AspectRatio;
            char           KetlaerFolder[1024];
            
            
            bool CreateDefaultFile();
            bool ReadFile(); 
            bool LoadFile();               
};
#endif //CONFIGFILE_H
