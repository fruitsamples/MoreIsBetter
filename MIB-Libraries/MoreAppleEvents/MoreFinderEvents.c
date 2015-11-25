/*
	File:		MoreFinderEvents.c

	Contains:	Functions to help you build and sending Apple events to the Finder.

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

$Log: MoreFinderEvents.c,v $
Revision 1.21  2002/11/08 22:58:27         
Convert nil to NULL. Include our header early to flush out any missing dependencies. Use MoreAEDataModel. Conditionalise CFString routines for Carbon only.

Revision 1.20  2002/10/16 20:35:37        
"	File:		MoreFinderEventsPlus.h" should be <*.c>.
Added MoreFEGetObjCommentCFString API.

Revision 1.19  2002/10/02 23:54:22        
Cleanup of "Always dispose" comments (inline instead of end-of-line).

Converted to use AEBuild API's.

MoreFEOpenFile, MoreFEOpenInfoWindow, MoreFEDuplicate, MoreFEMove now use object specifiers correctly.

Added MoreFEPrintFile, MoreFEGetWindows, MoreFEGetObjectProperty, MoreFERestart & MoreFEActivate routines.

Revision 1.18  2002/03/18 20:47:38        
Modified MoreFEGetAliasAsObject to support volumes.
Added MoreFEMakeObjectsVisible API.

Revision 1.17  2002/03/13 19:05:16        
More cleanup.
Modified MoreFEGetAliasAsObject to support isDirectory.

Revision 1.16  2002/03/08 23:53:28        
More cleanup.
Added MoreFEGetKindCFString, MoreFEGet/SetCommentCFString

Revision 1.15  2002/03/07 20:35:54        
General clean up.
Added AEBuild version of MoreFEGet/SetComment, MoreFEGetAliasAsObject.
Added (AEBuild only) API's: MoreFEDuplicate, MoreFEMove and MoreFEEmptyTrash.

Revision 1.14  2002/02/19 18:57:37        
Written by: => DRI:
AEDisposeDesc => MoreAEDisposeDesc

Revision 1.13  2002/01/30 23:44:12        
Fixed MFEOpen kAECoreSuite => kCoreEventClass
Added AEHelper (AEBuildAppleEvent) version.

Revision 1.12  2002/01/16 19:19:01        
err => anErr, (anErr ?= noErr) => (noErr ?= anErr)
Added MoreFEGetSelection, MoreFEOpenInfoWindow. MoreFESetComment,
MoreFEGetObjectAsAlias,  MoreFEGetAliasAsObject, MoreFEGetFSSpecAsObject routines.

Revision 1.11  2001/11/07 15:52:37         
Tidy up headers, add CVS logs, update copyright.


        <10>     24/9/01    Quinn   Fixed a cast that would break dataHandle opacity on the Carbon
                                    build.
         <9>     21/9/01    Quinn   Get rid of wacky Finder label.
         <8>     21/9/01    Quinn   Changes for CWPro7 Mach-O build.
         <7>     9/13/01    gaw     Change NewIconActionProc to NewIconActionUPP
         <6>     8/28/01    gaw     Added MoreFEGetKind and MoreFEGetComment

         <5>     8/28/01    gaw     Added MoreFEGetKind and MoreFEGetComment

         <5>     15/2/01    Quinn   Minor tweaks to get it building under Project Builder.
         <4>     4/26/00    gaw     Added MoreFEGetKind
         <3>      3/9/00    gaw     Fix AEDesc.dataHandle references
         <2>      3/9/00    gaw     API changes for MoreAppleEvents
*/

//*******************************************************************************
//**********	Our Prototypes			****************************************
#include "MoreFinderEvents.h"
//**********	Universal Headers		****************************************
#if !MORE_FRAMEWORK_INCLUDES
	#include <Aliases.h>
	#include <AEDataModel.h>
	#include <AEHelpers.h>
	#include <ASRegistry.h>
	#include <FinderRegistry.h>
	#include <Folders.h>
	#include <Icons.h>
	#include <OSA.h>
	#include <TextUtils.h>
	#include <stdio.h>
#endif
//**********	Project Headers			****************************************
#include "MoreAppleEvents.h"
#include "MoreAEObjects.h"
#include "MoreAEDataModel.h"
#include "MoreProcesses.h"


//********************************************************************************
static const OSType gFinderSignature = kFinderCreatorType;
/********************************************************************************
	Send an Apple event to the Finder to get file kind of the item 
	specified by the FSSpecPtr.

	pFSSpecPtr		==>		The item to get the file kind of.
	pKindStr		==>		A string into which the file kind will be returned.
	pKindStr		<==		the file kind of the FSSpec.
	pIdleProcUPP	==>		A UPP for an idle function (required)
	
	See note about idle functions above.
*/
pascal OSErr MoreFEGetKind(const FSSpecPtr pFSSpecPtr,Str255 pKindStr,const AEIdleUPP pIdleProcUPP)
{
	AppleEvent tAppleEvent = {typeNull,NULL};	//	If you always init AEDescs, it's always safe to dispose of them.
	OSErr anErr = paramErr;

	if (NULL != pIdleProcUPP)	// the idle proc is required
	{
		AEDesc 		tAEDesc = {typeNull,NULL};
		
		anErr = MoreAEOCreateObjSpecifierFromFSSpec(pFSSpecPtr,&tAEDesc);
		if (noErr == anErr)
		{
			AEBuildError	tAEBuildError;

			anErr = AEBuildAppleEvent(
				        kAECoreSuite,kAEGetData,
						typeApplSignature,&gFinderSignature,sizeof(OSType),
				        kAutoGenerateReturnID,kAnyTransactionID,
				        &tAppleEvent,&tAEBuildError,
				        "'----':obj {form:prop,want:type(prop),seld:type(kind),from:(@)}",&tAEDesc);

	        // always dispose of AEDescs when you are finished with them
	        (void) MoreAEDisposeDesc(&tAEDesc);

			if (noErr == anErr)
			{	
				//	Send the event.
					anErr = MoreAESendEventReturnPString(pIdleProcUPP,&tAppleEvent,pKindStr);
				// always dispose of AEDescs when you are finished with them
				(void) MoreAEDisposeDesc(&tAppleEvent);
			}
		}
		// always dispose of AEDescs when you are finished with them
		(void) MoreAEDisposeDesc(&tAppleEvent);
	}
	return anErr;
}	// end MoreFEGetKind

/********************************************************************************
	Send an Apple event to the Finder to get file kind of the item 
	specified by the FSRefPtr.

	pFSRefPtr		==>		The item to get the file kind of.
	pKindStr		==>		A string into which the file kind will be returned.
	pKindStr		<==		the file kind of the FSRef.
	pIdleProcUPP	==>		A UPP for an idle function (required)
	
	See note about idle functions above.
*/
#if TARGET_API_MAC_CARBON
pascal OSErr MoreFEGetKindCFString(const FSRefPtr pFSRefPtr,CFStringRef* pKindStr,const AEIdleUPP pIdleProcUPP)
{
	AppleEvent tAppleEvent = {typeNull,NULL};	//	If you always init AEDescs, it's always safe to dispose of them.
	OSErr anErr = noErr;

	if (NULL != pIdleProcUPP)	// the idle proc is required
	{
		AEDesc 		tAEDesc = {typeNull,NULL};
		
		anErr = MoreAEOCreateObjSpecifierFromFSRef(pFSRefPtr,&tAEDesc);
		if (noErr == anErr)
		{
			AEBuildError	tAEBuildError;

			anErr = AEBuildAppleEvent(
				        kAECoreSuite,kAEGetData,
						typeApplSignature,&gFinderSignature,sizeof(OSType),
				        kAutoGenerateReturnID,kAnyTransactionID,
				        &tAppleEvent,&tAEBuildError,
				        "'----':obj {form:prop,want:type(prop),seld:type(kind),from:(@)}",&tAEDesc);

			// always dispose of AEDescs when you are finished with them
			(void) MoreAEDisposeDesc(&tAppleEvent);

			if (noErr == anErr)
			{	
				//	Send the event.
				anErr = MoreAESendEventReturnAEDesc(pIdleProcUPP,&tAppleEvent,typeUnicodeText,&tAEDesc);
				// always dispose of AEDescs when you are finished with them
				(void) MoreAEDisposeDesc(&tAppleEvent);
				if (noErr == anErr)
				{
					anErr = MoreAEGetCFStringFromDescriptor(&tAEDesc,pKindStr);
				// always dispose of AEDescs when you are finished with them
				(void) MoreAEDisposeDesc(&tAppleEvent);
				}
			}
		}
				// always dispose of AEDescs when you are finished with them
				(void) MoreAEDisposeDesc(&tAppleEvent);
	}
	return anErr;
}	// end MoreFEGetKindCFString
#endif

/********************************************************************************
	Sets the Finder's selection to nothing by telling it to set it's 
	selection to an empty list.

	pIdleProcUPP 	==>		NULL for default, or UPP for the idle function to use
	
	Requires that the folder's window be open, otherwise an error is returned
	in the reply event.
	
	See note about idle functions above.
*/

pascal OSErr MoreFESetSelectionToNone(const AEIdleUPP pIdleProcUPP)
{
	OSErr		anErr = noErr;
	
	AppleEvent	tAppleEvent = {typeNull,NULL};	//	If you always init AEDescs, it's always safe to dispose of them.
	
	anErr = MoreAECreateAppleEventSignatureTarget(kFinderFileType,kFinderCreatorType,
											  kAECoreSuite,kAESetData,&tAppleEvent);
	if (noErr == anErr)
	{
		AEDesc		containerObj = {typeNull,NULL};	// start with the null (application) container
		AEDesc 		propertyObject = {typeNull,NULL};
			
		anErr = MoreAEOCreatePropertyObject(pSelection,&containerObj,&propertyObject);
		if (noErr == anErr)
		{
			anErr = AEPutParamDesc(&tAppleEvent,keyDirectObject,&propertyObject);
			MoreAEDisposeDesc(&propertyObject);	//	Always dispose of objects as soon as you are done (helps avoid leaks)
			if (noErr == anErr)
			{
				AEDescList		emptyList = {typeNull,NULL};
				
				anErr = AECreateList(NULL,0,false,&emptyList);
				if (noErr == anErr)
				{
					anErr = AEPutParamDesc(&tAppleEvent,keyAEData,&emptyList);
					if (noErr == anErr)
					{
						//	Send the event. In this case we don't care about the reply
						anErr = MoreAESendEventNoReturnValue(pIdleProcUPP,&tAppleEvent);
					}
				}
			}
		}
		// always dispose of AEDescs when you are finished with them
		(void) MoreAEDisposeDesc(&tAppleEvent);
	}
	return anErr;
}	// end MoreFESetSelectionToNone

