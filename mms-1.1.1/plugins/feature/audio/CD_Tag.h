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






#ifndef CD_TAG_H_
#define CD_TAG_H_

using namespace std;
#include <iostream>
#include <string>
#include <sstream>
#include <list>


//status values
#define STATUS_VIRGIN		0
#define STATUS_GOT_IDS		1
#define STATUS_GOT_ALL		2

//Verbosity levels
#define VERBOSE_NO			0//Nothing is printed to terminal
#define VERBOSE_ERRS			1//Only errors are printed to terminal
#define VERBOSE_ALL			2//Errors and warnings are printed to terminal 

//Message types for Log_Msg()
#define MSG_ERROR			1
#define MSG_WARNING			2


//timeout value (in seconds) for socket operations
#define MAX_SOCK_WAIT		5
struct CDDB_Query_Entry{

		string			cddb_entry_id;
		string 			cddb_entry_genre;
		string			cddb_entry_album;
		string			cddb_entry_data;



};//CDDB_Query_Entry

#define MAX_NUM_OF_ENTRIES	5


class CD_Tag
{
	private:
	list<CDDB_Query_Entry> query_entries;
	string cddb_id;
	string mbrainz_id;
	unsigned int first_track_num;
	unsigned int last_track_num;
	unsigned int track_off[100];
	string album;
	string artist;
	string genre;
	list<string> titles;
	ostringstream last_error;
	int status;
	int cddb_sum(unsigned int n);
	void ResetErr();
	int CDDB_Open(string servaddr, int portno);
	int CDDB_ReadChar(int sock, char *c);
	int CDDB_ReadLine(int sock, string *line);
	int CDDB_SendCmd(int sock, string line);
	int CDDB_Response_Val(string line);
	bool AddCDDBEntry(string line, bool multiple);
	bool GetEntryDataLine(string data, string field, string *retstring);
public:
	CD_Tag();
	virtual ~CD_Tag();
	unsigned int max_num_of_entries;
	int verbosity;
	string thisappname;
	string thisappversion;

/*
 * Public APIs
 */ 
	int TagCD(const char *cd_path);
	string  CDDB_Id();
	string  MBrainz_ID();
	int NumOfTracks();
	int FirstTrack();
	int LastTrack();
	int TrackTime(unsigned int track_num, int *hours,int *mins, int *secs);
	int GetTrackOffset(unsigned int track_num);
	void ResetAll(unsigned int level);
	unsigned int GetStatus();
	int Log_Msg(int msgtype, const char *fmt, ...);
	const char * GetLastErr();
	int QueryCDDB();
	int NumOfEntries();
	bool GetEntryInfo(unsigned int entrynum, const char *field, string *retstring);
	bool SaveEntryData(unsigned int entrynum, const char *filename, bool overwrite); //you may decide to save the 
											 //CDDB data for xine to read it
											 
	bool LoadEntryData(const char *filename);					 // is this needed?
	bool DownloadCDCover(const char *filename);

	
};

#endif /*CD_TAG_H_*/
