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
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>
#include <ctype.h>
#include <libconfig.h>
#include <RemoteMap.h>

IrMapFile::IrMapFile(const char* aRemoteType)
{
   struct stat st;
   char *   str;
   str=getenv("KETLAER_DIR");
   if (str==NULL) strcpy(KetlaerFolder, _QTDefaultBaseFolder); else strcpy(KetlaerFolder, str);
   if(stat(KetlaerFolder,&st) != 0)
   {
        strcpy(KetlaerFolder, _QTDefaultBaseFolder);
        if(stat(KetlaerFolder,&st) != 0)
        {
            //hum KETLAER_DIR is false or not set and the actual rep is not the standard one
            getcwd(KetlaerFolder,sizeof(KetlaerFolder));
        };
   }; 
   strcat(KetlaerFolder,"/etc");
   if(stat(KetlaerFolder,&st) != 0)
   {
        if (mkdir(KetlaerFolder,0777)!=0) fprintf(stderr,"[RemoteMap]Error while creating %s\n",KetlaerFolder) ;
   };
   strncpy(RemoteType,aRemoteType,20);
   strcpy(FileName,KetlaerFolder);
   strcat(FileName,"/remotes/");
   strcat(FileName,RemoteType);
   strcat(FileName,".map");
   printf("[RemoteMap]Remote file is '%s'\n",FileName);
   IR_Protocol=0;
   IR_Table_Elem=0;
   QT_Table_Elem=0;
   IRFileLoaded=false;
   QTFileLoaded=false;
   LoadFiles();
};

IrMapFile::~IrMapFile()
{
   int         i;
   
   for (i=0;i<IR_Table_Elem;i++)
   {
      if (IR_Table[i]!=NULL) free(IR_Table[i]);
   }
   for (i=0;i<QT_Table_Elem;i++)
   {
      if (QT_Table[i]!=NULL) free(QT_Table[i]);
   }
   if (IR_Table!=NULL) free(IR_Table);
   if (QT_Table!=NULL) free(QT_Table);
   IR_Table_Elem=0;
   QT_Table_Elem=0;
};

void IrMapFile::PrintTable()
{
   printf("=============================================================\n");
   printf("==                    Remote map Table                     ==\n");
   printf("=============================================================\n");
   printf("==  %-15s  %-15s %15s       ==\n","Remote code", "Qt name", "Qt value");
   printf("=============================================================\n");
   int i;
   for (i=0;i<IR_Table_Elem;i++)
   {
      printf("==  %-15X  %-15s %15X       ==\n", IR_Table[i]->code, IR_Table[i]->event, GetQtValueKey(IR_Table[i]->event));
   };
   printf("=============================================================\n");
}
bool IrMapFile::LoadFiles()
{
   IRFileLoaded=false;
   QTFileLoaded=false;
   
   struct stat buf;
   
   if (stat(FileName,&buf))
   {
      if (strcmp(RemoteType,"EKAH110")==0)
      {
         if (CreateIRDefaultFile())
         {
            if (ReadIRFile())
            {
               IRFileLoaded=true;
            }
            else
            {
               fprintf(stderr,"[RemoteMap]Error while reading IR default setting file !\n");
               return false;
            }
         }
         else
         {  
            fprintf(stderr,"[RemoteMap]Error while creating default IR setting file !\n");
            return false;
         };
      }
      else
      {
         fprintf(stderr,"[RemoteMap]Error file '%s' not found!\n",FileName);
         return false;
      }     
   }
   else
   {
      if (ReadIRFile())
      {
         IRFileLoaded=true;
      }
      else
      {
          fprintf(stderr,"[RemoteMap]Error while reading IR file '%s'!\n",FileName);
          return false;
      }
   }
   if (stat(_QTFileName,&buf))
   {
      if (CreateQTDefaultFile())
      {
         if (ReadQTFile())
         {
            QTFileLoaded=true;
         }
         else
         {
            fprintf(stderr,"[RemoteMap]Error while reading '%s' default setting file !\n",_QTFileName);
            return false;
         }
       }
       else
       {  
          fprintf(stderr,"[RemoteMap]Error while creating '%s' default setting file !\n",_QTFileName);
          return false;
       };     
   }
   else
   {
      if (ReadQTFile())
      {
         QTFileLoaded=true;
      }
      else
      {
          fprintf(stderr,"[RemoteMap]Error while reading '%s' default setting file  !\n",_QTFileName);
          return false;
      }
   }
   return true;
};

int IrMapFile::GetQtKey(int IrCode)
{
   int      i;
   for (i=0;i<IR_Table_Elem;i++)
   {
      if (IR_Table[i]->code==IrCode)
      {
         return GetQtValueKey(IR_Table[i]->event);
      }
   }
   return -1;
};
int IrMapFile::GetQtValueKey(char *Key)
{
   int      i;
   for (i=0;i<QT_Table_Elem;i++)
   {
      if (strcmp(QT_Table[i]->key,Key)==0)
      {
         return QT_Table[i]->value;
      }
   }
   return -1;
};

bool IrMapFile::CreateIRDefaultFile()
{
    FILE * f;
    struct stat st;
    char    dir[1024];
    strcpy(dir,KetlaerFolder);
    strcat(dir,"/remotes");
    if(stat(dir,&st) != 0)
    {
        if (mkdir(dir,0777)!=0) fprintf(stderr,"[RemoteMap]Error while creating '%s'\n",dir) ;
    };
    f = fopen(FileName, "wb");
    if (f != NULL)
    {
       
/////////////////////////////////////////////////
//           Default file settings             //
/////////////////////////////////////////////////
// Must be modified if structure change! 
        

        fprintf(f,"//Remote Map File\n");
        fprintf(f,"\n");
        fprintf(f,"name = \"EKAH110 IR_CODE\";\n");
        fprintf(f,"\n");
        fprintf(f,"////////////////////////////////////////////////////////////////////////////////\n");
        fprintf(f,"//  Remote protocol                                                           //\n");
        fprintf(f,"//                                                                            //\n");
        fprintf(f,"//  0 = NEC                                                                   //\n");
        fprintf(f,"//  1 = RC5                                                                   //\n");
        fprintf(f,"//  2 = SHARP                                                                 //\n");
        fprintf(f,"//  3 = SONY                                                                  //\n");
        fprintf(f,"//  4 = C03                                                                   //\n");
        fprintf(f,"//  5 = RC6                                                                   //\n");
        fprintf(f,"//                                                                            //\n");
        fprintf(f,"//                                                                            //\n");
        fprintf(f,"//                                                                            //\n");
        fprintf(f,"////////////////////////////////////////////////////////////////////////////////\n");
        fprintf(f,"\n");
        fprintf(f,"protocol = 0;\n");
        fprintf(f,"\n");
        fprintf(f,"\n");
        fprintf(f,"\n");
        fprintf(f,"\n");
        fprintf(f,"\n");
        fprintf(f,"////////////////////////////////////////////////////////////////////////////////\n");
        fprintf(f,"//  Remote map format.                                                        //\n");
        fprintf(f,"//                                                                            //\n");
        fprintf(f,"//  code = Enter the ir code, mappedto = Enter the event to be mapped         //\n");
        fprintf(f,"//                                                                            //\n");
        fprintf(f,"//  Your ir code can be obtened only with special app not already developped  //\n");
        fprintf(f,"//                                                                            //\n");
        fprintf(f,"//  The event list to be mapped http://doc.qt.nokia.com/4.6/qt.html#Key-enum  //\n");
        fprintf(f,"//                                                                            //\n");
        fprintf(f,"//                                                                            //\n");
        fprintf(f,"//                                                                            //\n");
        fprintf(f,"//                                                                            //\n");
        fprintf(f,"////////////////////////////////////////////////////////////////////////////////\n");
        fprintf(f,"remotemap =\n");
        fprintf(f,"(\n");
        fprintf(f,"  { code =  \"9F00EE11\"; mappedto = \"key_0\"; },\n");
        fprintf(f,"  { code =  \"9F00FA05\"; mappedto = \"key_1\"; },\n");
        fprintf(f,"  { code =  \"9F00F609\"; mappedto = \"key_2\"; },\n");
        fprintf(f,"  { code =  \"9F00EA15\"; mappedto = \"key_3\"; },\n");
        fprintf(f,"  { code =  \"9F00FB04\"; mappedto = \"key_4\"; },\n");
        fprintf(f,"  { code =  \"9F00F708\"; mappedto = \"key_5\"; },\n");
        fprintf(f,"  { code =  \"9F00EB14\"; mappedto = \"key_6\"; },\n");
        fprintf(f,"  { code =  \"9F00FF00\"; mappedto = \"key_7\"; },\n");
        fprintf(f,"  { code =  \"9F00EF10\"; mappedto = \"key_8\"; },\n");
        fprintf(f,"  { code =  \"9F00F30C\"; mappedto = \"key_9\"; },\n");
        fprintf(f,"  { code =  \"9F00BC43\"; mappedto = \"key_Up\"; },\n");
        fprintf(f,"  { code =  \"9F00F50A\"; mappedto = \"key_Down\"; },\n");
        fprintf(f,"  { code =  \"9F00F906\"; mappedto = \"key_Left\"; },\n");
        fprintf(f,"  { code =  \"9F00F10E\"; mappedto = \"key_Right\"; },\n");
        fprintf(f,"  { code =  \"9F00FD02\"; mappedto = \"key_Enter\"; },\n");
        fprintf(f,"  { code =  \"9F00A55A\"; mappedto = \"key_Home\"; },\n");
        fprintf(f,"  { code =  \"9F00A857\"; mappedto = \"key_PowerOff\"; },\n");
        fprintf(f,"  { code =  \"9F00AF50\"; mappedto = \"key_MediaPlay\"; },\n");
        fprintf(f,"  { code =  \"9F00EC13\"; mappedto = \"key_MediaStop\"; }\n");
        fprintf(f,");\n");
        fprintf(f,"//EOF\n");
        fclose(f);
    }
    else
    {
      fprintf(stderr,"[RemoteMap]Error while trying to write default remote file.\n");
      return false;
    }
    return true;
};

