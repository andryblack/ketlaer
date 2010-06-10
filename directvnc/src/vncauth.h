/*
 *  Copyright (C) 1999 AT&T Laboratories Cambridge.  All Rights Reserved.
 *
 *  This is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, a copy can be downloaded from 
 *  http://www.gnu.org/licenses/gpl.html, or obtained by writing to the
 *  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 *  Boston, MA 02110-1301, USA.
 */

/* 
 * vncauth.h - describes the functions provided by the vncauth library.
 */

#define MAXPWLEN 8
#define CHALLENGESIZE 16

extern int vncEncryptAndStorePasswd(char *passwd, char *fname);
extern char *vncDecryptPasswdFromFile(char *fname);
extern void vncRandomBytes(unsigned char *bytes);
extern void vncEncryptBytes(unsigned char *bytes, char *passwd);
