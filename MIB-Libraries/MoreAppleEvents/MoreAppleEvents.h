/*
	File:		MoreAppleEvents.h

	Contains:	Apple Event Manager utilities.

	DRI:		George Warner

	Copyright:	Copyright (c) 1998-2001 by Apple Computer, Inc., All Rights Reserved.

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

$Log: MoreAppleEvents.h,v $
Revision 1.19  2002/11/25 18:39:27         
Convert OSErr to OSStatus.

Revision 1.18  2002/11/08 22:50:45         
When using framework includes, explicitly include the frameworks we need. Convert nil to NULL. Moved a bunch of stuff to MoreAEDataModel.

Revision 1.17  2002/10/16 20:33:01        
Added MoreAEfprintDesc routine and changed desc parameters to pAEDesc.

Revision 1.16  2002/03/07 20:32:14        
General clean up.
Added recovery code to MoreAESendEventReturnPString.
New API: MoreAECreateAEDescFromCFString.

Revision 1.15  2002/02/19 18:55:16        
Written by: => DRI:

Revision 1.14  2002/01/16 19:11:56        
Added MoreAESendEventReturnAEDesc, MoreAESendEventReturnAEDescList,
MoreAETellSelfToSetCFStringRefProperty,  & MoreAEGetCFStringFromDescriptor routines.

Revision 1.13  2001/11/07 15:50:54         
Tidy up headers, add CVS logs, update copyright.


        <12>     21/9/01    Quinn   Changes for CWPro7 Mach-O build.
        <11>     8/28/01    gaw     CodeBert (error -> pError, theAppleEvent -> pAppleEvent, etc.)
        <10>     15/2/01    Quinn   MoreAECreateAppleEventTargetID is not supported for Carbon
                                    builds because all of its required declarations are defined
                                    CALL_NOT_IN_CARBON.
         <9>     27/3/00    Quinn   Remove MoreAEDeleteItemFromRecord.  It's functionality is
                                    covered by AEDeleteKeyDesc.
         <8>     20/3/00    Quinn   Added routines to deal with "missing value".  Added
                                    MoreAECopyDescriptorDataToHandle.  Fixed some copy and paste
                                    errors in the comments.
         <7>      3/9/00    GW      Intergrating AppleEvent Helper code
         <6>      6/3/00    Quinn   Added a bunch of trivial wrapper routines.  George may come
                                    along and change all these soon, but I needed them for MoreOSL.
         <5>      1/3/00    Quinn   Change the signature for AEGetDescData to match the version we
                                    actually shipped.
         <4>     2/15/99    PCG     add AEGetDescDataSize for non-Carbon clients
         <3>     1/29/99    PCG     add AEGetDescData
         <2>    11/11/98    PCG     fix headers
         <1>    11/10/98    PCG     first big re-org at behest of Quinn

	Old Change History (most recent first):

         <4>    10/11/98    Quinn   Convert "MorePrefix.h" to "MoreSetup.h".
         <3>      9/9/98    PCG     re-work import and export pragmas
         <2>     7/24/98    PCG	    coddle linker (C++, CFM-68K)
         <1>     6/16/98    PCG     initial checkin
*/

#pragma once

//********************************************************************************
//	A private conditionals file to setup the build environment for this project.
#include "MoreSetup.h"

//********************************************************************************
//	System includes
#if MORE_FRAMEWORK_INCLUDES
	#include <Carbon/Carbon.h>
#else
	#include <AERegistry.h>
	#include <AEObjects.h>
	#include <AEPackObject.h>
	#include <EPPC.h>
	#include <Gestalt.h>
	#include <Processes.h>
#endif
//#include <cstdio>
//#include <file_struc.h>     		

