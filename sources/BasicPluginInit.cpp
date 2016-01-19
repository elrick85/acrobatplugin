/*********************************************************************

 ADOBE SYSTEMS INCORPORATED
 Copyright (C) 1998-2006 Adobe Systems Incorporated
 All rights reserved.

 NOTICE: Adobe permits you to use, modify, and distribute this file
 in accordance with the terms of the Adobe license agreement
 accompanying it. If you have received this file from a source other
 than Adobe, then your use, modification, or distribution of it
 requires the prior written permission of Adobe.

 -------------------------------------------------------------------*/
/**
 \file BasicPluginInit.cpp

 - Skeleton .cpp file for a plug-in. It implements the basic
 handshaking methods required for all plug-ins.
 - A convenient function is provided to add a menu item easily.

 *********************************************************************/

// Acrobat Headers.
#ifndef MAC_PLATFORM
#include "PIHeaders.h"
#endif

/*-------------------------------------------------------
	Constants/Declarations
	-------------------------------------------------------*/
// stuff for Menu set up 
static AVMenuItem menuItem = NULL;
ACCB1 ASBool ACCB2 PluginMenuItem(void);
void AddMenuItemExt(AVMenu commonMenu, char * title, char * name, AVExecuteProc command, AVComputeEnabledProc enable);

void AddSeparator(AVMenu commonMenu, char * name);

// callback functions implemented in file "BasicPlugin.cpp"
extern ACCB1 void ACCB2 ValidationMockCommand(void *clientData);
extern ACCB1 void ACCB2 ValidationRunCommand(void *clientData);
extern ACCB1 void ACCB2 ValidationResetCommand(void *clientData);
extern ACCB1 void ACCB2 PrepareFileForSendingCommand(void *clientData);
extern ACCB1 void ACCB2 ChangeFontsToHelvCommand(void *clientData);
extern ACCB1 void ACCB2 ChangeCalcOrderCommand(void *clientData);
extern ACCB1 void ACCB2 CheckAllPrintCheckboxesCommand(void *clientData);
extern ACCB1 void ACCB2 UncheckAllPrintCheckboxesCommand(void *clientData);
extern ACCB1 void ACCB2 ImportFDFCommand(void *clientData);
extern ACCB1 void ACCB2 ExportAllAsFDFCommand(void *clientData);
extern ACCB1 void ACCB2 ExportPageAsFDF(void *clientData);

extern ACCB1 void ACCB2 VisitOneSourcePageCommand(void *clientData);
extern ACCB1 void ACCB2 RunExternalCommand(void *clientData);

extern ACCB1 ASBool ACCB2 ValidationIsEnabled(void *clientData);
extern ACCB1 ASBool ACCB2 CommonIsEnabled(void *clientData);
extern ACCB1 ASBool ACCB2 AlwaysEnabled(void *clientData);

extern ACCB1 ASBool ACCB2 MyPluginSetmenu();

extern const char* MyPluginExtensionName;

/*-------------------------------------------------------
	Core Handshake Callbacks
	-------------------------------------------------------*/

/**
	Callback invoked by the application to give the plug-in an opportunity to register
	an HFTServer with the application.
	@return true to indicate the plug-in should continue loading.
	*/
ACCB1 ASBool ACCB2 PluginExportHFTs(void)
{
	return true;
}

/**
	The application calls this function to allow it to
	<ul>
	<li> Import plug-in supplied HFTs.
	<li> Replace functions in the HFTs you're using (where allowed).
	<li> Register to receive notification events.
	</ul>
	*/
ACCB1 ASBool ACCB2 PluginImportReplaceAndRegister(void)
{
	return true;
}

/**
	The main initialization routine.
	We register our action handler with the application.
	@return true to continue loading the plug-in
	@return false to cause plug-in loading to stop.
	*/