bool IrMapFile::CreateQTDefaultFile()
{
    FILE * f;
    struct stat st;
    if(stat("/remotes",&st) != 0)
    {
        mkdir("remotes",0777);
    };
    f = fopen(_QTFileName, "wb");
    if (f != NULL)
    {
       
/////////////////////////////////////////////////
//           Default file settings             //
/////////////////////////////////////////////////
// Must be modified if structure change! 
        

        fprintf(f,"//Event map file for remote.\n");
        fprintf(f,"\n");
        fprintf(f,"name = \"QT_Event V1\";\n");
        fprintf(f,"\n");
        fprintf(f,"\n");
        fprintf(f,"\n");
        fprintf(f,"\n");
        fprintf(f,"////////////////////////////////////////////////////////////////////////////////\n");
        fprintf(f,"//  QT code can be added.                                                     //\n");
        fprintf(f,"//                                                                            //\n");
        fprintf(f,"//                                                                            //\n");
        fprintf(f,"//  The event list to be mapped http://doc.qt.nokia.com/4.6/qt.html#Key-enum  //\n");
        fprintf(f,"//                                                                            //\n");
        fprintf(f,"//                                                                            //\n");
        fprintf(f,"//                                                                            //\n");
        fprintf(f,"//                                                                            //\n");
        fprintf(f,"////////////////////////////////////////////////////////////////////////////////\n");
        fprintf(f,"qt_event =\n");
        fprintf(f,"(\n");
        fprintf(f,"  { key = \"key_0\" ; value = \"30\"; },\n");
        fprintf(f,"  { key = \"key_1\" ; value = \"31\"; },\n");
        fprintf(f,"  { key = \"key_2\" ; value = \"32\"; },\n");
        fprintf(f,"  { key = \"key_3\" ; value = \"33\"; },\n");
        fprintf(f,"  { key = \"key_4\" ; value = \"34\"; },\n");
        fprintf(f,"  { key = \"key_5\" ; value = \"35\"; },\n");
        fprintf(f,"  { key = \"key_6\" ; value = \"36\"; },\n");
        fprintf(f,"  { key = \"key_7\" ; value = \"37\"; },\n");
        fprintf(f,"  { key = \"key_8\" ; value = \"38\"; },\n");
        fprintf(f,"  { key = \"key_9\" ; value = \"39\"; },\n");
        fprintf(f,"  { key = \"key_Up\"; value = \"01000013\";},\n");
        fprintf(f,"  { key = \"key_Down\" ; value = \"01000015\"; },\n");
        fprintf(f,"  { key = \"key_Left\" ; value = \"01000012\"; },\n");
        fprintf(f,"  { key = \"key_Right\" ; value = \"01000014\"; },\n");
        fprintf(f,"  { key = \"key_Enter\" ; value = \"01000005\"; },\n");
        fprintf(f,"  { key = \"key_Home\" ; value = \"01000010\"; },\n");
        fprintf(f,"  { key = \"key_PowerOff\" ; value = \"010000b7\"; },\n");
        fprintf(f,"  { key = \"key_MediaPlay\" ; value = \"01000080\"; },\n");
        fprintf(f,"  { key = \"key_MediaStop\" ; value = \"01000081\"; },\n");
        fprintf(f,"  { key = \"key_VolumeUp\" ; value = \"01000072\"; },\n");
        fprintf(f,"  { key = \"key_VolumeDown\" ; value = \"01000070\"; },\n");
        fprintf(f,"  { key = \"key_VolumeMute\" ; value = \"01000071\"; }\n");
        fprintf(f,");\n");
        fprintf(f,"//EOF\n");
        fclose(f);
    }
    else
    {
      fprintf(stderr,"[RemoteMap]Error while trying to write default QtEvent.qtm file.\n");
      return false;
    }
    return true;
};

bool IrMapFile::ReadIRFile()
{

  config_t           cfg;
  config_setting_t   *setting;
  const char         *str;
  char               namebuffer[50];
  int                int_v;
  
  config_init(&cfg);

  /* Read the file. If there is an error, report it and exit. */
  if(! config_read_file(&cfg, FileName))
  {
    fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
            config_error_line(&cfg), config_error_text(&cfg));
    config_destroy(&cfg);
    return false;
  }
  strcpy(namebuffer,RemoteType);
  strcat(namebuffer," IR_CODE");

  /* Get the store name. */
  if(config_lookup_string(&cfg, "name", &str))
  {
    if (strcmp(str,namebuffer)==0)
    { 
      printf("[RemoteMap]Loading IR configuration file... \n");
      if (config_lookup_int(&cfg, "protocol", &int_v))
      {
         IR_Protocol=int_v;
         setting = config_lookup(&cfg, "remotemap");
         if(setting != NULL)
         {
            int count = config_setting_length(setting);
            int i;
            IR_Table=(sIR_Table**) malloc(count * sizeof(sIR_Table*));
       
            for(i = 0; i < count; ++i)
            {
               config_setting_t *remote = config_setting_get_elem(setting, i); 

               /* Only output the record if all of the expected fields are present. */
               const char *code, *event;
      
               if(!(config_setting_lookup_string(remote, "code", &code)
                     && config_setting_lookup_string(remote, "mappedto", &event)))
               continue;
      
               IR_Table[i]=(sIR_Table*) malloc(sizeof(sIR_Table));
               sscanf(code, "%X", &IR_Table[i]->code); // Convert ascii string to int
               strcpy(IR_Table[i]->event, event);
               IR_Table_Elem++;
            }
         }
         else
         {
            fprintf(stderr,"[RemoteMap]Cannot found appropriate IR protocol in %s!\n",FileName);
            goto EXIT_READFILE1;
         }
      }
      else
      {
         fprintf(stderr,"[RemoteMap]Cannot found appropriate IR setting in %s!\n",FileName);
         goto EXIT_READFILE1;
      }

   }
   else
   {
       fprintf(stderr, "'name' setting in IR configuration file is wrong --> '%s'.\n",str);
       goto EXIT_READFILE1;
   }
    
  }
  else
  {
    fprintf(stderr, "No 'name' setting in configuration file.\n");
    goto EXIT_READFILE1;
  }


  config_destroy(&cfg);
  return true;
EXIT_READFILE1:
  config_destroy(&cfg);
  return false;
  
};

bool IrMapFile::ReadQTFile()
{

  config_t           cfg;
  config_setting_t   *setting;
  const char         *str;
  
  config_init(&cfg);

  /* Read the file. If there is an error, report it and exit. */
  if(! config_read_file(&cfg, _QTFileName))
  {
    fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
            config_error_line(&cfg), config_error_text(&cfg));
    config_destroy(&cfg);
    return false;
  }

  /* Get the store name. */
  if(config_lookup_string(&cfg, "name", &str))
  {
    if (strcmp(str,"QT_Event V1")==0)
    { 
      printf("[RemoteMap]Loading QT configuration file... \n");
      setting = config_lookup(&cfg, "qt_event");
      if(setting != NULL)
      {
         int count = config_setting_length(setting);
         int i;
         QT_Table=(sQT_Table**) malloc(count * sizeof(sQT_Table*));
    
         for(i = 0; i < count; ++i)
         {
            config_setting_t *qtevent = config_setting_get_elem(setting, i);

            /* Only output the record if all of the expected fields are present. */
            const char *key, *value;
      
            if(!(config_setting_lookup_string(qtevent, "key", &key)
                  && config_setting_lookup_string(qtevent, "value", &value)))
            continue;
      
            QT_Table[i]=(sQT_Table*) malloc(sizeof(sQT_Table));
            sscanf(value, "%X", &QT_Table[i]->value); // Convert ascii string to int
            strcpy(QT_Table[i]->key, key);
            QT_Table_Elem++;
         }
      }
      else
      {
         fprintf(stderr,"[RemoteMap]Cannot found appropriate QT setting in %s!\n",_QTFileName);
         goto EXIT_READFILE2;
      }

   }
   else
   {
       fprintf(stderr, "'name' setting in QT configuration file is wrong --> '%s'.\n",str);
       goto EXIT_READFILE2;
   }
    
  }
  else
  {
    fprintf(stderr, "No 'name' setting in QT configuration file.\n");
    goto EXIT_READFILE2;
  }

  config_destroy(&cfg);
  return true;
EXIT_READFILE2:
  config_destroy(&cfg);
  return false;
  
};