/********************************************************************************
	Send an Apple event to the Finder to get a list of Finder-style object
	for each currently selected object.
	
	pIdleProcUPP	==>		A UPP for an idle function (required)
	pObjectList		==>		A null AEDesc.
					<==		A list containing object descriptors,
							or a null AEDesc if an error is encountered.
	
	See note about idle functions above.

 Apple event record at: 088E7594
  Class: core  ID: getd  Target Type: psn   "Finder"
  
  KEY  TYPE LENGTH  DATA
  ---- obj  000044  
       KEY  TYPE LENGTH  DATA
       form enum 000004  prop
       want type 000004  prop
       seld type 000004  sele
       from null 000000  Null object
  
*/

pascal OSErr MoreFEGetSelection(const AEIdleUPP pIdleProcUPP,AEDescList* pObjectList)
{
	AppleEvent	tAppleEvent = {typeNull,NULL};
	AEBuildError	tAEBuildError;
	OSErr		anErr = noErr;

	if (NULL == pIdleProcUPP)	// the idle proc is required
		return paramErr;

	if (NULL == pObjectList)
		return paramErr;
	
	MoreAENullDesc(pObjectList);

	anErr = AEBuildAppleEvent(
		        kAECoreSuite,kAEGetData,
				typeApplSignature,&gFinderSignature,sizeof(OSType),
		        kAutoGenerateReturnID,kAnyTransactionID,
		        &tAppleEvent,&tAEBuildError,
		        "'----':obj {want:type('prop'),from:'null'(),form:'prop',seld:type('sele')}");
	if (noErr == anErr)
	{
#if 1	// Set this true to printf the Apple Event before you send it.
		Handle strHdl;
		anErr = AEPrintDescToHandle(&tAppleEvent,&strHdl);
		if (noErr == anErr)
		{
			char	nul	= '\0';
			PtrAndHand(&nul,strHdl,1);
			printf("\n-MoreFEGetSelection: tAppleEvent=\"%s\".",*strHdl);
			fflush(stdout);
			DisposeHandle(strHdl);
		}
#endif
		//	Send the event.
					anErr = MoreAESendEventReturnAEDescList(pIdleProcUPP,&tAppleEvent,pObjectList);
		// always dispose of AEDescs when you are finished with them
		(void) MoreAEDisposeDesc(&tAppleEvent);
	}
	return anErr;
}	// end MoreFEGetSelection

/********************************************************************************
	Send an Apple event to Finder 7.5 or later to set the view of the window
	for the folder pointed to by the FSSpec.

	pFSSpecPtr		==>		FSSpec for the folder whose view is to be set
	pViewStyle		==>		A view constant, as defined in AERegistry.h
	pIdleProcUPP 	==>		NULL for default, or UPP for the idle function to use
	
	Requires that the folder's window be open, otherwise an error is returned
	in the reply event.
	
	See note about idle functions above.
*/
pascal OSErr MoreFEChangeFolderViewNewSuite(const FSSpecPtr pFSSpecPtr,
										  const long pViewStyle,
										  const AEIdleUPP pIdleProcUPP)
{
	OSErr		anErr = noErr;
	
	AppleEvent	tAppleEvent = {typeNull,NULL};
	
	anErr = MoreAECreateAppleEventSignatureTarget(kFinderFileType,kFinderCreatorType,
											  kAECoreSuite,kAESetData,&tAppleEvent);
	if (noErr == anErr)
	{
		AEDesc 		folderObject = {typeNull,NULL};
		
		anErr = MoreAEOCreateObjSpecifierFromFSSpec(pFSSpecPtr,&folderObject);
		if (noErr == anErr)
		{
			AEDesc 		propertyObject = {typeNull,NULL};
			
			anErr = MoreAEOCreatePropertyObject(pView,&folderObject,&propertyObject);
			MoreAEDisposeDesc(&folderObject);
			if (noErr == anErr)
			{
				anErr = AEPutParamDesc(&tAppleEvent,keyDirectObject,&propertyObject);
				MoreAEDisposeDesc(&propertyObject);
				if (noErr == anErr)
				{
					anErr = AEPutParamPtr(&tAppleEvent,keyAEData,
										   typeLongInteger,&pViewStyle,sizeof(pViewStyle));
					if (noErr == anErr)
					{
						//	Send the event. In this case we don't care about the reply
						anErr = MoreAESendEventNoReturnValue(pIdleProcUPP,&tAppleEvent);
					}
				}
			}
		}
		// always dispose of AEDescs when you are finished with them
		(void) MoreAEDisposeDesc(&tAppleEvent);
	}
	return anErr;
}	// end MoreFEChangeFolderViewNewSuite

/********************************************************************************
	Send an Apple event to Finder 7.1.1 or 7.1.2 to set the view of the window
	for the folder pointed to by the FSSpec.
	
	pFSSpecPtr		==>		FSSpec for the folder whose view is to be set
	pViewStyle		==>		A view constant, as defined in AERegistry.h
	pIdleProcUPP 	==>		NULL for default, or UPP for the idle function to use
	
	Requires that the folder's window be open, otherwise an error is returned
	in the reply event.
	
	See note about idle functions above.
*/
pascal OSErr MoreFEChangeFolderViewOldSuite(const FSSpecPtr pFSSpecPtr,
										  const long pViewStyle,
										  const AEIdleUPP pIdleProcUPP)
{
	OSErr		anErr = noErr;
	
	AppleEvent	tAppleEvent = {typeNull,NULL};
	
	anErr = MoreAECreateAppleEventSignatureTarget(kFinderFileType,kFinderCreatorType,
											  kAEFinderEvents,kAEChangeView,&tAppleEvent);
	if (noErr == anErr)
	{
		anErr = MoreAEOAddAliasParameterFromFSSpec(pFSSpecPtr,keyDirectObject,&tAppleEvent);
		if (noErr == anErr)
		{
			anErr = AEPutParamPtr(&tAppleEvent,keyMiscellaneous,
								   typeLongInteger,&pViewStyle,sizeof(pViewStyle));
			if (noErr == anErr)
			{
				//	Send the event. In this case we don't care about the reply
				anErr = MoreAESendEventNoReturnValue(pIdleProcUPP,&tAppleEvent);
			}
		}
		// always dispose of AEDescs when you are finished with them
		(void) MoreAEDisposeDesc(&tAppleEvent);
	}
	return anErr;
}	// end MoreFEChangeFolderViewOldSuite

/********************************************************************************
	Send an Apple event to the Finder (any version) to set the view of the window
	for the folder pointed to by the FSSpec.
	
	pFSSpecPtr		==>		FSSpec for the folder whose view is to be set
	pViewStyle		==>		A view constant, as defined in AERegistry.h
	pIdleProcUPP 	==>		NULL for default, or UPP for the idle function to use
	
	Requires that the folder's window be open, otherwise an error is returned
	in the reply event.
	
	See note about idle functions above.
*/
pascal OSErr MoreFEChangeFolderView(const FSSpecPtr pFSSpecPtr,
								  const long pViewStyle,
								  const AEIdleUPP pIdleProcUPP)
{
	OSErr	anErr = noErr;
	
	if (MoreFECallsAEProcess())
	{
		if (MoreFEIsOSLCompliant())
		{
			anErr = MoreFEChangeFolderViewNewSuite(pFSSpecPtr,pViewStyle,pIdleProcUPP);
		}
		else
		{
			anErr = MoreFEChangeFolderViewOldSuite(pFSSpecPtr,pViewStyle,pIdleProcUPP);
		}
	}
	else
	{
		anErr = errAEEventNotHandled;
	}
	
	return anErr;
}	// end MoreFEChangeFolderView

/********************************************************************************
	Send an Apple event to the Finder to add a custom icon to the item 
	specified by the fssPtr.
	
	pFSSpecPtr		==>		The item to add the custom icon to.
	pIconSuiteHdl	==>		A handle to the icon suite to install.
	pIconSelector	==>		An IconSelectorValue specifying which icon types
							to add (defined in Icons.h).
	pIdleProcUPP	==>		A UPP for an idle function, or NULL.
	
	See note about idle functions above.
*/
pascal OSErr MoreFEAddCustomIconToItem(const FSSpecPtr pFSSpecPtr,
									 const Handle pIconSuiteHdl,
									 const IconSelectorValue pIconSelector,
									 const AEIdleUPP pIdleProcUPP)
{
	OSErr		anErr = noErr;
	
	AppleEvent	tAppleEvent = {typeNull,NULL};
	
	anErr = MoreAECreateAppleEventSignatureTarget(kFinderFileType,kFinderCreatorType,
											  kAECoreSuite,kAESetData,&tAppleEvent);
	if (noErr == anErr)
	{
		AEDesc 		itemObject = {typeNull,NULL};
		
		anErr = MoreAEOCreateObjSpecifierFromFSSpec(pFSSpecPtr,&itemObject);
		if (noErr == anErr)
		{
			AEDesc 		propertyObject = {typeNull,NULL};
			
			anErr = MoreAEOCreatePropertyObject(pIconBitmap,&itemObject,&propertyObject);
			MoreAEDisposeDesc(&itemObject);
			if (noErr == anErr)
			{
				anErr = AEPutParamDesc(&tAppleEvent,keyDirectObject,&propertyObject);
				MoreAEDisposeDesc(&propertyObject);
				if (noErr == anErr)
				{
					AEDescList	iconFamilyRec = {typeNull,NULL};
					
					anErr = MoreFECreateIconFamilyRecord(pIconSuiteHdl,pIconSelector,&iconFamilyRec);
					if (noErr == anErr)
					{
						anErr = AEPutParamDesc(&tAppleEvent,keyAEData,&iconFamilyRec);
						if (noErr == anErr)
						{
							//	Send the event. In this case we don't care about the reply
							anErr = MoreAESendEventNoReturnValue(pIdleProcUPP,&tAppleEvent);
						}
					}
				}
			}
		}
		// always dispose of AEDescs when you are finished with them
		(void) MoreAEDisposeDesc(&tAppleEvent);
	}
	return anErr;
}	// end MoreFEAddCustomIconToItem