/* PluginInit
** ------------------------------------------------------
**
** The main initialization routine.
**
** Return true to continue loading plug-in.
** Return false to cause plug-in loading to stop.
*/
ACCB1 ASBool ACCB2 PluginInit(void)
{
	return MyPluginSetmenu();
}

/**
	The unload routine.
	Called when your plug-in is being unloaded when the application quits.
	Use this routine to release any system resources you may have
	allocated.

	Returning false will cause an alert to display that unloading failed.
	@return true to indicate the plug-in unloaded.
	*/
ACCB1 ASBool ACCB2 PluginUnload(void)
{
	if (menuItem)
		AVMenuItemRemove(menuItem);

	return true;
}

/**
	Return the unique ASAtom associated with your plug-in.
	@return the plug-ins name as an ASAtom.
	*/
ASAtom GetExtensionName()
{
	return ASAtomFromString(MyPluginExtensionName);
}


/**
	Function that provides the initial interface between your plug-in and the application.
	This function provides the callback functions to the application that allow it to
	register the plug-in with the application environment.

	Required Plug-in handshaking routine: <b>Do not change it's name!</b>

	@param handshakeVersion the version this plug-in works with. There are two versions possible, the plug-in version
	and the application version. The application calls the main entry point for this plug-in with its version.
	The main entry point will call this function with the version that is earliest.
	@param handshakeData OUT the data structure used to provide the primary entry points for the plug-in. These
	entry points are used in registering the plug-in with the application and allowing the plug-in to register for
	other plug-in services and offer its own.
	@return true to indicate success, false otherwise (the plug-in will not load).
	*/
ACCB1 ASBool ACCB2 PIHandshake(Uns32 handshakeVersion, void *handshakeData)
{
	if (handshakeVersion == HANDSHAKE_V0200) {
		/* Cast handshakeData to the appropriate type */
		PIHandshakeData_V0200 *hsData = (PIHandshakeData_V0200 *)handshakeData;

		/* Set the name we want to go by */
		hsData->extensionName = GetExtensionName();

		/* If you export your own HFT, do so in here */
		hsData->exportHFTsCallback = (void*)ASCallbackCreateProto(PIExportHFTsProcType, &PluginExportHFTs);

		/*
		** If you import plug-in HFTs, replace functionality, and/or want to register for notifications before
		** the user has a chance to do anything, do so in here.
		*/
		hsData->importReplaceAndRegisterCallback = (void*)ASCallbackCreateProto(PIImportReplaceAndRegisterProcType,
			&PluginImportReplaceAndRegister);

		/* Perform your plug-in's initialization in here */
		hsData->initCallback = (void*)ASCallbackCreateProto(PIInitProcType, &PluginInit);

		/* Perform any memory freeing or state saving on "quit" in here */
		hsData->unloadCallback = (void*)ASCallbackCreateProto(PIUnloadProcType, &PluginUnload);

		/* All done */
		return true;

	} /* Each time the handshake version changes, add a new "else if" branch */

	/*
	** If we reach here, then we were passed a handshake version number we don't know about.
	** This shouldn't ever happen since our main() routine chose the version number.
	*/
	return false;
}

/*-------------------------------------------------------
	Menu Utility
	-------------------------------------------------------*/

/**
	A convenient function to add a menu item under Acrobat SDK menu.
	@return true if successful, false if failed.
	@see AVAppGetMenubar
	@see AVMenuItemNew
	@see AVMenuItemSetExecuteProc
	@see AVMenuItemSetComputeEnabledProc
	@see AVMenubarAcquireMenuItemByName
	@see AVMenubarAcquireMenuByName
	*/
