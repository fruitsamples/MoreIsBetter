/*
	File:		MoreNavigation.c

	Contains:	Basic Navigation services utilities functionality.

	Written by:	Quinn

	Copyright:	Copyright (c) 2000-2001 by Apple Computer, Inc., All Rights Reserved.

	Disclaimer:	IMPORTANT:  This Apple software is supplied to you by Apple Computer, Inc.
				("Apple") in consideration of your agreement to the following terms, and your
				use, installation, modification or redistribution of this Apple software
				constitutes acceptance of these terms.  If you do not agree with these terms,
				please do not use, install, modify or redistribute this Apple software.

				In consideration of your agreement to abide by the following terms, and subject
				to these terms, Apple grants you a personal, non-exclusive license, under Apple�s
				copyrights in this original Apple software (the "Apple Software"), to use,
				reproduce, modify and redistribute the Apple Software, with or without
				modifications, in source and/or binary forms; provided that if you redistribute
				the Apple Software in its entirety and without modifications, you must retain
				this notice and the following text and disclaimers in all such redistributions of
				the Apple Software.  Neither the name, trademarks, service marks or logos of
				Apple Computer, Inc. may be used to endorse or promote products derived from the
				Apple Software without specific prior written permission from Apple.  Except as
				expressly stated in this notice, no other rights or licenses, express or implied,
				are granted by Apple herein, including but not limited to any patent rights that
				may be infringed by your derivative works or by other works in which the Apple
				Software may be incorporated.

				The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
				WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
				WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
				PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN
				COMBINATION WITH YOUR PRODUCTS.

				IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
				CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
				GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
				ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR DISTRIBUTION
				OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT
				(INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN
				ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Change History (most recent first):

$Log: MoreNavigation.c,v $
Revision 1.4  2002/11/08 23:38:58         
Convert nil to NULL. Convert MoreAssertQ to assert.

Revision 1.3  2001/11/07 15:53:26         
Tidy up headers, add CVS logs, update copyright.


         <2>     21/9/01    Quinn   Changes for CWPro7 Mach-O build.
         <1>     20/3/00    Quinn   First checked in.
*/

/////////////////////////////////////////////////////////////////

// Our Prototypes

#include "MoreNavigation.h"

// Mac OS Interfaces

#if ! MORE_FRAMEWORK_INCLUDES
	#include <Resources.h>
#endif

// MIB Prototypes

/////////////////////////////////////////////////////////////////

extern pascal OSStatus MoreNavExtractSingleReply(const NavReplyRecord *reply, FSSpec *target)
	// See comment in header.
{
	OSStatus err;
	#if MORE_DEBUG
		SInt32 selectionCount;
	#endif
	AEKeyword junkKeyword;
	DescType  junkType;
	Size      junkSize;

	assert((AECountItems(&reply->selection, &selectionCount) == noErr) && (selectionCount == 1));
	err = AEGetNthPtr(&reply->selection, 1, typeFSS, &junkKeyword, &junkType, target, sizeof(*target), &junkSize);
	if (err == noErr) {
		assert(junkType == typeFSS && junkSize == sizeof(FSSpec));
		if (target->name[0] == 0) {
			err = FSMakeFSSpec(target->vRefNum, target->parID, target->name, target);	// Guard against potential Nav braindeadedness.
		}
	}
	return err;
}