/********************************************************************************
	Send an Apple event to the Finder to get the icon of the item 
	specified by the fssPtr.

	pFSSpecPtr		==>		The item to get the icon from.
	pIdleProcUPP	==>		A UPP for an idle function (required)
	pIconSuiteHdl	==>		A handle into which the icon suite will be returned.
	pIconSuiteHdl	<==		A handle to an icon suite.
	
	See note about idle functions above.
*/
pascal OSErr MoreFEGetItemIconSuite(const FSSpecPtr pFSSpecPtr,
								  const AEIdleUPP pIdleProcUPP,
										Handle	 *pIconSuiteHdl)
{
	OSErr	anErr = noErr;
	
	AppleEvent	tAppleEvent = {typeNull,NULL};
	
	if (NULL == pIdleProcUPP)	// the idle proc is required
		return paramErr;

	if (NULL == pIconSuiteHdl)	// the icon suite handle is required
		return paramErr;

	anErr = MoreAECreateAppleEventSignatureTarget(kFinderFileType,kFinderCreatorType,
											  kAECoreSuite,kAEGetData,&tAppleEvent);
	if (noErr == anErr)
	{
		AEDesc 		itemObject = {typeNull,NULL};
		
		anErr = MoreAEOCreateObjSpecifierFromFSSpec(pFSSpecPtr,&itemObject);
		if (noErr == anErr)
		{
			AEDesc 		propertyObject = {typeNull,NULL};
			
			anErr = MoreAEOCreatePropertyObject(pIconBitmap,&itemObject,&propertyObject);
			MoreAEDisposeDesc(&itemObject);
			if (noErr == anErr)
			{
				anErr = AEPutParamDesc(&tAppleEvent,keyDirectObject,&propertyObject);
				MoreAEDisposeDesc(&propertyObject);
				if (noErr == anErr)
				{
					AEDescList tAEDescList = {typeNull,NULL};

					anErr = MoreAESendEventReturnAEDescList(pIdleProcUPP,&tAppleEvent,&tAEDescList);
					if (noErr == anErr)
					{
						anErr = MoreFECreateIconSuite(&tAEDescList,pIconSuiteHdl);

						MoreAEDisposeDesc(&tAEDescList);
					}
				}
			}
		}
		// always dispose of AEDescs when you are finished with them
		(void) MoreAEDisposeDesc(&tAppleEvent);
	}
	return anErr;
}	// end MoreFEGetItemIconSuite

/********************************************************************************
	Send an Apple event to the Finder to get a list of Finder-style object
	for each item on the desktop. This includes files (of all types),
	folders, and volumes.
	
	pIdleProcUPP	==>		A UPP for an idle function (required)
	pObjectList		==>		A null AEDesc.
					<==		A list containing object descriptors,
							or a null AEDesc if an error is encountered.
	
	See note about idle functions above.
*/
pascal OSErr MoreFEGetEveryItemOnDesktop(const AEIdleUPP pIdleProcUPP,AEDescList *pObjectList)
{
	OSErr		anErr = noErr;
	
	AppleEvent	tAppleEvent = {typeNull,NULL};

	if (NULL == pIdleProcUPP)	// the idle proc is required
		return paramErr;

	if (NULL == pObjectList)		// the object list is required
		return paramErr;

	MoreAENullDesc(pObjectList);

	anErr = MoreAECreateAppleEventSignatureTarget(kFinderFileType,kFinderCreatorType,
											  kAECoreSuite,kAEGetData,&tAppleEvent);
	if (noErr == anErr)
	{
		AEDesc		containerObj = {typeNull,NULL};		// start with the null (application) container
		AEDesc 		propertyObject = {typeNull,NULL};
		
		anErr = MoreAEOCreatePropertyObject(kDesktopFolderType,&containerObj,&propertyObject);
		if (noErr == anErr)
		{
			DescType	selectAll = kAEAll;
			AEDesc		allObjectsDesc = {typeNull,NULL};
			
			anErr = MoreAEOCreateSelectionObject(selectAll,&propertyObject,&allObjectsDesc);
			MoreAEDisposeDesc(&propertyObject);
			if (noErr == anErr)
			{
				anErr = AEPutParamDesc(&tAppleEvent,keyDirectObject,&allObjectsDesc);
				MoreAEDisposeDesc(&allObjectsDesc);

				if (noErr == anErr)
				{
					// All this is ignored on Mac OS X anyway
					// DescType tAliasDescType = typeAlias;
  					// anErr = AEPutKeyPtr(&tAppleEvent,keyAERequestedType,typeType,&tAliasDescType,sizeof(DescType));
  					// if (noErr == anErr)
					{
						anErr = MoreAESendEventReturnAEDescList(pIdleProcUPP,&tAppleEvent,pObjectList);
					}
				}
			}
		}
		// always dispose of AEDescs when you are finished with them
		(void) MoreAEDisposeDesc(&tAppleEvent);
	}
	return anErr;
}	// end MoreFEGetEveryItemOnDesktop

/********************************************************************************
	Send an Apple event to the Finder to get the current font and font size
	as set in the Views control panel.
	
	pIdleProcUPP	==>		A UPP for an idle function (required)
	pFont			==>		The font ID for the current view settings.
	pFontSize		==>		The font size for the current view settings.
	
	You MUST supply an idle function for this routine to work, since it is
	returning data.  You may use the simple handler provided in this library.
	
	See note about idle functions above.
	
	Errors
	
	-50		paramErr	No idle function provided.
*/
pascal OSErr MoreFEGetViewFontAndSize(const AEIdleUPP pIdleProcUPP,
										  SInt16 *pFont,
										  SInt16 *pFontSize)
{
	OSErr		anErr = noErr;
	
	AppleEvent	tAppleEvent = {typeNull,NULL};
		

	if (NULL == pIdleProcUPP)	// the idle proc is required
		return paramErr;

	if ((NULL == pFont) || (NULL == pFontSize))	// the font & fontsize parameters are required
		return paramErr;

	//	Build the event we will send
	anErr = MoreAECreateAppleEventSignatureTarget(kFinderFileType,kFinderCreatorType,
											  kAECoreSuite,kAEGetData,&tAppleEvent);
	if (noErr == anErr)
	{
		AEDesc		containerObj = {typeNull,NULL};		// start with the null (application) container
		AEDesc 		viewPropertyObj = {typeNull,NULL};
		//	Create an object descriptor for the view preferences property
		anErr = MoreAEOCreatePropertyObject(pViewPreferences,&containerObj,&viewPropertyObj);
		if (noErr == anErr)
		{
			AEDesc 		fontPropertyObj = {typeNull,NULL};
			
			//	Create an object descriptor for the view font property of the view preferences property
			anErr = MoreAEOCreatePropertyObject(pViewFont,&viewPropertyObj,&fontPropertyObj);
			//	Don't dispose of the fontPropertyObj yet, we will need it for the font size property
			if (noErr == anErr)
			{
				anErr = AEPutParamDesc(&tAppleEvent,keyDirectObject,&fontPropertyObj);
				(void) MoreAEDisposeDesc(&fontPropertyObj);
				if (noErr == anErr)
				{
					anErr = MoreAESendEventReturnSInt16(pIdleProcUPP,&tAppleEvent,pFont);
					if (noErr == anErr)
					{
						AEDesc 		fontSizePropertyObj = {typeNull,NULL};
						
						anErr = MoreAEOCreatePropertyObject(pViewFontSize,&viewPropertyObj,&fontSizePropertyObj);
						// always dispose of AEDescs when you are finished with them
						(void) MoreAEDisposeDesc(&viewPropertyObj);
						if (noErr == anErr)
						{
							anErr = AEPutParamDesc(&tAppleEvent,keyDirectObject,&fontSizePropertyObj);
							(void) MoreAEDisposeDesc(&fontSizePropertyObj);
							if (noErr == anErr)
							{
								anErr = MoreAESendEventReturnSInt16(pIdleProcUPP,&tAppleEvent,pFontSize);
							}
						}
					}
				}
			}
		}
		// always dispose of AEDescs when you are finished with them
		(void) MoreAEDisposeDesc(&tAppleEvent);
	}
	return anErr;
}	// end MoreFEGetViewFontAndSize

/********************************************************************************
	Send an Apple event to the Finder to update the display of the item specified
	by the FSSpec.

	pFSSpecPtr		==>		The item whose display should be updated.

	No reply is returned, so none is asked for. Hence, no idle function is needed.
*/
pascal OSErr MoreFEUpdateItemFSS(const FSSpecPtr pFSSpecPtr)
{
	OSErr			anErr = noErr;
	AliasHandle		tAliasHdl;
	
	anErr = NewAlias(NULL,pFSSpecPtr,&tAliasHdl);

	if (noErr == anErr)
	{
		anErr = MoreFEUpdateItemAlias(tAliasHdl);
	}
	
	DisposeHandle((Handle) tAliasHdl);
	
	return anErr;
}	// end MoreFEUpdateItemFSS

/********************************************************************************
	Send an Apple event to the Finder to update the display of the item specified
	by the FSRef.

	pFSRefPtr		==>		The item whose display should be updated.

	No reply is returned, so none is asked for. Hence, no idle function is needed.
*/
pascal OSErr MoreFEUpdateItemFSRef(const FSRefPtr pFSRefPtr)
{
	OSErr			anErr = noErr;
	AliasHandle		tAliasHdl;
	
	anErr = FSNewAlias(NULL,pFSRefPtr,&tAliasHdl);

	if (noErr == anErr)
	{
		anErr = MoreFEUpdateItemAlias(tAliasHdl);
	}
	
	DisposeHandle((Handle) tAliasHdl);
	
	return anErr;
}	// end MoreFEUpdateItemFSS

/********************************************************************************
	Send an Apple event to the Finder (any version) to set the visibility of
	a process specified by it's PSN.
	
	pPSN			==>		The processes serial number
	pVisible		==>		Make the process visible or not

	Note: should I be using kAEMiscStandards, kAEMakeObjectsVisible?
	
*/
pascal OSErr MoreFESetProcessVisibility(const ProcessSerialNumberPtr pPSN,Boolean pVisible)
{
	OSErr		anErr = noErr;
	AppleEvent	tAppleEvent = {typeNull,NULL};
	
	anErr = MoreAECreateAppleEventSignatureTarget(kFinderFileType,kFinderCreatorType,
											  kAECoreSuite,kAESetData,&tAppleEvent);
	if (noErr == anErr)
	{
		OSType		processType;
		OSType		processSignature;
		
		anErr = MoreProcGetProcessTypeSignature(pPSN,&processType,&processSignature);
		if (noErr == anErr)
		{
			AEDesc 	processObject = {typeNull,NULL};
			
		//	To hide the Finder it's visible property is directly set to false -- no process target
		//	is needed.  Only create a non-null processObject if the Finder is not the target.
			if ((processSignature != kFinderProcessSignature)  ||  (processType != kFinderProcessType))
			{	
				Str32	processName;
				
			//	The Finder expects the process to be specified by name.
				anErr = MoreProcGetProcessName(pPSN,processName);
				if (noErr == anErr)
				{
					AEDesc	containerObj = {typeNull,NULL};
					AEDesc	nameDesc = {typeNull,NULL};
					
					anErr = AECreateDesc(typeChar,&processName[1],processName[0],&nameDesc);
					if (noErr == anErr)
					{
						anErr = CreateObjSpecifier(cProcess,&containerObj,formName,
													&nameDesc,false,&processObject);
						(void) MoreAEDisposeDesc(&nameDesc);
					}
				}
			}
			
			if (noErr == anErr)
			{
				AEDesc	propertyObject = {typeNull,NULL};

			//	To hide a process tell the Finder to set the process' visible property to false.
				anErr = MoreAEOCreatePropertyObject(pVisible,&processObject,&propertyObject);
				(void) MoreAEDisposeDesc(&processObject);
				if (noErr == anErr)
				{
					anErr = AEPutParamDesc(&tAppleEvent,keyDirectObject,&propertyObject);
					(void) MoreAEDisposeDesc(&propertyObject);
					if (noErr == anErr)
					{
						anErr = AEPutParamPtr(&tAppleEvent,keyAEData,typeBoolean,&pVisible,sizeof(Boolean));
						if (noErr == anErr)
						{
							//	Send the event. In this case we don't care about the reply
							anErr = MoreAESendEventNoReturnValue(NULL,&tAppleEvent);
						}
					}
				}
			}
		}
		// always dispose of AEDescs when you are finished with them
		(void) MoreAEDisposeDesc(&tAppleEvent);
	}
	return anErr;
}	// end MoreFESetProcessVisibility

/********************************************************************************
	Send an Apple event to the Finder to update the display of the item specified
	by the alias.

	pAliasHdl		==>		The item whose display should be updated.

	No reply is returned, so none is asked for. Hence, no idle function is needed.
*/
pascal OSErr MoreFEUpdateItemAlias(const AliasHandle pAliasHdl)
{
	OSErr		anErr = noErr;
	
	AppleEvent	tAppleEvent = {typeNull,NULL};
	
	anErr = MoreAECreateAppleEventSignatureTarget(kFinderFileType,kFinderCreatorType,
											  kAEFinderSuite,kAESync,&tAppleEvent);
	if (noErr == anErr)
	{
		AEDesc 		aliasDesc = {typeNull,NULL};
		
		anErr = MoreAEOCreateAliasDesc(pAliasHdl,&aliasDesc);
		if (noErr == anErr)
		{
			anErr = AEPutParamDesc(&tAppleEvent,keyDirectObject,&aliasDesc);
			MoreAEDisposeDesc(&aliasDesc);
			if (noErr == anErr)
			{
				//	Send the event. In this case we don't care about the reply
				anErr = MoreAESendEventNoReturnValue(NULL,&tAppleEvent);
			}
		}
		// always dispose of AEDescs when you are finished with them
		(void) MoreAEDisposeDesc(&tAppleEvent);
	}
	return anErr;
}	// end MoreFEUpdateItemAlias

/********************************************************************************
	Send an odoc Apple event to the Finder to open the item specified by the FSSpec.
	
	This routine can be used to open a file with it's creator app, launch an app,
	open a control panel. Pretty much open anything you can open directly in the
	Finder by double-clicking.
	
	pFSSpecPtr		==>		The item whose display should be updated.

	No reply is returned, so none is asked for. Hence, no idle function is needed.
*/
pascal	OSErr MoreFEOpenFile(const FSSpecPtr pFSSpecPtr)
{
	AEDesc 		fileObjDesc = {typeNull,NULL};
	OSErr		anErr = noErr;
	
	anErr = MoreAEOCreateObjSpecifierFromFSSpec(pFSSpecPtr,&fileObjDesc);
	if (noErr == anErr)
	{
	AppleEvent	tAppleEvent = {typeNull,NULL};
	AEBuildError	tAEBuildError;

	anErr = AEBuildAppleEvent(
		        kCoreEventClass,kAEOpenDocuments,
				typeApplSignature,&gFinderSignature,sizeof(OSType),
		        kAutoGenerateReturnID,kAnyTransactionID,
			        &tAppleEvent,&tAEBuildError,
		        "'----':(@)",&fileObjDesc);

		(void) MoreAEDisposeDesc(&fileObjDesc);

		if (noErr == anErr)
		{	
			//	Send the event. In this case we don't care about the reply
		anErr = MoreAESendEventNoReturnValue(NULL,&tAppleEvent);
			// always dispose of AEDescs when you are finished with them
			(void) MoreAEDisposeDesc(&tAppleEvent);
		}
	}
	return anErr;
}	// end MoreFEOpenFile

/********************************************************************************
	Send an pdoc Apple event to the Finder to print the item specified by the FSSpec.

	pFSSpecPtr		==>		The item to be printed

	No reply is returned, so none is asked for. Hence, no idle function is needed.
*/
pascal	OSErr MoreFEPrintFile(const FSSpecPtr pFSSpecPtr)
{
	AEDesc 		fileObjDesc = {typeNull,NULL};
	OSErr		anErr = noErr;
		
	anErr = MoreAEOCreateObjSpecifierFromFSSpec(pFSSpecPtr,&fileObjDesc);
	if (noErr == anErr)
	{
		AppleEvent	tAppleEvent = {typeNull,NULL};
		AEBuildError	tAEBuildError;

		anErr = AEBuildAppleEvent(
		        kCoreEventClass,kAEPrintDocuments,
				typeApplSignature,&gFinderSignature,sizeof(OSType),
		        kAutoGenerateReturnID,kAnyTransactionID,
		        &tAppleEvent,&tAEBuildError,
		        "'----':(@)",&fileObjDesc);

		(void) MoreAEDisposeDesc(&fileObjDesc);

		if (noErr == anErr)
		{	
			//	Send the event. In this case we don't care about the reply
			anErr = MoreAESendEventNoReturnValue(NULL,&tAppleEvent);
			// always dispose of AEDescs when you are finished with them
			(void) MoreAEDisposeDesc(&tAppleEvent);
		}
	}
	return anErr;
}	// end MoreFEPrintFile

/********************************************************************************
	Send an odoc Apple event to the Finder to open the info window
	for the item specified by the FSSpec.

	pFSSpecPtr		==>		The item whose display should be updated.
	pIdleProcUPP 	==>		NULL for default, or UPP for the idle function to use

	See note about idle functions above.

 Apple event record at: 16BEAAF4
  Class: aevt  ID: odoc  Target Type: psn   "Finder"
  
  KEY  TYPE LENGTH  DATA
  ---- obj  0000B4  
       KEY  TYPE LENGTH  DATA
       form enum 000004  prop
       want type 000004  prop
       seld type 000004  iwnd
       from obj  000070  
            KEY  TYPE LENGTH  DATA
            want type 000004  alis
            from null 000000  Null object
            form enum 000004  name
            seld TEXT 000030  GeoWar1_28Gb:Source:AE&A

*/
pascal OSErr MoreFEOpenInfoWindow(const FSSpecPtr pFSSpecPtr,const AEIdleUPP pIdleProcUPP)
{
	AppleEvent tAppleEvent = {typeNull,NULL};	//	If you always init AEDescs, it's always safe to dispose of them.
	AEBuildError	tAEBuildError;
	AEDesc 			tAEDesc = {typeNull,NULL};
	OSErr anErr = noErr;

	anErr = MoreAEOCreateObjSpecifierFromFSSpec(pFSSpecPtr,&tAEDesc);
	if (noErr == anErr)
	{
		anErr = AEBuildAppleEvent(
			        kCoreEventClass,kAEOpenDocuments,
					typeApplSignature,&gFinderSignature,sizeof(OSType),
			        kAutoGenerateReturnID,kAnyTransactionID,
			        &tAppleEvent,&tAEBuildError,
			        "'----':obj {form:prop,want:type(prop),seld:type(iwnd),from:(@)}",
			        &tAEDesc);
		if (noErr == anErr)
		{
#if 0	// Set this true to printf the Apple Event before you send it.
		Handle strHdl;

		anErr = AEPrintDescToHandle(&tAppleEvent,&strHdl);
			if (noErr == anErr)
			{
			char	nul	= '\0';

			PtrAndHand(&nul,strHdl,1);
			printf("\n�MoreFEOpenInfoWindow: tAppleEvent=%s.",*strHdl); fflush(stdout);
			DisposeHandle(strHdl);
		}
#endif
					//	Send the event. In this case we don't care about the reply
					anErr = MoreAESendEventNoReturnValue(pIdleProcUPP,&tAppleEvent);
			// always dispose of AEDescs when you are finished with them
			(void) MoreAEDisposeDesc(&tAppleEvent);
		}
	}
	return anErr;
}	// end MoreFEOpenInfoWindow

