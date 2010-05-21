/***************************************************************************
*   Copyright (C) 2007                                                    *
*   This is part of the mms software                                      *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

#include <cstring>
#include <cstdlib>
#include <string>
#include <list>
#include <sstream>
#include <fstream>
#include <iostream>
#include "CD_Tag.h"
#include "kernelheaders.hpp"
#include <stdarg.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>             /* per  errno */
#include "sha1.h"
#include "base64.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "common.hpp"
#include "wget_wrapper.hpp"
#define XA_INTERVAL			((60 + 90 + 2) * CD_FRAMES)
#define FRAMES_X_SEC		75
#define __func__  __PRETTY_FUNCTION__


/*
CDDB_Id is a 32 bit integer represented as a string
with the form of XXYYYYZZ.
the string is made of hex digits where:
-	XX is a checksum based on the starting time of each track on the CD
-	YYYY is the total time of the CD in seconds
-	ZZ represent the number of tracks on the CD (Max 99)
*/


CD_Tag::CD_Tag()
{
  status			= STATUS_VIRGIN;
  cddb_id			= "";
  mbrainz_id		= "";
  first_track_num		=-1;
  last_track_num		=-1;
  titles.clear();
  memset (&track_off, 0, sizeof(track_off));
  verbosity = VERBOSE_ERRS; 
  query_entries.clear();
  thisappname = "mms";
  thisappversion ="1.1.0";
  max_num_of_entries= MAX_NUM_OF_ENTRIES;
}


CD_Tag::~CD_Tag()
{
}

unsigned int CD_Tag::GetStatus(){
  return status;
}

int CD_Tag::cddb_sum(unsigned int n){
  int sum = 0;
  while (n > 0) {
    sum = sum + (n % 10);
    n = n / 10;
  }
  return sum;
}