ACCB1 ASBool ACCB2 PluginMenuItem()
{
	AVMenubar menubar = AVAppGetMenubar();
	AVMenu volatile commonMenu = NULL;
		
	if (!menubar){
		return false;
	}

	DURING

	commonMenu = AVMenubarAcquireMenuByName(menubar, "ADBE:PerkinElmer");

	// if "Acrobat SDK" menu is not existing, create one.
	if (!commonMenu) {
		commonMenu = AVMenuNew("PerkinElmer", "ADBE:PerkinElmer", gExtensionID);
		AVMenubarAddMenu(menubar, commonMenu, APPEND_MENU);
	}

	AddMenuItemExt(commonMenu, "Validation->Mock", "ADBE:Validation_Mock", ValidationMockCommand, ValidationIsEnabled);
	AddMenuItemExt(commonMenu, "Validation->Run", "ADBE:Validation_Run", ValidationRunCommand, ValidationIsEnabled);
	AddMenuItemExt(commonMenu, "Validation->Reset", "ADBE:Validation_Reset", ValidationResetCommand, ValidationIsEnabled);

	AddSeparator(commonMenu, "ADBE:separator1");

	AddMenuItemExt(commonMenu, "Reset before publish", "ADBE:Reset_before_publish", PrepareFileForSendingCommand, CommonIsEnabled);
	AddMenuItemExt(commonMenu, "Change All Fonts to Helvetica", "ADBE:Change_Fonts", ChangeFontsToHelvCommand, CommonIsEnabled);
	AddMenuItemExt(commonMenu, "Fix Calc Order", "ADBE:Fix_Calc_Order", ChangeCalcOrderCommand, CommonIsEnabled);
	AddMenuItemExt(commonMenu, "Check Print Checkboxes", "ADBE:Check_Print_Checkboxes", CheckAllPrintCheckboxesCommand, CommonIsEnabled);
	AddMenuItemExt(commonMenu, "Uncheck Print Checkboxes", "ADBE:Uncheck_Print_Checkboxes", UncheckAllPrintCheckboxesCommand, CommonIsEnabled);

	AddSeparator(commonMenu, "ADBE:separator3");

	AddMenuItemExt(commonMenu, "fdf->import", "ADBE:Import_FDF", ImportFDFCommand, CommonIsEnabled);
	AddMenuItemExt(commonMenu, "fdf->export all", "ADBE:Export_FDF", ExportAllAsFDFCommand, CommonIsEnabled);
	AddMenuItemExt(commonMenu, "fdf->export page", "ADBE:Export_Page_FDF", ExportPageAsFDF, CommonIsEnabled);

	AddSeparator(commonMenu, "ADBE:separator2");

	AddMenuItemExt(commonMenu, "Visit OneSource Page", "ADBE:Visit_OneSource_Page", VisitOneSourcePageCommand, AlwaysEnabled);

	AddSeparator(commonMenu, "ADBE:separator4");

	AddMenuItemExt(commonMenu, "Run External", "ADBE:Run_External", RunExternalCommand, AlwaysEnabled);

	AVMenuRelease(commonMenu);

	HANDLER
		if (commonMenu){
			AVMenuRelease(commonMenu);
		}

		return false;
	END_HANDLER

	return true;
}

void AddMenuItemExt(AVMenu commonMenu, char * title, char * name, AVExecuteProc command, AVComputeEnabledProc enable){
	menuItem = AVMenuItemNew(title, name, NULL, true, NO_SHORTCUT, 0, NULL, gExtensionID);

	AVMenuItemSetExecuteProc(menuItem, ASCallbackCreateProto(AVExecuteProc, command), NULL);
	AVMenuItemSetComputeEnabledProc(menuItem, ASCallbackCreateProto(AVComputeEnabledProc, enable), (void *)pdPermEdit);

	AVMenuAddMenuItem(commonMenu, menuItem, APPEND_MENUITEM);
}

void AddSeparator(AVMenu commonMenu, char * name)
{
	menuItem = AVMenuItemNew("-", name, NULL, true, NO_SHORTCUT, 0, NULL, gExtensionID);
	AVMenuAddMenuItem(commonMenu, menuItem, APPEND_MENUITEM);
}