/********************************************************************************
	Send an Apple event to the Finder to create a new alias file at destFSSPtr
	location, with the file at sourceFSSPtr as the target, with newName as it's name.
	
	pSourceFSSPtr	==>		The target for the new alias file.
	pDestFSSPtr		==>		The location for the new alias file.
	pNewName			==>		The name for the new alias file.
	pIdleProcUPP	==>		A UniversalProcPtr for an idle function, or NULL.

	A reference to the newly created alias file will be returned. Currently this
	function ignores this result, but you could extract it if needed. You can ask
	that the result be returned as a Finder style object, as an alias, or as an FSSpec.

	See note about idle functions above.
*/
pascal	OSErr	MoreFECreateAliasFile(const FSSpecPtr pSourceFSSPtr,
								  const FSSpecPtr pDestFSSPtr,
								  ConstStr63Param pNewName,
								  const AEIdleUPP pIdleProcUPP)
{
	OSErr		anErr = noErr;
	
	AppleEvent	tAppleEvent = {typeNull,NULL};
	
	anErr = MoreAECreateAppleEventSignatureTarget(kFinderFileType,kFinderCreatorType,
											  kAECoreSuite,kAECreateElement,&tAppleEvent);
	if (noErr == anErr)
	{
		//	put the type of item to make into the Apple event
		OSType	anAliasType = typeAlias;
		
		anErr = AEPutParamPtr(&tAppleEvent,keyAEObjectClass,typeType,&anAliasType,sizeof(OSType));
		if (noErr == anErr)
		{
			//	add an alias that points to the targer for the alias file to be created
			anErr = MoreAEOAddAliasParameterFromFSSpec(pSourceFSSPtr,keyASPrepositionTo,&tAppleEvent);
			if (noErr == anErr)
			{
				//	where should the alias file be created?
				anErr = MoreAEOAddAliasParameterFromFSSpec(pDestFSSPtr,keyAEInsertHere,&tAppleEvent);
				if (noErr == anErr)
				{
					//	add a properties record so we can specify the name of the new alias file.
					AERecord	properties = {typeNull,NULL};
					
					anErr = AECreateList(NULL,0,true,&properties);
					if (noErr == anErr)
					{
						anErr = AEPutKeyPtr(&properties,keyAEName,typeChar,pNewName+1,*pNewName);
						if (noErr == anErr)
						{
							anErr = AEPutParamDesc(&tAppleEvent,keyAEPropData,&properties);
							if (noErr == anErr)
							{
								//	Send the event. In this case we don't care about the reply,but if we did we
								//	would get back an object that describes the alias file just created
								//	(either an object descriptor, an alias, or an FSSpec, depending on what we ask for).
								anErr = MoreAESendEventNoReturnValue(pIdleProcUPP,&tAppleEvent);
							}
						}
						MoreAEDisposeDesc(&properties);
					}
				}
			}
		}
		// always dispose of AEDescs when you are finished with them
		(void) MoreAEDisposeDesc(&tAppleEvent);
	}
	return anErr;
}	// end MoreFECreateAliasFile
/********************************************************************************
	Send set data Apple event to the Finder to change the position of the icon
	for the item specified by the FSSpec.  The Finder's display of the item is
	immediately updated.
	
	NOTE:	The position parameter is a Point, with it's values in y/x order,
			where as the Finder expects positions to be in x/y order.  This
			routine changes the order for you. 
	
	pFSSpecPtr		==>		The item whose position will be changed.
	pPoint			==>		The new position for the item.

	No reply is returned, so none is asked for. Hence, no idle function is needed.
*/
pascal OSErr MoreFEMoveDiskIcon(const FSSpecPtr pFSSpecPtr,
							  const Point pPoint)
{
	OSErr		anErr = noErr;
	
	AppleEvent	tAppleEvent = {typeNull,NULL};
	
	anErr = MoreAECreateAppleEventSignatureTarget(kFinderFileType,kFinderCreatorType,
											  kAECoreSuite,kAESetData,&tAppleEvent);
	if (noErr == anErr)
	{
		AEDesc 		itemObject = {typeNull,NULL};
		
		anErr = MoreAEOCreateObjSpecifierFromFSSpec(pFSSpecPtr,&itemObject);
		if (noErr == anErr)
		{
			AEDesc 		propertyObject = {typeNull,NULL};
			
			anErr = MoreAEOCreatePropertyObject(pPosition,&itemObject,&propertyObject);
			MoreAEDisposeDesc(&itemObject);
			if (noErr == anErr)
			{
				anErr = AEPutParamDesc(&tAppleEvent,keyDirectObject,&propertyObject);
				MoreAEDisposeDesc(&propertyObject);
				if (noErr == anErr)
				{
					AEDescList	posList;
					
					anErr = MoreAEOCreatePositionList(pPoint,&posList);
					if (noErr == anErr)
					{
						anErr = AEPutParamDesc(&tAppleEvent,keyAEData,&posList);
						if (noErr == anErr)
						{	
							//	Send the event. In this case we don't care about the reply
							anErr = MoreAESendEventNoReturnValue(NULL,&tAppleEvent);
						}
					}
				}
			}
		}
		// always dispose of AEDescs when you are finished with them
		(void) MoreAEDisposeDesc(&tAppleEvent);
	}
	return anErr;
}	// end MoreFEMoveDiskIcon

/********************************************************************************
	Make an icon suite containing the icons in an icon family record, as returned
	by the Finder. Behaves simmilar to a call to GetIconSuite, i.e., a new icon
	suite handle will be returned in pIconSuiteHdl.
	
	pIconFamilyAEDescList	input:	The icon family to process.
	pIconSuiteHdl		input:	Pointer to an icon suite handle variable
						output:	An icon suite containing the icons from the
								icon family record.
	
	Result Codes
	____________
	noErr				    0	No error	
	paramErr			  -50	The value of target or alias parameter, or of
								both, is NIL, or the alias record is corrupt
	memFullErr			 -108	Not enough room in heap zone	
	errAECoercionFail 	-1700	Data could not be coerced to the requested 
								Apple event data type	
*/
pascal	OSErr	MoreFECreateIconSuite(const AEDescList *pIconFamilyAEDescList,
										Handle *pIconSuiteHdl)
{
	OSErr	anErr = noErr;

	const	long		iconTypesCnt = 6;
	static	DescType	iconTypes[] = {	typeIconAndMask,type8BitMask,type32BitIcon,type8BitIcon,
										type4BitIcon,typeSmallIconAndMask,typeSmall8BitMask,
										typeSmall32BitIcon,typeSmall8BitIcon,typeSmall4BitIcon};
	
	AEDescList	iconList = {typeNull,NULL};
	
	if (MoreFEUsesIconFamily())
	{
		anErr = AECoerceDesc(pIconFamilyAEDescList,typeAERecord,&iconList);
	}
	else
	{
		iconList = *pIconFamilyAEDescList;
	}
	
	if (noErr == anErr)
	{
		anErr = NewIconSuite(pIconSuiteHdl);
		if (noErr == anErr)
		{
			long	index;
			AEDesc	iconDataDesc = {typeNull,NULL};
			
			for (index = 0; index < iconTypesCnt; index++)
			{
				anErr = AEGetKeyDesc(&iconList,iconTypes[ index ],
									  typeWildCard,&iconDataDesc);
				if (noErr == anErr)
				{
					Handle tmpH;
					
					anErr = MoreAECopyDescriptorDataToHandle(&iconDataDesc,&tmpH);
					if (noErr == anErr) {
						anErr = AddIconToSuite(tmpH,*pIconSuiteHdl,iconTypes[index]);
						DisposeHandle(tmpH);
						assert(noErr == MemError());
					}
				}
			}
		}
	}
	MoreAEDisposeDesc(&iconList);

	return anErr;
}	// end MoreFECreateIconSuite
/********************************************************************************
	Used by MoreFECreateIconFamilyRecord, passed to ForEachIconDo as the IconAction
	function. Puts each icon in an icon suite into the descriptor record passed
	in the myDataPtr parameter.
*/
static pascal OSErr MyIconAction(ResType theIconType,Handle *theIcon,void *myDataPtr)
{
	OSErr	anErr = noErr;
	
	if (*theIcon != NULL)	// only add the icon if it's really there
	{
		AEDescList *pIconFamilyAEDescList = (AEDescList*)myDataPtr;
		
		anErr = AEPutKeyPtr(pIconFamilyAEDescList,theIconType,theIconType,
							 **theIcon,GetHandleSize(*theIcon));
	}
		
	return anErr;
}	// end MyIconAction
/********************************************************************************
	Make an icon family record containing the icons specified in the
	pIconSelector parameter.
	The pIconSuiteHdl parameter should contain an icon suite, as returned by a
	call to GetIconSuite.
	
	pIconSuiteHdl			input:	The icon suite to build the record from.
	pIconSelector			input:	Which icons to include in the record.
	pIconFamilyAEDescList	input:	Pointer to null AEDesc.
							output:	An AERecord that's been coerced to an
								icon family record.
	
	Result Codes
	____________
	noErr				    0	No error	
	paramErr			  -50	The value of target or alias parameter, or of
								both, is NIL, or the alias record is corrupt
	memFullErr			 -108	Not enough room in heap zone	
	errAECoercionFail 	-1700	Data could not be coerced to the requested 
								Apple event data type	
*/
pascal	OSErr	MoreFECreateIconFamilyRecord(	const Handle pIconSuiteHdl,
												const IconSelectorValue pIconSelector,
												AEDescList *pIconFamilyAEDescList)
{
	OSErr	anErr = noErr;
	AEDescList	iconList = {typeNull,NULL};
	
	static	IconActionUPP iconActionUPP;
	
	if (iconActionUPP == NULL)
		iconActionUPP = NewIconActionUPP(&MyIconAction);

	// create a record for the icon family
	anErr = AECreateList(NULL,0,true,&iconList);
	
	if (noErr == anErr)
	{
		ForEachIconDo(pIconSuiteHdl,pIconSelector,iconActionUPP,&iconList);
		
		if (MoreFEUsesIconFamily())
		{
			anErr = AECoerceDesc(&iconList,typeIconFamily,pIconFamilyAEDescList);
		}
		else
		{
			*pIconFamilyAEDescList = iconList;
		}
		
		MoreAEDisposeDesc(&iconList);
	}

	return anErr;
}	// end MoreFECreateIconFamilyRecord
/********************************************************************************
	Does the Finder call AEProcessAppleEvent when it receives an Apple event.
	
	RESULT CODES
	____________
	true	Finder is version 7.1.3 or later, calls AEProcessAppleEvent,
			and supports the full old Finder event suite.
	false	The Finder supports a subset of the old Finder event suite.
	
	Use this routine together with MoreFEIsOSLCompliant to determine which
	suite of events the Finder supports.
*/
pascal Boolean MoreFECallsAEProcess(void)
{
	static	long		gMFECallsAEProcess = kFlagNotSet;
		
	if (gMFECallsAEProcess == kFlagNotSet)
	{
		long	response;
		
		if (Gestalt(gestaltFinderAttr,&response) == noErr)
		{
			gMFECallsAEProcess = (response & (1L << gestaltFinderCallsAEProcess)) != 0;
		}
	}
	
	return gMFECallsAEProcess;
}	// end MoreFECallsAEProcess
/********************************************************************************
	Does the Finder uses the ObjectSupportLib to resolve objecs.
		
	RESULT CODES
	____________
	true	Finder is version 7.5 or later, and supports the Standard event suite,
			the new Finder event suite & a subset of the old Finder event suite.
	false	See result of MoreFECallsAEProcess.
	
	Support for the old Finder event suite is limited, with some events missing
	and some events only partially supported. The subset of old Finder event
	supported is not the same subset as the original Finder 7 supported.
	
	Use the new event suite, and avoid the old one, whenever possible. 
*/
pascal Boolean MoreFEIsOSLCompliant(void)
{
	static	long		gMFEIsOSLCompliant = kFlagNotSet;
	
	if (gMFEIsOSLCompliant == kFlagNotSet)
	{
		long	response;
		
		if (Gestalt(gestaltFinderAttr,&response) == noErr)
		{
			gMFEIsOSLCompliant = (response & (1L << gestaltOSLCompliantFinder)) != 0;
		}
	}
	return gMFEIsOSLCompliant;
}	// end MoreFEIsOSLCompliant
/********************************************************************************
	Does the Finder uses cIconFamily records or IconSuites.
	
	So far (as of Mac OS 8.1) only the Finder in Mac OS 8.0 requires the use of
	IconSuites, rather than cIconFamily like the other Finders.  This test
	lets us identify this odd Finder so we can special case Icon code.
		
	RESULT CODES
	____________
	true	Finder uses cIconFamily.
	false	Finder uses IconSuite.
*/
pascal Boolean MoreFEUsesIconFamily(void)
{
	static	long		gMFEUsesIconFamily = kFlagNotSet;
	
	if (gMFEUsesIconFamily == kFlagNotSet)
	{
		long	response;
		
		if (Gestalt(gestaltSystemVersion,&response) == noErr)
		{
			gMFEUsesIconFamily = (response != 0x00000800);
		}
	}
	
	return gMFEUsesIconFamily;
}	// end MoreFEUsesIconFamily

