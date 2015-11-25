/*
	File:		MoreOSA.h

	Contains:	Functions to help you when you are working with the OSA.

	DRI:		George Warner

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

$Log: MoreOSA.h,v $
Revision 1.8  2002/11/08 22:58:54         
Convert nil to NULL. When using framework includes, explicitly include the frameworks we need.

Revision 1.7  2002/02/19 18:58:19        
Written by: => DRI:

Revision 1.6  2001/11/07 15:53:52         
Tidy up headers, add CVS logs, update copyright.


         <5>     21/9/01    Quinn   Get rid of wacky Finder label.
         <4>     21/9/01    Quinn   Changes for CWPro7 Mach-O build.
         <3>      3/9/00    gaw     API changes for MoreAppleEvents
         <2>      3/9/00    gaw     API changes for MoreAppleEvents
*/

#pragma once
//********************************************************************************
//	A private conditionals file to setup the build environment for this project.
#include "MoreSetup.h"
//**********	Universal Headers		****************************************
#if MORE_FRAMEWORK_INCLUDES
	#include <Carbon/Carbon.h>
#else
	#include <AppleScript.h>
	#include <Components.h>
	#include <Files.h>
	#include <OSA.h>
#endif
//**********	Compiler directives		****************************************
#ifdef __cplusplus
	extern "C" {
#endif

#if PRAGMA_IMPORT
	#pragma import on
#endif

#if PRAGMA_STRUCT_ALIGN
	#pragma options align=mac68k
#elif PRAGMA_STRUCT_PACKPUSH
	#pragma pack(push, 2)
#elif PRAGMA_STRUCT_PACK
	#pragma pack(2)
#endif
//********************************************************************************
#define EVENPAD(ptr) ( ( (ptr) + 1 ) & 0xfffffffe )

enum {
	kScriptResID		= 128
};

#define kHandlerNameHeaderSize		8	// The size of everything before name below

struct HandlerNameList {
	DescType		nameType;
	long			nameLength;
	char			name[1];	//	variable length, not terminated
};
typedef struct HandlerNameList HandlerNameList;
typedef HandlerNameList *HandlerNameListPtr;
/********************************************************************************
	Checks for the presence of the AppleScript OSA component.
	
	RESULT CODES
	____________
	true		AppleScript is present and available
	false		It isn't
	____________
*/
extern pascal Boolean MoreOSAAppleScriptIsPresent(void);
/********************************************************************************
	Open the default scripting component.

	componentInstance	input:	Pointer to ComponentInstance variable.
						output:	The newly opened scripting component, or NULL if error.
	
	RESULT CODES
	____________
	noErr			 			0	No error	
	errOSACantOpenComponent	-1762	Can't connect to scripting system with that ID
	____________
*/
extern pascal OSErr MoreOSAOpenGenericScriptingComponent( ComponentInstance *compInstPtr );
/********************************************************************************
	Open the ApppleScript scripting component.

	componentInstance	input:	Pointer to ComponentInstance variable.
						output:	The newly opened scripting component, or NULL if error.
	
	RESULT CODES
	____________
	noErr			 			0	No error	
	errOSACantOpenComponent	-1762	Can't connect to scripting system with that ID
	____________
*/
extern pascal OSErr MoreOSAOpenAppleScriptComponent( ComponentInstance *compInstPtr );
/********************************************************************************
	Load a script resource for the given component instance.
	
	componentInstance	input:	Component instance to load script into.
	pRefNum			input:	Resource file to get script from.
	scriptResID			input:	Script (type 'scpt') resource ID to load.
	scriptIDPtr			input:	Pointer to OSAID to return the loaded script ID in.
						output:	The newly loaded script ID.
	
	RESULT CODES
	____________
	noErr			 				0	No error	
	resNotFound					 -192	Can't find the requested resource
	errOSACorruptData			-1702	Corrupt data	
	errOSASystemError			-1750	General scripting system error	
	errOSABadStorageType		-1752	Script data not for this scripting component
	errOSADataFormatObsolete	-1758	Data format is obsolete	
	errOSADataFormatTooNew		-1759	Data format is too new	
	badComponentInstance	$80008001	Invalid component instance	
	____________
*/
extern pascal OSErr MoreOSALoadScriptResource( ComponentInstance componentInstance,
							  short pRefNum,
							  short scriptResID,
							  OSAID *scriptIDPtr );
/********************************************************************************
	Dispose of a script for the given component instance.  If there have been
	changes to the script, it is saved before being unloaded.
	

	componentInstance	input:	Component instance to load script into.
	pRefNum			input:	Resource file to get script from.
	scriptResID			input:	Script (type 'scpt') resource ID to load.
	scriptIDPtr			input:	OSAID to return the loaded script ID in.
	
	RESULT CODES
	____________
	noErr			 				0	No error	
	memFullErr					 -108	Not enough memory	
	nilHandleErr				 -109	NIL master pointer	
	memWZErr					 -111	Attempt to operate on a free block	
	resNotFound					 -192	Can't find the requested resource
	resAttrErr					 �198	Attribute inconsistent with operation
	errOSASystemError			-1750	General scripting system error	
	errOSAInvalidID				-1751	Invalid script ID	
	errOSABadStorageType		-1752	Desired type not supported by this scripting component   	
	errOSABadSelector			-1754	Selector value not supported by scripting component	
	badComponentInstance	$80008001	Invalid component instance	
	____________
*/
extern pascal OSErr MoreOSAUnloadScriptResource( ComponentInstance componentInstance,
								short pRefNum,
								short scriptResID,
								OSAID scriptID );

/********************************************************************************
	Given an AppleScript script ID, return an list containing the names of all
	handlers inplemented in the script.
	
	NOTE:  Valid only for AppleScript scripts.
	
	Esentially a wrapper around OSAGetHandlerNames() that allows you to pass in either
	a generic or AppleScript component instance.
	
	Returns a list of handler names (typeChar)
	
	The handler name for subroutines, sometimes called named handlers, for should be of 
	typeChar and be in cononical form, i.e., all lower case with diacriticals removed.  
	The name of a handler for a standard event (run, open, or �event xxxxyyyy�) will be 
	8 characters long, and will be of the form clasIDID where 'clas' is the event class 
	and 'IDID' is the eventID.

	
	compInst		input:	A generic or AppleScript component instance.
	scriptID		input:	The script to get the handler names from.
	nameDescPtr		input:	A list to contain the handler names.
	
	RESULT CODES
	____________
	____________
*/
extern pascal OSAError MoreOSAGetHandlerNames( ComponentInstance compInst, OSAID scriptID, AEDescList* handlerListPtr );

/********************************************************************************
	Look for a particular handler name in a list of handler names (as returned by MoreOSAGetHandlerNames()).
	
	The handler name for subroutines, sometimes called named handlers, for should be of 
	typeChar and be in cononical form, i.e., all lower case with diacriticals removed.  
	The name of a handler for a standard event (run, open, or �event xxxxyyyy�) will be 
	8 characters long, and will be of the form clasIDID where 'clas' is the event class 
	and 'IDID' is the eventID.

	NOTE:  Valid only for AppleScript scripts.
	
	handlerListPtr		input:	The list of handler names for an AppleScript script.
	pRefNum			input:	The handler name to check for, in cononical form.
	nameDescPtr			input:	An string containing the name of the handler to look for
	
	RESULT CODES
	____________
	____________
*/
extern pascal Boolean MoreOSAHandlerIsInHandlerList( const AEDescList *handlerListPtr, const AEDesc *nameDescPtr );

/********************************************************************************
	Check a script to see if it contains a particular handler.
	
	The handler name for subroutines, sometimes called named handlers, for should be of 
	typeChar and be in cononical form, i.e., all lower case with diacriticals removed.  
	The name of a handler for a standard event (run, open, or �event xxxxyyyy�) will be 
	8 characters long, and will be of the form clasIDID where 'clas' is the event class 
	and 'IDID' is the eventID

	For subroutines, the input should be of type 'text' with the dataHandle
	containing a lowercased name for AppleScript english scripts
	For other event handlers (open, quite, �event clasIDID�) the input should
	be of type 'evnt' with the data handle containing the event class and ID
	DescTypes, i.e., 8 bytes of the form [clasIDID]
	
	componentInstance	input:	Component instance for script.
	pRefNum			input:	Script to get names from.
	nameDescPtr			input:	A typeChar desc containing the name of the handler to look for
								Note: Must be in cononical form, i.e., 'MyHandler' would be 'myhandler'.
	
	Returns a boolean indicating if the handler was found.
	
	RESULT CODES
	____________
	____________
*/
extern pascal Boolean MoreOSAScriptHasHandler( ComponentInstance compInst, OSAID scriptID, const AEDesc *nameDescPtr );
//********************************************************************************
#if PRAGMA_STRUCT_ALIGN
	#pragma options align=reset
#elif PRAGMA_STRUCT_PACKPUSH
	#pragma pack(pop)
#elif PRAGMA_STRUCT_PACK
	#pragma pack()
#endif

#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif
