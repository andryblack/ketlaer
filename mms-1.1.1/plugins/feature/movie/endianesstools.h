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

#ifndef ENDIANESSTOOLS_H_
#define ENDIANESSTOOLS_H_


#include <cstdlib>
//#include <stdlib.h>



bool big_e;


//I hope this is not too slow, I don't want to resort to ASM
static uint16_t invert16(uint16_t x) {
	return (x>>8&0xff)|(x<<8&0xff00); 
}

static uint32_t invert32(uint32_t x){
	union {
		uint32_t z1;
		uint16_t z2[2];
	} a, b;
	a.z1 = x;
	b.z2[0] = invert16(a.z2[1]);
	b.z2[1] = invert16(a.z2[0]);
	return b.z1;
}

static uint64_t invert64(uint64_t x){
	union {
		uint64_t z1;
		uint32_t z2[2];
	} a, b;
	a.z1=x;
	b.z2[0] = invert32(a.z2[1]);
	b.z2[1] = invert32(a.z2[0]);
	return b.z1;
}

static uint16_t straight16(uint16_t x) {return x;}
static uint32_t straight32(uint32_t x) {return x;}
static uint64_t straight64(uint64_t x) {return x;}

uint64_t (*le_val64) (uint64_t)=NULL;
uint32_t (*le_val32) (uint32_t)=NULL;
uint16_t (*le_val16) (uint16_t)=NULL;


uint64_t (*be_val64) (uint64_t)=NULL;
uint32_t (*be_val32) (uint32_t)=NULL;
uint16_t (*be_val16) (uint16_t)=NULL;




void ProbeEndianess(){
	char test1[]="\x01\x02\x03\x04";
	unsigned int test2=*( unsigned int *)&test1;
	big_e=(test2==0x01020304);
	if (big_e){
		be_val64=&straight64;
		be_val32=&straight32;
		be_val16=&straight16;
		
		le_val64=&invert64;
		le_val32=&invert32;
		le_val16=&invert16;
	}
	else {
		le_val64=&straight64;
		le_val32=&straight32;
		le_val16=&straight16;
		
		be_val64=&invert64;
		be_val32=&invert32;
		be_val16=&invert16;
	}

}

//routine to convert a 64bit integer into a double precision number.
//taken from ffmpeg/intfloat_readwrite.c
double int2dbl(uint64_t v){
    if(v+v > 0xFFEULL<<52)
    	return 0.0/0.0;
    return ldexp((long double)((v&((1LL<<52)-1)) + (1LL<<52)) * (v>>63|1), (v>>52&0x7FF)-1075);
}

float int2flt(int32_t v){
	if(v+v > 0xFF000000U)
		return 0.0/0.0;
	return ldexp((float)((v&0x7FFFFF) + (1<<23)) * (v>>31|1), (v>>23&0xFF)-150);
}



#endif /*ENDIANESSTOOLS_H_*/