/********************************************************************************
	Send an Apple event to the Finder to get the finder comment of the item 
	specified by the FSSpecPtr.

	pFSSpecPtr		==>		The item to get the file kind of.
	pCommentStr		==>		A string into which the finder comment will be returned.
	pIdleProcUPP	==>		A UPP for an idle function (required)
	
	See note about idle functions above.
*/
pascal OSErr MoreFEGetComment(const FSSpecPtr pFSSpecPtr,Str255 pCommentStr,const AEIdleUPP pIdleProcUPP)
{
	AppleEvent tAppleEvent = {typeNull,NULL};	//	If you always init AEDescs, it's always safe to dispose of them.
	AEDesc tAEDesc = {typeNull,NULL};
	OSErr anErr = noErr;

	if (NULL == pIdleProcUPP)	// the idle proc is required
		return paramErr;

	anErr = MoreAEOCreateObjSpecifierFromFSSpec(pFSSpecPtr,&tAEDesc);
	if (noErr == anErr)
	{
		AEBuildError	tAEBuildError;

		anErr = AEBuildAppleEvent(
			        kAECoreSuite,kAEGetData,
					typeApplSignature,&gFinderSignature,sizeof(OSType),
			        kAutoGenerateReturnID,kAnyTransactionID,
			        &tAppleEvent,&tAEBuildError,
			        "'----':obj {form:prop,want:type(prop),seld:type(comt),from:(@)}",&tAEDesc);

        // always dispose of AEDescs when you are finished with them
        (void) MoreAEDisposeDesc(&tAEDesc);

		if (noErr == anErr)
		{	
			//	Send the event.
			anErr = MoreAESendEventReturnPString(pIdleProcUPP,&tAppleEvent,pCommentStr);
			// always dispose of AEDescs when you are finished with them
			(void) MoreAEDisposeDesc(&tAppleEvent);
		}
	}
	return anErr;
}	// end MoreFEGetComment

/********************************************************************************
	Send an Apple event to the Finder to set the finder comment of the item 
	specified by the FSSpecPtr.

	pFSSpecPtr		==>		The item to set the file kind of.
	pCommentStr		==>		A string to which the file comment will be set
	pIdleProcUPP	==>		A UPP for an idle function, or NULL.
	
	See note about idle functions above.
*/
pascal OSErr MoreFESetComment(const FSSpecPtr pFSSpecPtr,const Str255 pCommentStr,const AEIdleUPP pIdleProcUPP)
{
	AppleEvent tAppleEvent = {typeNull,NULL};	//	If you always init AEDescs, it's always safe to dispose of them.
	AEBuildError	tAEBuildError;
	AEDesc 			tAEDesc = {typeNull,NULL};
	OSErr anErr = noErr;

	anErr = MoreAEOCreateObjSpecifierFromFSSpec(pFSSpecPtr,&tAEDesc);
	if (noErr == anErr)
	{
		char* dataPtr = NewPtr(pCommentStr[0]);

		CopyPascalStringToC(pCommentStr,dataPtr);
		anErr = AEBuildAppleEvent(
			        kAECoreSuite,kAESetData,
					typeApplSignature,&gFinderSignature,sizeof(OSType),
			        kAutoGenerateReturnID,kAnyTransactionID,
			        &tAppleEvent,&tAEBuildError,
			        "'----':obj {form:prop,want:type(prop),seld:type(comt),from:(@)},data:'TEXT'(@)",
			        &tAEDesc,dataPtr);

		DisposePtr(dataPtr);

		if (noErr == anErr)
		{	
			//	Send the event. In this case we don't care about the reply
			anErr = MoreAESendEventNoReturnValue(pIdleProcUPP,&tAppleEvent);
			// always dispose of AEDescs when you are finished with them
			(void) MoreAEDisposeDesc(&tAppleEvent);
		}
	}
	return anErr;
}	// end MoreFESetComment


/********************************************************************************
	Send an Apple event to the Finder to get the finder comment of the item 
	specified by the FSRefPtr.

	pFSRefPtr		==>		The item to get the file kind of.
	pCommentStr		==>		A string into which the finder comment will be returned.
	pIdleProcUPP	==>		A UPP for an idle function (required)
	
	See note about idle functions above.
*/
#if TARGET_API_MAC_CARBON
pascal OSErr MoreFEGetCommentCFString(const FSRefPtr pFSRefPtr,CFStringRef* pCommentStr,const AEIdleUPP pIdleProcUPP)
{
	AppleEvent tAppleEvent = {typeNull,NULL};	//	If you always init AEDescs, it's always safe to dispose of them.
	AEDesc tAEDesc = {typeNull,NULL};
	OSErr anErr = noErr;

	if (NULL == pIdleProcUPP)	// the idle proc is required
		return paramErr;


	anErr = MoreAEOCreateObjSpecifierFromFSRef(pFSRefPtr,&tAEDesc);

	if (noErr == anErr)
	{
		AEBuildError	tAEBuildError;

		anErr = AEBuildAppleEvent(
			        kAECoreSuite,kAEGetData,
					typeApplSignature,&gFinderSignature,sizeof(OSType),
			        kAutoGenerateReturnID,kAnyTransactionID,
			        &tAppleEvent,&tAEBuildError,
			        "'----':obj {form:prop,want:type(prop),seld:type(comt),from:(@)}",&tAEDesc);

        // always dispose of AEDescs when you are finished with them
        (void) MoreAEDisposeDesc(&tAEDesc);

		if (noErr == anErr)
		{	
#if 0	// Set this true to printf the Apple Event before you send it.
			Handle strHdl;
			anErr = AEPrintDescToHandle(&tAppleEvent,&strHdl);
			if (noErr == anErr)
			{
				char	nul	= '\0';
				PtrAndHand(&nul,strHdl,1);
				printf("\n-MoreFEGetCommentCFString: tAppleEvent=%s.",*strHdl); fflush(stdout);
				DisposeHandle(strHdl);
			}
#endif
			//	Send the event.
			anErr = MoreAESendEventReturnAEDesc(pIdleProcUPP,&tAppleEvent,typeUnicodeText,&tAEDesc);
			// always dispose of AEDescs when you are finished with them
			(void) MoreAEDisposeDesc(&tAppleEvent);
			if (noErr == anErr)
			{
				anErr = MoreAEGetCFStringFromDescriptor(&tAEDesc,pCommentStr);
				// always dispose of AEDescs when you are finished with them
				(void) MoreAEDisposeDesc(&tAEDesc);
			}
		}
	}
	return anErr;
}	// end MoreFEGetCommentCFString
#endif

/********************************************************************************
Send an Apple event to the Finder to get the finder comment of the item
specified by the FSRefPtr.
pFSRefPtr		==>		The item to get the file kind of.
pCommentStr		==>		A string into which the finder comment will be returned.
pIdleProcUPP	==>		A UPP for an idle function (required)
See note about idle functions above.
*/
#if TARGET_API_MAC_CARBON
pascal OSErr MoreFEGetObjCommentCFString(const AEDesc pAEDesc,CFStringRef* pCommentStr,const AEIdleUPP pIdleProcUPP)
 {
	AppleEvent tAppleEvent = {typeNull,NULL};	//	If you always init AEDescs, it's always safe to dispose of them.
	OSErr anErr = noErr;
	if (NULL == pIdleProcUPP)	// the idle proc is required
		return paramErr;
	{
		AEBuildError	tAEBuildError;
		anErr = AEBuildAppleEvent(
							kAECoreSuite,kAEGetData,
							typeApplSignature,&gFinderSignature,sizeof(OSType),
							kAutoGenerateReturnID,kAnyTransactionID,
							&tAppleEvent,&tAEBuildError,
							"'----':obj {form:prop,want:type(prop),seld:type(comt),from:(@)}",&pAEDesc);
		if (noErr == anErr)
		{
			AEDesc tAEDesc;
#if 0	// Set this true to printf the Apple Event before you send it.
			Handle strHdl;
			anErr = AEPrintDescToHandle(&tAppleEvent,&strHdl);
			if (noErr == anErr)
			{
				char	nul	= '\0';
				PtrAndHand(&nul,strHdl,1);
				printf("\n-MoreFEGetObjCommentCFString: tAppleEvent=\"%s\".",*strHdl); fflush(stdout);
				DisposeHandle(strHdl);
			}
#endif
			//	Send the event.
			anErr = MoreAESendEventReturnAEDesc(pIdleProcUPP,&tAppleEvent,typeUnicodeText,&tAEDesc);
			// always dispose of AEDescs when you are finished with them
			(void) MoreAEDisposeDesc(&tAppleEvent);
			if (noErr == anErr)
			{
				anErr = MoreAEGetCFStringFromDescriptor(&tAEDesc,pCommentStr);
				// always dispose of AEDescs when you are finished with them
				(void) MoreAEDisposeDesc(&tAEDesc);
			}
		}
	}
	return anErr;
 }	// end MoreFEGetCommentCFString
#endif
/********************************************************************************
	Send an Apple event to the Finder to set the finder comment of the item 
	specified by the FSRefPtr.

	pFSRefPtr		==>		The item to set the file kind of.
	pCommentStr		==>		A string to which the file comment will be set
	pIdleProcUPP	==>		A UPP for an idle function, or NULL.
	
	See note about idle functions above.
*/
#if TARGET_API_MAC_CARBON
pascal OSErr MoreFESetCommentCFString(const FSRefPtr pFSRefPtr,const CFStringRef pCommentStr,const AEIdleUPP pIdleProcUPP)
{
	AppleEvent 		tAppleEvent = {typeNull,NULL};	//	If you always init AEDescs, it's always safe to dispose of them.
	AEBuildError	tAEBuildError;
	AEDesc 			itemObject = {typeNull,NULL};
	OSErr			anErr = noErr;

	anErr = MoreAEOCreateObjSpecifierFromFSRef(pFSRefPtr,&itemObject);
	if (noErr == anErr)
	{
		AEDesc tAEDesc = {typeNull,NULL};

		anErr = MoreAECreateAEDescFromCFString(pCommentStr,&tAEDesc);
		if (noErr == anErr)
		{
			anErr = AEBuildAppleEvent(
				        kAECoreSuite,kAESetData,
						typeApplSignature,&gFinderSignature,sizeof(OSType),
				        kAutoGenerateReturnID,kAnyTransactionID,
				        &tAppleEvent,&tAEBuildError,
				        "'----':obj {form:prop,want:type(prop),seld:type(comt),from:(@)},data:(@)",
				        &itemObject,&tAEDesc);

			if (noErr == anErr)
			{	
				//	Send the event. In this case we don't care about the reply
				anErr = MoreAESendEventNoReturnValue(pIdleProcUPP,&tAppleEvent);
				// always dispose of AEDescs when you are finished with them
				(void) MoreAEDisposeDesc(&tAppleEvent);
			}
			// always dispose of AEDescs when you are finished with them
			(void) MoreAEDisposeDesc(&tAEDesc);
		}
	}

	return anErr;
}	// end MoreFESetCommentCFString
#endif