int CD_Tag::TagCD(const char *cd_path){  //Do all things related to CD
  memset (&track_off, 0, sizeof(track_off));
  ResetErr();
  bool multisession = false;
  int fd;  //File Descriptor

  if ((fd = open(cd_path, O_RDONLY | O_NONBLOCK)) < 0 ) {
    Log_Msg(MSG_ERROR, "%s: Unable to open %s - %s\n",  __func__, cd_path,  strerror(errno));
//		last_error << "Unable to open " << cd_path <<" - " << strerror(errno) << endl;
    return -1;
  }
  struct cdrom_tochdr toch;
  struct cdrom_multisession cd_ms;

  if(ioctl(fd, CDROMREADTOCHDR, &toch) < 0) {
    Log_Msg(MSG_ERROR, "%s: Unable to get TOC from CD - %s\n", __func__, strerror(errno));
//		last_error << "Unable to get TOC from CD "  << " - " << strerror(errno) << endl;
    close(fd);
    return -1;
  }
  first_track_num = toch.cdth_trk0;
  last_track_num  = toch.cdth_trk1;
  cd_ms.addr_format = CDROM_LBA;
  if( ioctl(fd, CDROMMULTISESSION, &cd_ms)<0) {
    Log_Msg(MSG_ERROR, "%s: Can't determine CD type - %s\n", __func__, strerror(errno));
//		last_error << "Can't determine CD type " << " - " << strerror(errno) << endl;
    close(fd);
    return -1;
  }
  if ( cd_ms.xa_flag ){ //if this is a multisession disc
                          //we ignore the last track and we can set the leadout now
                          //FIXME: this does not handle multi-
                          //session discs with more than one 
                          //track in the second session.
    multisession=true;
    last_track_num -=1;
    track_off[0]=cd_ms.addr.lba - XA_INTERVAL + 150;
  }
  /*We now have the ZZ part of the CDDB ID. */ 
  struct cdrom_tocentry tentry;
  for (unsigned int t = first_track_num; t <= last_track_num; t++){
    memset(&tentry, 0, sizeof(tentry));
    tentry.cdte_track = t;
    tentry.cdte_format = CDROM_LBA;
    if (ioctl(fd, CDROMREADTOCENTRY, &tentry) < 0) {
                        //TODO:add error messages and stuff
      Log_Msg(MSG_ERROR, "%s: Unable to get TOC entries from CD - %s\n", __func__,strerror(errno));
//		last_error << "Unable to get TOC entries from CD " << " - " << strerror(errno) << endl;
      close(fd);
      return -1;
    }  
    track_off[t] = 150 + tentry.cdte_addr.lba;
  }
        
        //and finally we read the leadout track (for single session discs only)
  if (!multisession){
    memset(&tentry, 0, sizeof(tentry));
    tentry.cdte_track = CDROM_LEADOUT;
    tentry.cdte_format = CDROM_LBA;
    if (ioctl(fd, CDROMREADTOCENTRY, &tentry) < 0) {
      Log_Msg(MSG_ERROR,  "%s: Unable to get TOC entries from CD - %s\n", __func__, strerror(errno));
//			last_error << "Unable to get TOC entries from CD " << " - " << strerror(errno) << endl;
      close(fd);
      return -1;
    }  
    track_off[0] = 	tentry.cdte_addr.lba+150;
  }
  close(fd);
        
        /* we have everything we need to calculate both
  * CDDB ID and MusicBrainz ID
  * CDDB is first...
        */ 
  close(fd);
  char strbuf[256];
  unsigned int xx,yyyy,zz;
  xx=0;
  for (unsigned int t = 1; t <=last_track_num; t++){
    xx+=cddb_sum(track_off[t]/FRAMES_X_SEC);
  }
  yyyy=(track_off[0] /FRAMES_X_SEC - track_off[1]/FRAMES_X_SEC);
  zz=last_track_num;
  sprintf(strbuf,"%02x%04x%02x", (xx %255),(yyyy&0xffff),zz);
  cddb_id=strbuf;
        
  /*MusicBrainz ID calculation*/	
  SHA_INFO        sha;
  sha_init(&sha);
        
  sprintf(strbuf,"%02X", first_track_num);
  sha_update(&sha,reinterpret_cast<unsigned char*>(strbuf),strlen(strbuf));
        
  sprintf(strbuf,"%02X", last_track_num);
  sha_update(&sha,reinterpret_cast<unsigned char*>(strbuf),strlen(strbuf));
  memset(&strbuf,0,sizeof(strbuf));
  for (int i = 0; i < 100; i++) {
    sprintf(strbuf, "%08X", track_off[i]);
    sha_update(&sha, reinterpret_cast<unsigned char*>(strbuf), strlen(strbuf));
  }
        
  unsigned char digest[20], *base64;
  sha_final(digest, &sha);
        
  unsigned long  size;
  base64 = rfc822_binary(digest, sizeof(digest), &size);
  memcpy(strbuf, base64, size);
  strbuf[size]='\0';
  mbrainz_id=strbuf;
  free(base64);
  status=STATUS_GOT_IDS;
  return 0;
}


std::string  CD_Tag::CDDB_Id(){
  ResetErr();
  if (status < STATUS_GOT_IDS){
    Log_Msg(MSG_ERROR, "%s: Run CD_Tag::TagCD first!!!\n", __func__);
    return "";
  }
  return cddb_id;
}


std::string CD_Tag::MBrainz_ID(){
  ResetErr();
  if (status < STATUS_GOT_IDS){
    Log_Msg(MSG_ERROR, "%s: Run CD_Tag::TagCD first!!!\n", __func__);
    return "";
  }
  return mbrainz_id;
}


int CD_Tag::GetTrackOffset(unsigned int track_num){
  ResetErr();
  if (status < STATUS_GOT_IDS){
    Log_Msg(MSG_ERROR, "%s: Run CD_Tag::TagCD first!!!\n", __func__);
    return -1;
  }
        
  if (track_num > last_track_num){
    Log_Msg(MSG_ERROR, "%s: Run CD_Tag::Track number is out of range\n", __func__);
//		last_error << "Track Number is out of range" << endl;
    return -1;
  }
  return  track_off[track_num];
}


int CD_Tag::LastTrack(){
  ResetErr();
  if (status < STATUS_GOT_IDS){
    Log_Msg(MSG_ERROR, "%s: Run CD_Tag::TagCD first!!!\n", __func__);
//		last_error << "Run CD_Tag::TagCD first!!!" << endl;
    return -1;
  }
  return last_track_num;
}


