#ifndef LYRIC_FETCH_HELPER_HPP
#define LYRIC_FETCH_HELPER_HPP

#include "common-feature.hpp"

#include "tinyxml.h"
#include "tinystr.h"

#include <string>

class LyricFetchHelper 
{
  public:
    std::string artist;
    std::string title;
    std::string str_lyric;

    virtual bool fetch() = 0;

    bool status() {
       return str_lyric.size() > 0;
    }

    std::string get_text() {
       // std::cout << "LyricFetchHelper::get_text() result : " << str_lyric << std::endl;
       return str_lyric;
    }

    LyricFetchHelper(std::string _artist, std::string _title)
    {
      // FIXME: strip extra title (), feat, ft, etc
      artist = string_format::lowercase(string_format::convert(_artist));
      title = string_format::lowercase(string_format::convert(_title));
      str_lyric.clear();
    }

    std::string xml_normalize_newline(std::string text)
    {
        // take from tinyxml.cpp line 973
        long length = text.size();

        // If we have a file, assume it is all one big XML file, and read it in.
	// The document parser may decide the document ends sooner than the entire file, however.
	TIXML_STRING data;
	data.reserve( length );

	// Subtle bug here. TinyXml did use fgets. But from the XML spec:
	// 2.11 End-of-Line Handling
	// <snip>
	// <quote>
	// ...the XML processor MUST behave as if it normalized all line breaks in external 
	// parsed entities (including the document entity) on input, before parsing, by translating 
	// both the two-character sequence #xD #xA and any #xD that is not followed by #xA to 
	// a single #xA character.
	// </quote>
	//
	// It is not clear fgets does that, and certainly isn't clear it works cross platform. 
	// Generally, you expect fgets to translate from the convention of the OS to the c/unix
	// convention, and not work generally.

	/*
	while( fgets( buf, sizeof(buf), file ) )
	{
		data += buf;
	}
	*/

	char* buf = new char[ length+1 ];
        strcpy(buf, text.c_str());
	/* buf[0] = 0;

	if ( fread( buf, length, 1, file ) != 1 ) {
		delete [] buf;
		SetError( TIXML_ERROR_OPENING_FILE, 0, 0, TIXML_ENCODING_UNKNOWN );
		return false;
	} */

	const char* lastPos = buf;
	const char* p = buf;

	buf[length] = 0;
	while( *p ) {
		assert( p < (buf+length) );
		if ( *p == 0xa ) {
			// Newline character. No special rules for this. Append all the characters
			// since the last string, and include the newline.
			data.append( lastPos, (p-lastPos+1) );	// append, include the newline
			++p;									// move past the newline
			lastPos = p;							// and point to the new buffer (may be 0)
			assert( p <= (buf+length) );
		}
		else if ( *p == 0xd ) {
			// Carriage return. Append what we have so far, then
			// handle moving forward in the buffer.
			if ( (p-lastPos) > 0 ) {
				data.append( lastPos, p-lastPos );	// do not add the CR
			}
			data += (char)0xa;						// a proper newline

			if ( *(p+1) == 0xa ) {
				// Carriage return - new line sequence
				p += 2;
				lastPos = p;
				assert( p <= (buf+length) );
			}
			else {
				// it was followed by something else...that is presumably characters again.
				++p;
				lastPos = p;
				assert( p <= (buf+length) );
			}
		}
		else {
			++p;
		}
	}
	// Handle any left over characters.
	if ( p-lastPos ) {
		data.append( lastPos, p-lastPos );
	}		
	delete [] buf;
	buf = 0;

	return data.c_str();
    }
    
};

class LFH_leoslyrics : public LyricFetchHelper
{
  public: 
    LFH_leoslyrics(std::string _artist, std::string _title)
	: LyricFetchHelper(_artist, _title)
    {}

    bool fetch();
    std::string get_hid();
};

class LFH_lyricwiki : public LyricFetchHelper
{
  public: 
    LFH_lyricwiki(std::string _artist, std::string _title)
	: LyricFetchHelper(_artist, _title)
    {}

    bool fetch();
};
#endif