/********************************************************************************
	Send an Apple event to the Finder to get the finder object as an alias

	pAEDesc			==>		The finder object to get the alias of
	pAliasHdl		==>		An alias handle to which the objects alias will be set
	pIdleProcUPP	==>		A UPP for an idle function (required)
	
	See note about idle functions above.
*/
pascal OSErr MoreFEGetObjectAsAlias(const AEDesc* pAEDesc,AliasHandle* pAliasHdl,const AEIdleUPP pIdleProcUPP)
{
	AppleEvent tAppleEvent = {typeNull,NULL};	//	If you always init AEDescs, it's always safe to dispose of them.
	OSErr anErr = noErr;

	// the descriptor pointer, alias handle and idle proc are required
	if ((NULL == pAEDesc) || (NULL == pAliasHdl) || (NULL == pIdleProcUPP))
		return paramErr;

	if (typeObjectSpecifier != pAEDesc->descriptorType)
		return paramErr;	// this has to be an object specifier

	anErr = MoreAECreateAppleEventSignatureTarget(kFinderFileType,kFinderCreatorType,
											  kAECoreSuite,kAEGetData,&tAppleEvent);
	if (noErr == anErr)
	{

		anErr = AEPutParamDesc(&tAppleEvent,keyDirectObject,pAEDesc);
		if (noErr == anErr)
		{
			DescType tAliasDescType = typeAlias;

			anErr = AEPutKeyPtr(&tAppleEvent,keyAERequestedType,typeType,&tAliasDescType,sizeof(DescType));
			if (noErr == anErr)
			{
				AEDesc tAEDesc;

				anErr = MoreAESendEventReturnAEDesc(pIdleProcUPP,&tAppleEvent,typeAlias,&tAEDesc);
				if (noErr == anErr)
				{
					anErr = MoreAECopyDescriptorDataToHandle(&tAEDesc,(Handle*) pAliasHdl);
					// always dispose of AEDescs when you are finished with them
					(void) MoreAEDisposeDesc(&tAppleEvent);
				}
			}
		}
		// always dispose of AEDescs when you are finished with them
		(void) MoreAEDisposeDesc(&tAppleEvent);
	}
	return anErr;
}	// end MoreFEGetObjectAsAlias

/********************************************************************************
	Send an Apple event to the Finder to get an alias as a finder object

	pAEDesc			==>		The finder object to get the alias of
	pAliasHdl		==>		An alias handle to which the objects alias will be set
	pIdleProcUPP	==>		A UPP for an idle function (required)
	
	See note about idle functions above.
*/
pascal OSErr MoreFEGetAliasAsObject(const AliasHandle pAliasHdl,AEDesc* pAEDesc,const AEIdleUPP pIdleProcUPP)
{
	AppleEvent tAppleEvent = {typeNull,NULL};	//	If you always init AEDescs, it's always safe to dispose of them.
	AEBuildError	tAEBuildError;
	DescType		tObjectType = cFile;
	OSErr anErr = noErr;

	// the alias handle, descriptor pointer and idle proc are required
	if ((NULL == pAliasHdl) || (NULL == pAEDesc) || (NULL == pIdleProcUPP))
		return paramErr;

	// Now we have to see if this is a file or a folder.
	{
		FSRef			tFSRef;
		FSCatalogInfo	tFSCatalogInfo;
		Boolean 		wasChanged;

		anErr = FSResolveAliasWithMountFlags(NULL,pAliasHdl,
					&tFSRef,&wasChanged,kResolveAliasFileNoUI);
		if (noErr != anErr)
			return anErr;

		anErr = FSGetCatalogInfo(&tFSRef,
					kFSCatInfoNodeFlags | kFSCatInfoParentDirID | kFSCatInfoNodeID,
					&tFSCatalogInfo,NULL,NULL,NULL);

		// if this is a container...
		if (kFSNodeIsDirectoryMask == (tFSCatalogInfo.nodeFlags & kFSNodeIsDirectoryMask))
		{	// if this is a volume...
			if ((tFSCatalogInfo.parentDirID == fsRtParID) && (tFSCatalogInfo.nodeID == fsRtDirID))
				tObjectType = cDisk;
			else	// ...otherwise it's a folder
				tObjectType = cFolder;
		}
	}

	anErr = AEBuildAppleEvent(
		        kAECoreSuite,kAEGetData,
				typeApplSignature,&gFinderSignature,sizeof(OSType),
		        kAutoGenerateReturnID,kAnyTransactionID,
		        &tAppleEvent,&tAEBuildError,
		        "'----':obj {form:alis,want:type(@),seld:'alis'(@@),from:null()}",
		        tObjectType,pAliasHdl);

	if (noErr == anErr)
	{
#if 0	// Set this true to printf the Apple Event before you send it.
		Handle strHdl;

		anErr = AEPrintDescToHandle(&tAppleEvent,&strHdl);
		if (noErr == anErr)
		{
			char	nul	= '\0';

			PtrAndHand(&nul,strHdl,1);
			printf("\n�MoreFEGetAliasAsObject: tAppleEvent=%s.",*strHdl); fflush(stdout);
			DisposeHandle(strHdl);
		}
#endif
			anErr = MoreAESendEventReturnAEDesc(pIdleProcUPP,&tAppleEvent,typeObjectSpecifier,pAEDesc);
		// always dispose of AEDescs when you are finished with them
		(void) MoreAEDisposeDesc(&tAppleEvent);
		}
	return anErr;
}	// end MoreFEGetAliasAsObject

/********************************************************************************
	Send an Apple event to the Finder to get an alias as a finder object

	pFSSpecPtr		==>		The item to get an object specifier for
	pAliasHdl		==>		An alias handle to which the objects alias will be set
	pIdleProcUPP	==>		A UPP for an idle function (required)
	
	See note about idle functions above.
*/
pascal OSErr MoreFEGetFSSpecAsObject(const FSSpecPtr pFSSpecPtr,AEDesc* pAEDesc,const AEIdleUPP pIdleProcUPP)
{
	OSErr			anErr = noErr;
	AliasHandle		tAliasHdl;
	
	anErr = NewAlias( NULL,pFSSpecPtr,&tAliasHdl);
	if ( noErr == anErr  &&  tAliasHdl == NULL )
		anErr = paramErr;
	else
	{
		anErr = MoreFEGetAliasAsObject(tAliasHdl,pAEDesc,pIdleProcUPP);
		DisposeHandle((Handle) tAliasHdl);
	}
	return anErr;
}	// end MoreFEGetAliasAsObject

/********************************************************************************
	Send an Apple event to the Finder to tell it to duplicate a file

	pFileFSSpecPtr		==>		The item to duplicate
	pFolderFSSpecPtr	==>		Where to duplicate it
	pWithReplacing		==>		Boolean with/without replacing
	pAEDescPtr			<==		the resulting object
	pIdleProcUPP		==>		A UPP for an idle function (required)
	
	See note about idle functions above.
*/
pascal OSErr MoreFEDuplicate(const FSSpecPtr pFileFSSpecPtr,const FSSpecPtr pFolderFSSpecPtr,const Boolean pWithReplacing,AEDesc* pAEDescPtr,const AEIdleUPP pIdleProcUPP)
{
	OSErr			anErr = paramErr;

	if ((NULL != pFileFSSpecPtr) && (NULL != pFolderFSSpecPtr) && (NULL != pIdleProcUPP) && (NULL != pAEDescPtr))
	{
		AEDesc 		fileObjDesc = {typeNull,NULL};

		anErr = MoreAEOCreateObjSpecifierFromFSSpec(pFileFSSpecPtr,&fileObjDesc);
		if (noErr == anErr)
		{
			AEDesc 		folderObjDesc = {typeNull,NULL};
			
			anErr = MoreAEOCreateObjSpecifierFromFSSpec(pFolderFSSpecPtr,&folderObjDesc);
			if (noErr == anErr)
			{
				AppleEvent		tAppleEvent = {typeNull,NULL};
				AEBuildError	tAEBuildError;
				DescType		boolDescType = (pWithReplacing ? typeTrue : typeFalse);

				anErr = AEBuildAppleEvent(
					        kAECoreSuite,'clon',
							typeApplSignature,&gFinderSignature,sizeof(OSType),
					        kAutoGenerateReturnID,kAnyTransactionID,
					        &tAppleEvent,&tAEBuildError,
					        "'----':(@),insh:(@),alrp:(@)",&fileObjDesc,&folderObjDesc,&boolDescType);

				if (noErr == anErr)
				{
					//	Send the event.
					anErr = MoreAESendEventReturnAEDesc(pIdleProcUPP,&tAppleEvent,typeWildCard,pAEDescPtr);
					// always dispose of AEDescs when you are finished with them
					(void) MoreAEDisposeDesc(&tAppleEvent);
				}
				(void) MoreAEDisposeDesc(&folderObjDesc);
			}
			(void) MoreAEDisposeDesc(&fileObjDesc);
		}
	}
	return anErr;
}	// end MoreFEDuplicate

/********************************************************************************
	Send an Apple event to the Finder to tell it to move a file

	pFileFSSpecPtr		==>		The item to duplicate
	pFolderFSSpecPtr	==>		Where to duplicate it
	pWithReplacing		==>		Boolean with/without replacing
	pAEDescPtr			<==		the resulting object
	pIdleProcUPP		==>		A UPP for an idle function (required)

	See note about idle functions above.
*/
pascal OSErr MoreFEMove(const FSSpecPtr pFileFSSpecPtr,const FSSpecPtr pFolderFSSpecPtr,const Boolean pWithReplacing,AEDesc* pAEDescPtr,const AEIdleUPP pIdleProcUPP)
{
	OSErr			anErr = paramErr;

	if ((NULL != pFileFSSpecPtr) && (NULL != pFolderFSSpecPtr) && (NULL != pIdleProcUPP) && (NULL != pAEDescPtr))
	{
		AEDesc 		fileObjDesc = {typeNull,NULL};
		
		anErr = MoreAEOCreateObjSpecifierFromFSSpec(pFileFSSpecPtr,&fileObjDesc);
		if (noErr == anErr)
		{
			AEDesc 		folderObjDesc = {typeNull,NULL};
			
			anErr = MoreAEOCreateObjSpecifierFromFSSpec(pFolderFSSpecPtr,&folderObjDesc);
			if (noErr == anErr)
			{
				AppleEvent		tAppleEvent = {typeNull,NULL};
				AEBuildError	tAEBuildError;
				DescType		boolDescType = (pWithReplacing ? typeTrue : typeFalse);

				anErr = AEBuildAppleEvent(
					        kAECoreSuite,kAEMove,
							typeApplSignature,&gFinderSignature,sizeof(OSType),
					        kAutoGenerateReturnID,kAnyTransactionID,
					        &tAppleEvent,&tAEBuildError,
					        "'----':(@),insh:(@),alrp:(@)",&fileObjDesc,&folderObjDesc,&boolDescType);

				if (noErr == anErr)
				{
					//	Send the event.
					anErr = MoreAESendEventReturnAEDesc(pIdleProcUPP,&tAppleEvent,typeWildCard,pAEDescPtr);
					// always dispose of AEDescs when you are finished with them
					(void) MoreAEDisposeDesc(&tAppleEvent);
				}
				(void) MoreAEDisposeDesc(&folderObjDesc);
			}
			(void) MoreAEDisposeDesc(&fileObjDesc);
		}
	}
	return anErr;
}	// end MoreFEMove