//********************************************************************************
#define	kFlagNotSet	-1
//********************************************************************************
#ifdef __cplusplus
	extern "C" {
#endif
/********************************************************************************
	Create and return an AEDesc for the process target with the specified PSN.
	If no PSN is supplied the use the current process

	pAEEventClass	==>	The class of the event to be created.
	pAEEventID		==>	The ID of the event to be created.
	pAppleEvent		==>	Pointer to an AppleEvent where the
						event record will be returned.
					<==	The Apple event.
	
	RESULT CODES
	____________
	noErr			   0	No error	
	memFullErr		-108	Not enough room in heap zone	
	____________
*/
extern pascal OSStatus MoreAECreateProcessTarget (ProcessSerialNumber* pPSN,AEDesc* target);
/********************************************************************************
	Create and return an AppleEvent of the given class and ID. The event will be
	targeted at the current process, with an AEAddressDesc of type
	typeProcessSerialNumber.

	pAEEventClass	==>	The class of the event to be created.
	pAEEventID		==>	The ID of the event to be created.
	pAppleEvent		==>	Pointer to an AppleEvent where the
						event record will be returned.
					<==	The Apple event.
	
	RESULT CODES
	____________
	noErr			   0	No error	
	memFullErr		-108	Not enough room in heap zone	
	____________
*/
extern pascal OSStatus MoreAECreateAppleEventSelfTarget( AEEventClass pAEEventClass,
							  AEEventID pAEEventID,
							  AppleEvent* pAppleEvent );
/********************************************************************************
	Create and return an AppleEvent of the given class and ID. The event will be
	targeted at the process specified by the target type and creator codes, 
	with an AEAddressDesc of type typeProcessSerialNumber.

	pType			==>	The file type of the process to be found.
	pCreator		==>	The creator type of the process to be found.
	pAEEventClass	==>	The class of the event to be created.
	pAEEventID		==>	The ID of the event to be created.
	pAppleEvent		==>	Pointer to an AppleEvent where the
						event record will be returned.
					<==	The Apple event.
	
	RESULT CODES
	____________
	noErr			   0	No error	
	memFullErr		-108	Not enough room in heap zone	
	procNotFound	�600	No eligible process with specified descriptor
	____________
*/
extern pascal OSStatus	MoreAECreateAppleEventSignatureTarget( OSType pType,
											 OSType pCreator,
											 AEEventClass pAEEventClass,
											 AEEventID pAEEventID,
											 AppleEvent* pAppleEvent );
/********************************************************************************
	Create and return an AppleEvent of the given class and ID. The event will be
	targeted at the application with the specific creator.

	psnPtr			==>	Pointer to the PSN to target the event with.
	pAEEventClass	==>	The class of the event to be created.
	pAEEventID		==>	The ID of the event to be created.
	pAppleEvent		==>	Pointer to an AppleEvent where the
						event record will be returned.
					<==	The Apple event.
	
	RESULT CODES
	____________
	noErr			   0	No error	
	memFullErr		-108	Not enough room in heap zone	
	procNotFound	�600	No eligible process with specified descriptor
	____________
*/
extern pascal OSStatus MoreAECreateAppleEventCreatorTarget(
							const AEEventClass pAEEventClass,
							const AEEventID pAEEventID,
							const OSType pCreator,
							AppleEvent* pAppleEvent);
/********************************************************************************
	Create and return an AppleEvent of the given class and ID. The event will be
	targeted with the provided PSN.

	psnPtr			==>	Pointer to the PSN to target the event with.
	pAEEventClass	==>	The class of the event to be created.
	pAEEventID		==>	The ID of the event to be created.
	pAppleEvent		==>	Pointer to an AppleEvent where the
						event record will be returned.
					<==	The Apple event.
	
	RESULT CODES
	____________
	noErr			   0	No error	
	memFullErr		-108	Not enough room in heap zone	
	procNotFound	�600	No eligible process with specified descriptor
	____________
*/
extern pascal OSStatus	MoreAECreateAppleEventProcessTarget( const ProcessSerialNumberPtr psnPtr,
										   AEEventClass pAEEventClass,
										   AEEventID pAEEventID,
										   AppleEvent* pAppleEvent );
/********************************************************************************
	Create and return an AppleEvent of the given class and ID. The event will be
	targeted with the provided TargetID.

	pTargetID		==>	Pointer to the TargetID to target the event with.
	pAEEventClass	==>	The class of the event to be created.
	pAEEventID		==>	The ID of the event to be created.
	pAppleEvent		==>	Pointer to an AppleEvent where the
						event record will be returned.
					<==	The Apple event.
	
	RESULT CODES
	____________
	noErr			   0	No error	
	memFullErr		-108	Not enough room in heap zone	
	procNotFound	�600	No eligible process with specified descriptor
	____________
*/

#if CALL_NOT_IN_CARBON

//********************************************************************************
// Addressing Apple events by target has been deprecated in Carbon, 
// so much so that constants like typeTargetID are not available to 
// Carbon applications.  So we can�t support this routine for Carbon 
// development.

extern pascal OSStatus	MoreAECreateAppleEventTargetID( const TargetID* pTargetID,
									  AEEventClass pAEEventClass,
									  AEEventID pAEEventID,
									  AppleEvent* pAppleEvent );

#endif

/********************************************************************************
	Send the provided AppleEvent using the provided idle function.
	Will wait for a reply if an idle function is provided, but no result will be returned.

	pIdleProcUPP	==>	The idle function to use when sending the event.
	pAppleEvent		==>	The event to be sent.
	
	RESULT CODES
	____________
	noErr			   0	No error	
	
	and any other error that can be returned by AESend or the handler
	in the application that gets the event.
	____________
*/
extern pascal OSStatus	MoreAESendEventNoReturnValue( const AEIdleUPP pIdleProcUPP,
										   const AppleEvent* pAppleEvent );
/********************************************************************************
	Send the provided AppleEvent using the provided idle function.
	Return the direct object as a AEDesc of pAEDescType

	pIdleProcUPP	==>	The idle function to use when sending the event.
	pAppleEvent		==>	The event to be sent.
	pDescType		==>	The type of value returned by the event.
	pAEDescList		<==	The value returned by the event.

	RESULT CODES
	____________
	noErr			   0	No error	
	paramErr		 -50	No idle function provided

	and any other error that can be returned by AESend or the handler
	in the application that gets the event.
	____________
*/
extern pascal OSStatus MoreAESendEventReturnAEDesc(
						const AEIdleUPP		pIdleProcUPP,
						const AppleEvent	*pAppleEvent,
						const DescType		pDescType,
						AEDesc				*pAEDesc);
/********************************************************************************
	Send the provided AppleEvent using the provided idle function.
	Return the direct object as a AEDescList

	pIdleProcUPP	==>	The idle function to use when sending the event.
	pAppleEvent		==>	The event to be sent.
	pAEDescList		<==	The value returned by the event.

	RESULT CODES
	____________
	noErr			   0	No error	
	paramErr		 -50	No idle function provided

	and any other error that can be returned by AESend or the handler
	in the application that gets the event.
	____________
*/
extern pascal OSStatus MoreAESendEventReturnAEDescList(
						const AEIdleUPP		pIdleProcUPP,
						const AppleEvent	*pAppleEvent,
						AEDescList			*pAEDescList);
/********************************************************************************
	Send the provided AppleEvent using the provided idle function.
	Return data (at pDataPtr) of type pDesiredType

	pIdleProcUPP	==>	The idle function to use when sending the event.
	pAppleEvent		==>	The event to be sent.
	theValue		<==	The value returned by the event.

	RESULT CODES
	____________
	noErr			   0	No error	
	paramErr		 -50	No idle function provided

	and any other error that can be returned by AESend or the handler
	in the application that gets the event.
	____________
*/
extern pascal OSStatus MoreAESendEventReturnData(
						const AEIdleUPP		pIdleProcUPP,
						const AppleEvent	*pAppleEvent,
						DescType			pDesiredType,
						DescType*			pActualType,
						void*		 		pDataPtr,
						Size				pMaximumSize,
						Size 				*pActualSize);
/********************************************************************************
	Send the provided AppleEvent using the provided idle function.
	Return a SInt16 (typeSmallInteger).

	pIdleProcUPP	==>	The idle function to use when sending the event.
	pAppleEvent		==>	The event to be sent.
	theValue		<==	The value returned by the event.
	
	RESULT CODES
	____________
	noErr			   0	No error	
	paramErr		 -50	No idle function provided
	
	and any other error that can be returned by AESend or the handler
	in the application that gets the event.
	____________
*/
extern pascal OSStatus	MoreAESendEventReturnSInt16(
					const AEIdleUPP pIdleProcUPP,
					const AppleEvent* pAppleEvent,
					SInt16* theValue );
/********************************************************************************
	Send the provided AppleEvent using the provided idle function.
	Returns a PString.

	pIdleProcUPP	==>	The idle function to use when sending the event.
	pAppleEvent		==>	The event to be sent.
	pStr255			<==	The value returned by the event.

	RESULT CODES
	____________
	noErr			   0	No error	
	paramErr		 -50	No idle function provided

	and any other error that can be returned by AESend or the handler
	in the application that gets the event.
	____________
*/
extern pascal OSStatus MoreAESendEventReturnPString(
						const AEIdleUPP pIdleProcUPP,
						const AppleEvent* pAppleEvent,
						Str255 pStr255);
/********************************************************************************
	Send an AppleEvent of the specified Class & ID to myself using the 
	provided idle function.  The actual target of the event is the
	pSelection property of the application.

	pEventID		==>	The event to be sent.

	RESULT CODES
	____________
	noErr			   0	No error	

	and any other error that can be returned by AESend or the handler
	in the application that gets the event.
	____________
*/
extern pascal OSStatus MoreAESendToSelfNoReturnValue(
				const AEEventClass pEventClass,
				const AEEventID pEventID);
/********************************************************************************
	Send an AppleEvent of the specified Class & ID to myself using the 
	provided idle function. Wait for a reply and extract a SInt16 result.
	The actual target of the event is the pSelection property of the 
	application.

	pEventID		==>	The event to be sent.
	pValue			<==	Where the return SInt16 will be stored.

	RESULT CODES
	____________
	noErr			   0	No error	

	and any other error that can be returned by AESend or the handler
	in the application that gets the event.
	____________
*/
extern pascal OSStatus MoreAESendToSelfReturnSInt16(
				const AEEventClass pEventClass,
				const AEEventID pEventID,
				SInt16* pValue);
/********************************************************************************
	Send a get data (kAEGetData) AppleEvent to myself using the 
	provided idle function. Wait for a reply and extract a SInt16 result.
	
	pEventID		==>	The event to be sent.
	pValue			<==	Where the resulting SInt16 will be stored.

	RESULT CODES
	____________
	noErr			   0	No error	

	and any other error that can be returned by AESend or the handler
	in the application that gets the event.
	____________
*/
extern pascal OSStatus MoreAETellSelfToGetSInt16Property(
				const DescType pPropType,
				SInt16* pValue);
/********************************************************************************
	Send a get data (kAEGetData) AppleEvent to myself using the 
	provided idle function. Wait for a reply and extract a Str255 result.
	The actual target of the event is the pSelection property of the 
	application.

	pEventID		==>	The event to be sent.
	pValue			<==	Where the resulting Str255 will be stored.

	RESULT CODES
	____________
	noErr			   0	No error	

	and any other error that can be returned by AESend or the handler
	in the application that gets the event.
	____________
*/
extern pascal OSStatus MoreAETellSelfToGetStr255Property(
				const DescType pPropType,
				Str255 pValue);
/********************************************************************************
	Send a set data (kAESetData) AppleEvent to myself with a SInt16 parameter
	and using the provided idle function.

	pEventID		==>	The event to be sent.
	pValue			<==	Where the resulting SInt16 will be stored.

	RESULT CODES
	____________
	noErr			   0	No error	

	and any other error that can be returned by AESend or the handler
	in the application that gets the event.
	____________
*/
extern pascal OSStatus MoreAETellSelfToSetSInt16Property(
				const DescType pPropType,
				SInt16 pValue);
/********************************************************************************
	Send a set data (kAESetData) AppleEvent to myself with a Pascal string
	parameter and using the default idle function.

	pEventID		==>	The event to be sent.
	pValue			==>	The Str255 to be sent.

	RESULT CODES
	____________
	noErr			   0	No error	

	and any other error that can be returned by AESend or the handler
	in the application that gets the event.
	____________
*/
extern pascal OSStatus MoreAETellSelfToSetStr255Property(
				const DescType pPropType,
				Str255 pValue);
/********************************************************************************
	Send a set data (kAESetData) AppleEvent to myself with a CFStringRef
	parameter and using the default idle function.

	pEventID		==>	The event to be sent.
	pValue			==>	The CFString to be sent.

	RESULT CODES
	____________
	noErr			   0	No error	

	and any other error that can be returned by AESend or the handler
	in the application that gets the event.
	____________
*/
extern pascal OSStatus MoreAETellSelfToSetCFStringRefProperty(
				const DescType pPropType,
				const CFStringRef pCFStringRef);

//*******************************************************************************
// Appends each of the items in pSourceList to the pDestList.
extern pascal OSStatus MoreAEAppendListToList(const AEDescList* pSourceList,AEDescList* pDestList);

//*******************************************************************************
// This routine takes a result descriptor and an error.
// If there is a result to add to the reply it makes sure the reply isn't
// NULL itself then adds the error to the reply depending on the error
// and the type of result.
extern pascal OSStatus MoreAEMoreAESetReplyErrorNumber (OSErr pOSErr,AppleEvent* pAEReply);
//*******************************************************************************
// This routine takes a result descriptor, a reply descriptor and an error.
// If there is a result to add to the reply it makes sure the reply isn't
// NULL itself then adds the result to the reply depending on the error
// and the type of result.
extern pascal OSStatus MoreAEAddResultToReply(const AEDesc* pResult,AEDesc* pAEReply,const OSErr pError);
// ----------------------------------------------------------------------
//	Name:		MoreAEGotRequiredParams
//	Function:	Checks that all parameters defined as 'required' have been read
// ----------------------------------------------------------------------
extern pascal OSStatus	MoreAEGotRequiredParams(const AppleEvent* pAppleEventPtr);
/********************************************************************************
	Takes a reply event checks it for any errors that may have been returned
	by the event handler. A simple function, in that it only returns the error
	number. You can often also extract an error string and three other error
	parameters from a reply event.
	
	Also see:
		IM:IAC for details about returned error strings.
		AppleScript developer release notes for info on the other error parameters.
	
	pAEReply		==>	The reply event to be checked.

	RESULT CODES
	____________
	noErr				    0	No error	
	????					??	Pretty much any error, depending on what the
								event handler returns for it's errors.
*/
extern pascal OSStatus	MoreAEGetHandlerError( const AppleEvent* pAEReply );
/********************************************************************************
	Get the class and ID from an AppleEvent.

	pAppleEvent		==>	The event to get the class and ID from.
	pAEEventClass	<==	The event's class.
	pAEEventID		<==	The event's ID.
	
	RESULT CODES
	____________
	noErr					    0	No error	
	memFullErr				 -108	Not enough room in heap zone	
	errAEDescNotFound 		-1701	Descriptor record was not found	
	errAENotAEDesc			-1704	Not a valid descriptor record	
	errAEReplyNotArrived	-1718	Reply has not yet arrived	
*/	
extern pascal OSStatus MoreAEExtractClassAndID ( const AppleEvent* pAppleEvent,
							 AEEventClass* pAEEventClass,
							 AEEventID* pAEEventID );
/********************************************************************************
	A very simple idle function. It simply ignors any event it receives,
	returns 30 for the sleep time and NULL for the mouse region.
	
	Your application should supply an idle function that handles any events it
	might receive. This is especially important if your application has any windows.
	
	Also see:
		IM:IAC for details about idle functions.
		Pending Update Perils technote for more about handling low-level events.
*/	
extern pascal Boolean	MoreAESimpleIdleFunction( EventRecord* event,
									   long* sleepTime,
									   RgnHandle* mouseRgn );
/********************************************************************************
	Is the Apple Event Manager present.
	
	RESULT CODES
	____________
	true	The Apple Event Manager is present
	false	It isn't
*/
extern pascal Boolean	MoreAEHasAppleEvents(void);
//*******************************************************************************
// Did this AppleEvent come from the Finder?
extern pascal OSStatus MoreAEIsSenderFinder (const AppleEvent* pAppleEvent,Boolean* pIsFinder);

//*******************************************************************************
// This routine returns true if and only if desc is the "missing value" value.

extern pascal Boolean MoreAEIsMissingValue(const AEDesc* pAEDesc);

//*******************************************************************************
// This routine creates a descriptor that represents the missing value.

extern pascal OSStatus MoreAECreateMissingValue(AEDesc* pAEDesc);

#ifdef __cplusplus
}
#endif