int CD_Tag::FirstTrack(){
  ResetErr();
  if (status < STATUS_GOT_IDS){
    Log_Msg(MSG_ERROR, "%s: Run CD_Tag::TagCD first!!!\n",__func__);
//		last_error << "Run CD_Tag::TagCD first!!!" << endl;
    return -1;
  }
  return first_track_num;
}


int CD_Tag::NumOfTracks(){
  ResetErr();
  if (status < STATUS_GOT_IDS){
    Log_Msg(MSG_ERROR, "%s: Run CD_Tag::TagCD first!!!\n",__func__);
//		last_error << "Run CD_Tag::TagCD first!!!" << endl;
    return -1;
  }
  return last_track_num - first_track_num +1;
}


int CD_Tag::TrackTime(unsigned int track_num, int *hours,int *mins, int *secs){
  ResetErr();
  if (status < STATUS_GOT_IDS){
    Log_Msg(MSG_ERROR, "%s: Run CD_Tag::TagCD first!!!\n", __func__);
//		last_error << "Run CD_Tag::TagCD first!!!" << endl;
    return -1;
  }		
  if (track_num > last_track_num){
    Log_Msg(MSG_ERROR, "%s: Track Number is out of range: last track is %02d, you asked for %02d\n",__func__, last_track_num, track_num);
//		last_error << "Track Number is out of range" << endl;
    return -1;
  }
  int sec;
  if (track_num !=0){
    int tt=(track_num < last_track_num) ? track_num+1 : 0;
    sec=(track_off[tt] - track_off[track_num])/FRAMES_X_SEC;
  }
  else
    sec=track_off[0]/FRAMES_X_SEC;
  if (hours!=NULL && mins !=NULL && secs !=NULL){
    *hours=sec/3600;
    *mins=sec/60%60;
    *secs=sec%60;
  }
  return sec;
}


void CD_Tag::ResetErr(){
  last_error.clear();
  last_error.str("");
}


//The following is some sort of printf function especially
//made for logging etc.
int CD_Tag::Log_Msg(int msgtype, const char *fmt, ...){
  string str;
  char *p, *np;
  va_list ap;
  int  n;
  size_t size, nextsize;
  size = 32;
        
  if (( p =reinterpret_cast<char*>(malloc(size))) == NULL){
    fprintf(stderr, "%s, Out of memory error in CD_Tag::Log_Msg()\n", __func__); //warn user no matter what
    return -1;
  }
  while (true){
    va_start(ap, fmt);
    n = vsnprintf (p, size, fmt, ap);
    va_end(ap);
    if (n == -1){// buffer is too small but we don't know how much we should allocate;
      nextsize = size*2;
    }
                
    else if (static_cast<unsigned int>(n) == size){ //buffer is too small because eol couldn't fit
      nextsize = size+3; 
    }
                
    else if (static_cast<unsigned int>(n) == size -1){	//This is ambiguous. It may mean that the output string
      nextsize = size*2;					//was truncated to fit the buffer
    }

    else if (static_cast<unsigned int>(n)> size) {	//this is telling us how much the buffer should be
      nextsize = static_cast<unsigned int>(n) +3;  //3 bytes more just to stay on the safe side
    }

    else {//string fits in the buffer!!!!!!!
      break;
    }
    if ((np = reinterpret_cast<char*>(realloc (p, nextsize))) == NULL) {
      free(p);
      fprintf(stderr, "%s: Out of memory error in CD_Tag::Log_Msg()\n",__func__);
      return -1;
    } else {
      size = nextsize;
      p = np;
    }
  }
  if (msgtype == MSG_ERROR){
    ResetErr();
    last_error << p;
  }
  if (verbosity >= msgtype)
    fprintf(stderr,"CD_Tag: %s", p);
  free(p);
  return n;					
        
}


const char * CD_Tag::GetLastErr(){
  ostringstream str;
  str <<"CD_Tag: ";
  if(last_error.str().empty())
    str << "No error" << endl;
  else	
    str << last_error.str();
  return str.str().c_str();
}