/********************************************************************************
	Send an Apple event to the Finder to tell it to empty the trash

	pIdleProcUPP		==>		A UPP for an idle function (required)

	See note about idle functions above.
*/
pascal OSErr MoreFEEmptyTrash(const AEIdleUPP pIdleProcUPP)
{
	OSErr			anErr = paramErr;

	if (NULL != pIdleProcUPP)
	{
		AppleEvent		tAppleEvent = {typeNull,NULL};
		AEBuildError	tAEBuildError;

		anErr = AEBuildAppleEvent(
			        kAEFinderSuite,kAEEmpty,
					typeApplSignature,&gFinderSignature,sizeof(OSType),
			        kAutoGenerateReturnID,kAnyTransactionID,
			        &tAppleEvent,&tAEBuildError,
			        "'----':obj {form:prop,want:typeprop),seld:type(trsh),from:'null'()}");
		if (noErr == anErr)
		{
			//	Send the event.
			anErr = MoreAESendEventNoReturnValue(pIdleProcUPP,&tAppleEvent);
			// always dispose of AEDescs when you are finished with them
			(void) MoreAEDisposeDesc(&tAppleEvent);
		}
	}
	return anErr;
}	// end MoreFEEmptyTrash

/********************************************************************************
	Send an Apple event to the Finder to tell it to make a list of objects visible

	pObjectList		==>		A list containing object descriptors
	pIdleProcUPP	==>		A UPP for an idle function (required)

	See note about idle functions above.
*/
pascal OSErr MoreFEMakeObjectsVisible(const AEDescList* pObjectList,const AEIdleUPP pIdleProcUPP)
{
	OSErr			anErr = paramErr;

	if (NULL != pIdleProcUPP)
	{
		AppleEvent		tAppleEvent = {typeNull,NULL};
		AEBuildError	tAEBuildError;

		anErr = AEBuildAppleEvent(
			        kAEMiscStandards,kAEMakeObjectsVisible,
					typeApplSignature,&gFinderSignature,sizeof(OSType),
			        kAutoGenerateReturnID,kAnyTransactionID,
			        &tAppleEvent,&tAEBuildError,
			        "'----':(@)",pObjectList);
		if (noErr == anErr)
		{
#if 0	// Set this true to printf the Apple Event before you send it.
		Handle strHdl;

		anErr = AEPrintDescToHandle(&tAppleEvent,&strHdl);
		if (noErr == anErr)
		{
			char	nul	= '\0';

			PtrAndHand(&nul,strHdl,1);
			printf("\n�MoreFEMakeObjectsVisible: tAppleEvent=%s.",*strHdl); fflush(stdout);
			DisposeHandle(strHdl);
		}
#endif
			//	Send the event.
			anErr = MoreAESendEventNoReturnValue(pIdleProcUPP,&tAppleEvent);
			// always dispose of AEDescs when you are finished with them
			(void) MoreAEDisposeDesc(&tAppleEvent);
		}
	}
	return anErr;
}	// end MoreFEMakeObjectsVisible

/********************************************************************************
	Send an Apple event to the Finder to ask it for it's windows

	pIdleProcUPP	==>		A UPP for an idle function (required)
	pObjectList		<==		A list containing window object descriptors

	See note about idle functions above.
*/
pascal OSErr MoreFEGetWindows(AEDescList* pWindowList,const AEIdleUPP pIdleProcUPP)
{
	OSErr			anErr = paramErr;

	if (NULL != pIdleProcUPP)
	{
		AppleEvent		tAppleEvent = {typeNull,NULL};
		AEBuildError	tAEBuildError;

		anErr = AEBuildAppleEvent(
			        kAECoreSuite,kAEGetData,
					typeApplSignature,&gFinderSignature,sizeof(OSType),
			        kAutoGenerateReturnID,kAnyTransactionID,
			        &tAppleEvent,&tAEBuildError,
			        "'----':obj {form:indx,want:type(cwin),seld:abso('all '),from:'null'()}");

		if (noErr == anErr)
		{
#if 0	// Set this true to printf the Apple Event before you send it.
		Handle strHdl;

		anErr = AEPrintDescToHandle(&tAppleEvent,&strHdl);
		if (noErr == anErr)
		{
			char	nul	= '\0';

			PtrAndHand(&nul,strHdl,1);
			printf("\n�MoreFEGetWindows: tAppleEvent=%s.",*strHdl); fflush(stdout);
			DisposeHandle(strHdl);
		}
#endif
			//	Send the event.
			anErr = MoreAESendEventReturnAEDescList(pIdleProcUPP,&tAppleEvent,pWindowList);
			// always dispose of AEDescs when you are finished with them
			(void) MoreAEDisposeDesc(&tAppleEvent);
		}
	}
	return anErr;
}

/********************************************************************************
	Send an Apple event to the Finder to ask it for a property of an object

	pObjectAEDesc	==>		An object descriptor
	pPropDescType	==>		the property type of the property we want
	pIdleProcUPP	==>		A UPP for an idle function (required)
	pResults		<==		An AEDesc containing the property

	See note about idle functions above.
*/
pascal OSErr MoreFEGetObjectProperty(AEDesc* pObjectAEDesc,const DescType pPropDescType,AEDesc* pResults,const AEIdleUPP pIdleProcUPP)
{
	OSErr			anErr = paramErr;

	if (NULL != pIdleProcUPP)
	{
		AppleEvent		tAppleEvent = {typeNull,NULL};
		AEBuildError	tAEBuildError;

		anErr = AEBuildAppleEvent(
			        kAECoreSuite,kAEGetData,
					typeApplSignature,&gFinderSignature,sizeof(OSType),
			        kAutoGenerateReturnID,kAnyTransactionID,
			        &tAppleEvent,&tAEBuildError,
			        "'----':obj {form:'prop',want:type('prop'),seld:type(@),from:(@)}",
			        pPropDescType,pObjectAEDesc);

		if (noErr == anErr)
		{
#if 0	// Set this true to printf the Apple Event before you send it.
			Handle strHdl;

			anErr = AEPrintDescToHandle(&tAppleEvent,&strHdl);
			if (noErr == anErr)
			{
				char	nul	= '\0';

				PtrAndHand(&nul,strHdl,1);
				printf("\n�MoreFEGetObjectProperty: tAppleEvent=%s.",*strHdl); fflush(stdout);
				DisposeHandle(strHdl);
			}
#endif
			//	Send the event.
			anErr = MoreAESendEventReturnAEDesc(pIdleProcUPP,&tAppleEvent,typeWildCard,pResults);
			// always dispose of AEDescs when you are finished with them
			(void) MoreAEDisposeDesc(&tAppleEvent);

#if 0	// Set this true to printf the results you get back
			{
				Handle strHdl;

				anErr = AEPrintDescToHandle(pResults,&strHdl);
				if (noErr == anErr)
				{
					char	nul	= '\0';

					PtrAndHand(&nul,strHdl,1);
					printf("\n�MoreFEGetObjectProperty: results = %s.",*strHdl); fflush(stdout);
					DisposeHandle(strHdl);
				}
			}
#endif
		}
	}
	return anErr;
}

/********************************************************************************
	Send an Apple event to the Finder to ask it to restart the machine.

	pIdleProcUPP	==>		A UPP for an idle function (required)

	See note about idle functions above.
*/
pascal OSErr MoreFERestart(const AEIdleUPP pIdleProcUPP)
{
	OSErr			anErr = paramErr;

	if (NULL != pIdleProcUPP)
	{
		AppleEvent		tAppleEvent = {typeNull,NULL};
		AEBuildError	tAEBuildError;
#if 1
		{
			ProcessSerialNumber systemPSN = {0,kSystemProcess};
			anErr = AEBuildAppleEvent(
				        typeAppleEvent,kAERestart,
						typeProcessSerialNumber,&systemPSN,sizeof(ProcessSerialNumber),
				        kAutoGenerateReturnID,kAnyTransactionID,
				        &tAppleEvent,&tAEBuildError,"");
		}
#else
		anErr = AEBuildAppleEvent(
			        kAEFinderSuite,kAERestart,
					typeApplSignature,&gFinderSignature,sizeof(OSType),
			        kAutoGenerateReturnID,kAnyTransactionID,
			        &tAppleEvent,&tAEBuildError,"");
#endif
		if (noErr == anErr)
		{
#if 0	// Set this true to printf the Apple Event before you send it.
			Handle strHdl;

			anErr = AEPrintDescToHandle(&tAppleEvent,&strHdl);
			if (noErr == anErr)
			{
				char	nul	= '\0';

				PtrAndHand(&nul,strHdl,1);
				printf("\n�MoreFERestart: tAppleEvent=%s.",*strHdl); fflush(stdout);
				DisposeHandle(strHdl);
			}
#endif
			//	Send the event.
			anErr = MoreAESendEventNoReturnValue(pIdleProcUPP,&tAppleEvent);
			// always dispose of AEDescs when you are finished with them
			(void) MoreAEDisposeDesc(&tAppleEvent);
		}
	}
	return anErr;
}

/********************************************************************************
	Send an Apple event to the Finder to make it active.

	pIdleProcUPP	==>		A UPP for an idle function (required)

	See note about idle functions above.
*/
pascal OSErr MoreFEActivate(const AEIdleUPP pIdleProcUPP)
{
	OSErr			anErr = paramErr;

	if (NULL != pIdleProcUPP)
	{
		AppleEvent		tAppleEvent = {typeNull,NULL};
		AEBuildError	tAEBuildError;

		anErr = AEBuildAppleEvent(
			        kAEMiscStandards,kAEActivate,
					typeApplSignature,&gFinderSignature,sizeof(OSType),
			        kAutoGenerateReturnID,kAnyTransactionID,
			        &tAppleEvent,&tAEBuildError,"");

		if (noErr == anErr)
		{
			//	Send the event.
			anErr = MoreAESendEventNoReturnValue(pIdleProcUPP,&tAppleEvent);
			// always dispose of AEDescs when you are finished with them
			(void) MoreAEDisposeDesc(&tAppleEvent);
		}
	}
	return anErr;
}
																																																																																																																																																																																																																																																																																																																																										
