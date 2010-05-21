// Copyright (C) 2004-2005 Open Source Telecom Corporation.
//  
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU General Public License.  This exception does not however    
// invalidate any other reasons why the executable file might be covered by
// the GNU General Public License.    
//
// This exception applies only to the code released under the name GNU
// Common C++.  If you copy code from other releases into a copy of GNU
// Common C++, as the General Public License permits, the exception does
// not apply to the code that you add in this way.  To avoid misleading
// anyone as to the status of such modified files, you must delete
// this exception notice from them.
//
// If you write modifications of your own for GNU Common C++, it is your choice
// whether to permit this exception to apply to your modifications.
// If you do not wish that, delete this exception notice.
//

#include <cc++/config.h>
#include <cc++/export.h>
#include <cc++/thread.h>
#include <cc++/object.h>
#include "private.h"

#ifdef	CCXX_NAMESPACES
namespace ost {
#endif

LinkedSingle::~LinkedSingle() {};

LinkedSingle *LinkedSingle::getFirst(void)
{
	return this;
}

LinkedSingle *LinkedSingle::getLast(void)
{
	LinkedSingle *obj = this;

	while(obj->nextObject)
		obj = obj->nextObject;

	return obj;
}

void LinkedSingle::insert(LinkedSingle& obj)
{
	nextObject = obj.nextObject;
	obj.nextObject = this;
}

LinkedSingle &LinkedSingle::operator+=(LinkedSingle &obj)
{
	insert(obj);
	return *this;
}	

LinkedDouble::~LinkedDouble()
{
	detach();
}

void LinkedDouble::enterLock() {};

void LinkedDouble::leaveLock() {};

LinkedDouble *LinkedDouble::getFirst(void)
{
	LinkedDouble *node = this;

	while(node->prevObject)
		node = node->prevObject;

	return node;
}

LinkedDouble *LinkedDouble::getLast(void)
{
        LinkedDouble *node = this;

        while(node->nextObject)
                node = node->nextObject;

        return node;
}

LinkedDouble *LinkedDouble::getInsert(void)
{
	return getLast();
}

void LinkedDouble::insert(LinkedDouble &obj)
{
	LinkedDouble *node;
	enterLock();
	obj.detach();

	node = getLast();
	obj.nextObject = node->nextObject;
	obj.prevObject = node;
	node->nextObject = &obj;
	if(obj.nextObject)
		obj.nextObject->prevObject = &obj;
	leaveLock();
}

void LinkedDouble::detach(void)
{
	enterLock();

	if(prevObject)
		prevObject->nextObject = nextObject;

	if(nextObject)
		nextObject->prevObject = prevObject;

	leaveLock();
}

#ifdef	CCXX_NAMESPACES
}
#endif		
	
/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 8
 * End:
 */