int CD_Tag::CDDB_Open(string servaddr, int portno){
  ResetErr();
  int sd;
  if ((sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){ 
    Log_Msg(MSG_ERROR, "%s: Unable to create socket!!! - %s\n",__func__, strerror(errno));
//		last_error << "Unable to create socket!!! - " << strerror(errno)  << endl;
    return -1;
  }
        
  struct sockaddr_in sin;
  struct hostent *host = gethostbyname(servaddr.c_str());
  if (host == NULL){
    Log_Msg(MSG_ERROR,"%s: Unable to resolve %s - %s\n",__func__, servaddr.c_str(), strerror(errno));
//		last_error << "Unable to resolve " << servaddr << " - " << strerror(errno) << endl;
    close(sd);
    return -1;
  }

  memcpy(&sin.sin_addr.s_addr, host->h_addr, host->h_length);
  sin.sin_family = AF_INET;
  sin.sin_port = htons(portno);

  /*** CONNECT SOCKET TO THE SERVICE DESCRIBED BY sockaddr_in struct ***/
  if (connect(sd, (struct sockaddr *)&sin, sizeof(sin)) < 0){
    Log_Msg(MSG_ERROR, "%s: Unable to connect to %s (%s) - %s\n", __func__, servaddr.c_str(), host->h_name, strerror(errno));
//		last_error << "Unable to connect to " << servaddr << " (" << host->h_name << ") - " << strerror(errno) << endl;
    close(sd);
    return -1;
  }

  return sd;
}

int CD_Tag::CDDB_ReadChar(int sock, char *c){
  int ret, n;
  struct timeval tv;
  fd_set readfds;
  while (true){
    tv.tv_sec = MAX_SOCK_WAIT;
    tv.tv_usec = 0;
    FD_ZERO(&readfds);
    FD_SET(sock, &readfds);
    n = sock+1;
    ret = select(n, &readfds, NULL, NULL, &tv);
    switch (ret){
      case 0:   //timeout
        Log_Msg(MSG_ERROR,"%s: Timeout receiving data from remote host\n", __func__);
        return -2;
        break;
        case -1: //some sort of error happened
          Log_Msg(MSG_ERROR, "%s: Error receiving data from remote host - %s\n",__func__, strerror(errno));
          return -1;
          break;
    }
        //select says socket is ready, but is it true?
    if (FD_ISSET(sock, &readfds))
      break; //now it should be true
  }
  ret = recv(sock,c, 1, 0);
  if (ret == 0) {//peer has closed connection
    Log_Msg(MSG_ERROR,"%s: Connection closed by remote host\n",__func__);
                
  }
  else if( ret < 0){
    Log_Msg(MSG_ERROR, "%s: Error receiving data from remote host - %s\n",__func__, strerror(errno));
  }
  return ret; 
}


int CD_Tag::CDDB_ReadLine(int sock, string *line){
  char c;
  int ret;
  string lline;
  line->clear();
  while (true){
    ret=CDDB_ReadChar(sock, &c);
                //fprintf(stderr,"%s\n",c);
    if (ret < 0){
      return ret;
    }
    if (c == '\r'|| c== '\n'|| ret==0)
      break;
                        
    lline =lline+  c;
  }
  if (c == '\r' && ret > 0)
    ret=CDDB_ReadChar(sock, &c);
  line->append(lline.c_str()) ;
  Log_Msg(MSG_WARNING,"--> %s\n",line->c_str());

  return ret;
}


int CD_Tag::CDDB_SendCmd(int sock, string line){
  Log_Msg(MSG_WARNING,"<-- %s\n", line.c_str());
  int ret, n;
  struct timeval tv;
  fd_set writefds;
  while (true){
    tv.tv_sec = MAX_SOCK_WAIT;
    tv.tv_usec = 0;
    FD_ZERO(&writefds);
    FD_SET(sock, &writefds);
    n = sock+1;
    ret = select(n, NULL, &writefds, NULL, &tv);
    switch (ret){
      case 0:   //timeout
        Log_Msg(MSG_ERROR,"%s: Timeout sending data to remote host\n",__func__);
        return -2;
        break;
        case -1: //some sort of error happened
          Log_Msg(MSG_ERROR, "%s: Error sending data to remote host - %s\n",__func__, strerror(errno));
          return -1;
          break;
    }
        //select says socket is ready, but is it true?
    if (FD_ISSET(sock, &writefds))
      break; //now it should be true
  }
  ret = send(sock,line.c_str(),line.length(),0);
  if (ret == 0) {//peer has closed connection
    Log_Msg(MSG_ERROR,"%s: Connection closed by remote host\n",__func__);
  }
        
  else if( ret < 0){
    Log_Msg(MSG_ERROR, "%s: Error retrieving data from remote host - %s\n",__func__, strerror(errno));
  }
  return ret;
}


int CD_Tag::CDDB_Response_Val(string line){
/*
Server response code (three digit code):

First digit:
  1xx	Informative message
  2xx	Command OK
  3xx	Command OK so far, continue
  4xx	Command OK, but cannot be performed for some specified reasons
  5xx	Command unimplemented, incorrect, or program error

Second digit:
  x0x	Ready for further commands
  x1x	More server-to-client output follows (until terminating marker)
  x2x	More client-to-server input follows (until terminating marker)
  x3x	Connection will close

Third digit:
  xx[0-9]	Command-specific code
*/
  int code = -1;
  int ret = sscanf(line.c_str(), "%d",&code);
  if (ret != 1  || code >599 || code < 100){
    Log_Msg(MSG_ERROR, "%s: Error parsing reply from freedb.freedb.org: no server reply code\n", __func__);
    return -1;
  }
        
  return code;
}


int CD_Tag::QueryCDDB(){
  ResetErr();
  query_entries.clear();
  int ret, sock;
  if (status < STATUS_GOT_IDS){
    Log_Msg(MSG_ERROR, "%s: Run CD_Tag::TagCD first!!!\n", __func__);
//		last_error << "Run CD_Tag::TagCD first!!!" << endl;
    return -1;
  }
  Log_Msg(MSG_WARNING, "%s: Connecting to Freedb.org\n",__func__);
  if((sock = CDDB_Open("freedb.freedb.org", 8880))<= 0){
    Log_Msg(MSG_ERROR,"%s: Unable to connect to freedb.freedb.org!!!\n",__func__);
    return -1;
  }
        //read welcome message
  string line;
  if (CDDB_ReadLine(sock, &line) <= 0){
    Log_Msg(MSG_ERROR,"%s: Unable to read welcome message from freedb.freedb.org!!!\n",__func__);
    close(sock);
    return -1;
  }
  ret= CDDB_Response_Val(line);
  if (ret < 0 || (ret/100) !=2){// server error or malformed message
    Log_Msg(MSG_ERROR,"%s: freedb.freedb.org is not available at this moment in time\n",__func__);
    close(sock);
    return -1;
  }
        //sends hello line
  line.clear();
  line="cddb hello unknown invalidhost " + thisappname +" " +thisappversion +"\n";
  if (CDDB_SendCmd(sock, line)< 1){
    Log_Msg(MSG_ERROR,"%s: Error sending hello line to freedb.freedb.org\n",__func__);
    close(sock);
    return -1;
  }
        
  if (CDDB_ReadLine(sock, &line) <= 0){
    Log_Msg(MSG_ERROR,"%s: Cant' get reply to hello command!!!\n",__func__);
    close(sock);
    return -1;
  }
        
  if (CDDB_Response_Val(line) != 200){
    Log_Msg(MSG_ERROR,"freedb.freedb.org may be unavailable or we're not welcome to connect\n");
    close (sock);
    return -1;
  }
        //we now switch to protocol 6 for UTF-8 support
                
  if (CDDB_SendCmd(sock, "proto 6\n")< 1){
    Log_Msg(MSG_ERROR,"Error while sending protocol command\n");
    close(sock);
    return -1;
  }

  if (CDDB_ReadLine(sock, &line) <= 0){
    Log_Msg(MSG_ERROR,"Cant' get reply to proto command!!!\n");
    close(sock);
    return -1;
  }

        
  if (CDDB_Response_Val(line)/100 != 2){
    Log_Msg(MSG_ERROR,"Error while switching to protocol 6\n");
    close (sock);
    return -1;
  }
        //let's query CDDB
  ostringstream lline;
  lline.clear();
  lline.str("");
  int tt = NumOfTracks();
  lline << "cddb query " <<  " " << cddb_id << " "<< tt;
  for (int i =1 ; i < tt+1; i++){
    lline << " " << GetTrackOffset(i);
  }
  lline << " " << TrackTime(0, NULL, NULL, NULL) << endl;

  if (CDDB_SendCmd(sock, lline.str())< 1){
    Log_Msg(MSG_ERROR,"Error while sending query command\n");
    close(sock);
    return -1;
  }

  if (CDDB_ReadLine(sock, &line) <= 0){
    Log_Msg(MSG_ERROR,"Cant' get reply to query command!!!\n");
    close(sock);
    return -1;
  }

  if ((ret=CDDB_Response_Val(line))/100 != 2){
    Log_Msg(MSG_ERROR,"%s: Error while querying database\n",__func__);
    close (sock);
    return -1;
  }
  bool _error = false;
  int er;
        
  switch (ret) {
    case 200:		//found exact match
      Log_Msg(MSG_WARNING,"%s: Found exact match\n",__func__);
      AddCDDBEntry(line, false);
      break;
                
      case 202:		//no match found
        Log_Msg(MSG_WARNING,"%s: No match found\n",__func__);
        break;
                
        case 210:		//found multiple matches
          Log_Msg(MSG_WARNING,"%s: Found multiple exact matches\n",__func__);		

          while ((er = CDDB_ReadLine(sock, &line) > 0) && line !="."){
            AddCDDBEntry(line, true);	
          }
          if (er < 1) 
            _error = true;
          break;
                        
    case 211:
      Log_Msg(MSG_WARNING,"%s: Found multiple inexact matches\n",__func__);

      while ((er = CDDB_ReadLine(sock, &line) > 0) && line !="."){
        AddCDDBEntry(line, true);	
      }
      if (er < 1) 
        _error = true;
      break;
                
      default: // we shouldn't be here
        Log_Msg(MSG_ERROR,"%s: Received unknown reply code from server\n",__func__);
        _error = true;
  }
        
  if(_error){
    close (sock);
    query_entries.clear();
    return -1;
  }
        //At this point we download disc details for each entry we found
  CDDB_Query_Entry  entry;
  for (list<CDDB_Query_Entry>::iterator iter = query_entries.begin(); iter != query_entries.end();iter++){
    entry = (*iter);
    line = "cddb read " + entry.cddb_entry_genre + " " + entry.cddb_entry_id + "\n" ;

    if (CDDB_SendCmd(sock, line)< 1){
      Log_Msg(MSG_ERROR,"%s: Error while sending read command\n",__func__);
      close(sock);
      return -1;
    }

    if (CDDB_ReadLine(sock, &line) <= 0){
      Log_Msg(MSG_ERROR,"%s: Cant' get reply to read command!!!\n",__func__);
      close(sock);
      return -1;
    }
        
    if ((ret=CDDB_Response_Val(line))/100 != 2){
      Log_Msg(MSG_ERROR,"%s: Error while reading CD details from database\n",__func__);
      close (sock);
      return -1;
    }
    while ((er = CDDB_ReadLine(sock, &line) > 0) && line !="."){
      entry.cddb_entry_data += line + "\n";
    }
    if (er < 1){
      Log_Msg(MSG_ERROR,"%s: Error while reading CD details from database\n",__func__);
      close (sock);
      return -1;
    }
    status = STATUS_GOT_ALL;
    (*iter) = entry;
                
  }
                
  close(sock);
  return query_entries.size();
}

bool CD_Tag::AddCDDBEntry(string line, bool multiple){
        
  char categ[256], discid[32];
  int pos, ret;
  if (!multiple)
    ret=sscanf(line.c_str(), "%*d%256s%32s %n", categ, discid, &pos);
  else
    ret=sscanf(line.c_str(), "%256s %32s %n", categ, discid, &pos);
  if (ret != 2){
    Log_Msg(MSG_WARNING,"%s: Can't parse CDDB entry\n", __func__);
    Log_Msg(MSG_WARNING,"%s: Discarding: %s\n",__func__, line.c_str());
    return false;
  }
                        
  CDDB_Query_Entry entry;
  entry.cddb_entry_genre= categ;
  entry.cddb_entry_id= discid;
  entry.cddb_entry_album=line.substr(pos,line.length()-pos);
  entry.cddb_entry_data.clear();
        //printf ("%s\n",  entry.cddb_entry_album.c_str());
  if (query_entries.size() > max_num_of_entries) {
    Log_Msg(MSG_WARNING,"%s: Maximum number of entries allowed already reached\n",__func__);
    Log_Msg(MSG_WARNING,"%s: Discarding: %s\n", __func__, line.c_str());
  }
  else
    query_entries.push_back(entry);
        
  return true;
}
        
int CD_Tag::NumOfEntries(){
  if (status !=STATUS_GOT_ALL){
    Log_Msg(MSG_ERROR, "%s: Run CD_Tag::QueryCDDB first!!!\n",__func__);
    return -1;
  }
  return query_entries.size();
}
                

bool CD_Tag::GetEntryDataLine(string data,  string field, string *retstring){
  string result, line="";
  char c; 
  field +="=";
  int cc; 
  for (string::const_iterator t = data.begin(); t != data.end(); t++){ 
    c=(*t);
    if (c!= '\n' && c!='\r')
      line +=c;
    else if (line.find(field, 0) ==0){
      cc=field.size();
      line.erase(0, field.size());			
      retstring->assign(line);
      return  true;
                        
    }
    else
      line.clear();
  }
  return false;

}



bool CD_Tag::GetEntryInfo(unsigned int entrynum,  const char *field, string *retstring){
  ResetErr();
  retstring->clear();
  if (status !=STATUS_GOT_ALL){
    Log_Msg(MSG_ERROR, "%s: Run CD_Tag::QueryCDDB first!!!\n", __func__);
    return false;
  }
        
  if (entrynum >= query_entries.size()){
    Log_Msg(MSG_ERROR, "%s: Entrynumber is out of range.\n",__func__);
    return false;
  }
        
  CDDB_Query_Entry entry;
  list<CDDB_Query_Entry>::const_iterator iter = query_entries.begin();
  for (unsigned int tt = 0; tt < entrynum; ++tt){
    ++iter;
  }
  entry =(*iter);
  std::string   line;
  if	(strcmp (field,"album")==0){ 
    string::size_type  pos;
    std::string slash =" / ";
    string::size_type tt = slash.length();
    line =  entry.cddb_entry_album;
    pos = line.find (slash, 0);
    if (pos !=string::npos){
      line.erase(0, pos+tt);
    }
    retstring->assign(line);
    return true; 
  }
        
  else if	(strcmp (field,"genre")==0){
    retstring->assign(entry.cddb_entry_genre);
    return true;
        
  }
        
  else if	(strcmp (field,"cdid")==0){
    retstring->assign(entry.cddb_entry_id);
    return  true;
                
  }

  else if (strcmp (field,"artist")==0){
    string::size_type pos;
    string slash =" / ";
    line =  (*iter).cddb_entry_album;
    pos = line.find (slash, 0);
    if (pos !=string::npos){
      retstring->assign(line.substr(0, pos));
      return true;
    }
    else{
      return false;
//			field="DARTIST";
    }
  }
  else{

/*		str.clear();
    str = "\n"; 
    str +=field ;
    str +="=";
    unsigned int pos1;
    unsigned int pos2;
    line = (*iter).cddb_entry_data;
    pos1 = line.find( str,0);
    if (pos1 == string::npos){
    return NULL;
  }
    pos1+=str.size();
    pos2 = line.find('\n',pos1);
    if (pos2 == string::npos){
    return NULL;
  }
    else
    line.assign((*iter).cddb_entry_data, pos1, pos2 - pos1);

    return line.c_str();
*/
    return GetEntryDataLine((*iter).cddb_entry_data,  field, retstring);
  }
}

bool CD_Tag::SaveEntryData(unsigned int entrynum, const char *filename, bool overwrite){
  ResetErr();
  CDDB_Query_Entry entry;
  if (status !=STATUS_GOT_ALL){
    Log_Msg(MSG_ERROR, "%s: Run CD_Tag::QueryCDDB first!!!\n", __func__);
    return false;
  }

  if (entrynum >= query_entries.size()){
    Log_Msg(MSG_ERROR, "%s: Entrynumber is out of range.\n",__func__);
    return false;
  }
  list<CDDB_Query_Entry>::const_iterator iter = query_entries.begin();
  for (unsigned int tt = 0; tt < entrynum; ++tt){
    ++iter;
  }
  entry =(*iter);
  if (!overwrite){
    ifstream test(filename);
    if (test.is_open()){
      test.close();
      Log_Msg(MSG_ERROR, "%s: File '%s' exists\n",__func__, filename);
      return false;
    }
  }
  ofstream ffile;
  ffile.open(filename);
  if (ffile.fail()){
    Log_Msg(MSG_ERROR, "%s: Unable to save to %s: %s\n", __func__, filename, strerror(errno));
    return false;
  }
  ffile << entry.cddb_entry_data << endl;
  if (ffile.fail()){
    Log_Msg(MSG_ERROR, "%s: Unable to save to %s: %s\n", __func__, filename, strerror(errno));
    ffile.close();
    return false;
  }
  ffile.close();
  return true;
}

bool CD_Tag::LoadEntryData(const char *filename){
  ResetErr();
  if (status < STATUS_GOT_IDS){
    Log_Msg(MSG_ERROR, "%s: Run CD_Tag::TagCD first!!!\n", __func__);
    return false;
  }
  status = STATUS_GOT_IDS;
  CDDB_Query_Entry entry;
  query_entries.clear();
  ifstream ffile;
  ffile.open(filename);
  if (ffile.fail()){
    Log_Msg(MSG_ERROR, "%s: Unable to open file  %s: %s\n", __func__, filename, strerror(errno));
    return false;
  }
  string _str;
  while(!ffile.eof() && !ffile.fail()) {
    std::getline(ffile, _str);
    entry.cddb_entry_data+=_str + '\n';
  }
/*
  if (ffile.fail()){
  Log_Msg(MSG_ERROR, "%s: Unable to read from file  %s: %s\n", __func__, filename, strerror(errno));
  ffile.close();
  return false;
}
  */	ffile.close();
        if (!GetEntryDataLine(entry.cddb_entry_data, "DISCID", &_str))
          return false;
        entry.cddb_entry_id =_str;

        if (!GetEntryDataLine(entry.cddb_entry_data, "DGENRE", &_str))
          return false;
        entry.cddb_entry_genre =_str;

        if (!GetEntryDataLine(entry.cddb_entry_data, "DTITLE", &_str))
          return false;
        entry.cddb_entry_album =_str;
        query_entries.push_back(entry);
        status = STATUS_GOT_ALL;
        return true;
}


bool  CD_Tag::DownloadCDCover(const char* filename){
  ResetErr();
  if (status < STATUS_GOT_IDS){
    Log_Msg(MSG_ERROR, "%s: Run CD_Tag::TagCD first!!!\n",__func__);
    return false;
  }
//	We first download the proper xml entry from Musicbrainz, using the following URL
//	//http://musicbrainz.org/ws/1/release/?type=xml&discid=<Nusicbrainz ID>
//	Then we parse it for <asin>YYYYYY</asin> value
//	then we download the cover art from Amazon with the wollowing URL:
//	http://ec1.images-amazon.com/images/P/<ASID>.01._SCLZZZZZZZ_.jpg
//	wish me good luck :P
  string str1;
  string str2 ="http://musicbrainz.org/ws/1/release/?type=xml&discid=" + mbrainz_id;
  if (!WgetWrapper::download(str2, str1))
    return false; //Error, we exit

  string asin = regex_tools::extract_substr(str1, "<asin>(.*?)</asin>", true); 
  if (!asin.empty()){
    str1 = filename;
    str2 = "http://ec1.images-amazon.com/images/P/" + asin;
    str2+=".01._SCLZZZZZZZ_.jpg";
    WgetWrapper::download_to_file(str2, str1);
    return true;
  }
  Log_Msg(MSG_ERROR,"%s: Unable to download a cover picture for this CD\n",__func__);
  return false;
}


