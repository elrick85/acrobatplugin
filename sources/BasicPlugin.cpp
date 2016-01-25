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
\file BasicPlugin.cpp

- This file implements the functionality of the BasicPlugin.
*********************************************************************/

#include <string>
#include "OutInfo.h"
#include <direct.h>

// Acrobat Headers.
#ifndef MAC_PLATFORM
#include "PIHeaders.h"
#endif

#include "FormsHFT.h"
HFT gAcroFormHFT = NULL;


/*-------------------------------------------------------
	Constants/Declarations
	-------------------------------------------------------*/
// This plug-in's name, you should specify your own unique name here.
#pragma  message ("Please specify your own UNIQUE plug-in name. Remove this message if you have already done so")
const char* MyPluginExtensionName = "ADBE:BasicPlugin";

/* A convenient function to add a menu item for your plugin.
*/
ACCB1 ASBool ACCB2 PluginMenuItem(void);
OutInfo RunProcess(char * command);
const char * GetCurrentDirPath();
char * GetCurrentDocPath();
void RunReportCommand(char * reportType, char * jsAction);

AVDoc currentDoc;
PDDoc currentPdfDoc;

/*-------------------------------------------------------
	Functions
	r-------------------------------------------------------*/

/* MyPluginSetmenu
** ------------------------------------------------------
**
** Function to set up menu for the plugin.
** It calls a convenient function PluginMenuItem.
** Return true if successful, false if failed.
*/
ACCB1 ASBool ACCB2 MyPluginSetmenu()
{
	// Add a new menu item under Acrobat SDK submenu.
	// The new menu item name is "ADBE:BasicPluginMenu", title is "Basic Plugin".
	// Of course, you can change it to your own.
	return PluginMenuItem();
}

/* AlwaysEnabled
** ------------------------------------------------------
*/
ACCB1 ASBool ACCB2 AlwaysEnabled(void *clientData)
{
	return true;
}

/* CommonIsEnabled
** ------------------------------------------------------
*/
ACCB1 ASBool ACCB2 CommonIsEnabled(void *clientData)
{
	return AVAppGetActiveDoc() != NULL;
}

/* ValidationIsEnabled
** ------------------------------------------------------
*/
ACCB1 ASBool ACCB2 ValidationIsEnabled(void *clientData)
{
	gAcroFormHFT = Init_AcroFormHFT;
	AVDoc avDoc = AVAppGetActiveDoc();

	if (avDoc == NULL) {
		return false;
	}

	PDDoc pdDoc = AVDocGetPDDoc(avDoc);
	char * dResult;

	ASBool  bRc = AFExecuteThisScript(pdDoc, "event.value = Boolean(this.tstdata).toString()", &dResult);

	ASBool vOut = dResult && stricmp(dResult, "true") == 0;

	return vOut;
}

void RunSimpleCommand(char * comandLine)
{
	// get this plugin's name for display
	char str[256] = "There is no PDF document loaded in Acrobat.";

	// try to get front PDF document 
	if (currentDoc == NULL){
		currentDoc = AVAppGetActiveDoc();
	}

	gAcroFormHFT = Init_AcroFormHFT;

	if (currentDoc == NULL) {
		AVAlertNote(str);
	}
	else {
		currentPdfDoc = AVDocGetPDDoc(currentDoc);
		ASBool bRc = AFExecuteThisScript(currentPdfDoc, comandLine, NULL);
	}
}

/* ValidationMockCommand
** ------------------------------------------------------
*/
ACCB1 void ACCB2 ValidationMockCommand(void *clientData)
{
	RunSimpleCommand("tstf.handler(this, 'mock')");
}

/* ValidationRunCommand
** ------------------------------------------------------
*/
ACCB1 void ACCB2 ValidationRunCommand(void *clientData)
{
	RunSimpleCommand("tstf.handler(this, 'run')");
}

/* ValidationResetCommand
** ------------------------------------------------------
*/
ACCB1 void ACCB2 ValidationResetCommand(void *clientData)
{
	RunSimpleCommand("tstf.handler(this, 'unmock')");
}

/* Reset before publish
** ------------------------------------------------------
*/
ACCB1 void ACCB2 PrepareFileForSendingCommand(void *clientData)
{
	RunSimpleCommand("Helper.prepareFileForSending()");
}

/* Change All Fonts to Helvetica
** ------------------------------------------------------
*/
ACCB1 void ACCB2 ChangeFontsToHelvCommand(void *clientData)
{
	RunSimpleCommand("changeAllFonts(this);");
}

/* Fix Calc Order
** ------------------------------------------------------
*/
ACCB1 void ACCB2 ChangeCalcOrderCommand(void *clientData)
{
	RunSimpleCommand("changeCalcOrder(this);");
}

/* Check Print Checkboxes
** ------------------------------------------------------
*/
ACCB1 void ACCB2 CheckAllPrintCheckboxesCommand(void *clientData)
{
	RunSimpleCommand("checkAllPrintCheckboxes(this);");
}

/* Uncheck Print Checkboxes
** ------------------------------------------------------
*/
ACCB1 void ACCB2 UncheckAllPrintCheckboxesCommand(void *clientData)
{
	RunSimpleCommand("uncheckAllPrintCheckboxes(this);");
}

/* Import FDF
** ------------------------------------------------------
*/
ACCB1 void ACCB2 ImportFDFCommand(void *clientData)
{
	RunSimpleCommand("Helper.importFDF();");
}

/* Export all data as FDF
** ------------------------------------------------------
*/
ACCB1 void ACCB2 ExportAllAsFDFCommand(void *clientData)
{
	RunSimpleCommand("exportAllAsFDF(this);");
}

/* Export page data as FDF
** ------------------------------------------------------
*/
ACCB1 void ACCB2 ExportPageAsFDF(void *clientData)
{
	RunSimpleCommand("exportPageAsFDF(this);");
}

/* Visit OneSource Page
** ------------------------------------------------------
*/
ACCB1 void ACCB2 VisitOneSourcePageCommand(void *clientData)
{
	ShellExecute(NULL, "open", "http://www.perkinelmer.com/onesource/", NULL, NULL, SW_SHOWNORMAL);
}

ACCB1 void ACCB2 GetFontReportCommand(void *clientData)
{
	char * scriptAction = "showWin(lm);";
	char * fType = "fonts";
	RunReportCommand(fType, scriptAction);
}

const char * GetCurrentDirPath()
{
	ASFileSys asfs;
	ASPathName asp;
	ASPathName rsp;

	ZeroMemory(&asfs, sizeof(ASFileSys));
	ZeroMemory(&asp, sizeof(ASPathName));

	AVAcquireSpecialFolderPathName(kAVSCApp, kAVSFPlugIns, false, &asfs, &asp);

	ASText asText = ASTextNew();
	
	ASFileSysDisplayASTextFromPath(asfs, asp, asText);

	ASScript bestScript = ASTextGetBestScript(asText, kASEUnicodeScript);
	const char * res = ASTextGetScriptText(asText, kASEUnicodeScript);
	return res;
	
	//return ASFileSysDIPathFromPath(asfs, asp, rsp);
}

char * GetCurrentDocPath()
{
	currentPdfDoc = AVDocGetPDDoc(currentDoc);

	ASFile fileinfo = PDDocGetFile(currentPdfDoc);
	ASFileSys fileSys = ASFileGetFileSys(fileinfo);
	ASPathName pathname = ASFileAcquirePathName(fileinfo);

	return ASFileSysDisplayStringFromPath(fileSys, pathname);
}

/* Run external process
** ------------------------------------------------------
*/
void RunReportCommand(char * reportType, char * jsAction)
{
	if (currentDoc == NULL) {
		currentDoc = AVAppGetActiveDoc();
	}

	if (currentDoc != NULL) {
		char scriptCommand[1000];
		char commandLine[1000];
		char* result = GetCurrentDocPath();

		std::string dpath(GetCurrentDirPath());

		std::string scriptManager = dpath + "\\BusicPlugin\\PdfTool.ScriptManager.exe";
		const char command[] = "%s -a %s -t %s -in \"%s\"";
		char * scriptAction = jsAction;

		sprintf(commandLine, command, scriptManager.c_str(), "report", reportType, result);

		OutInfo outInfo = RunProcess(commandLine);

		std::string sa(scriptAction);
		std::string vr = "eval('var lm = " + outInfo.out + ";');";
		std::string rs = vr + sa;

		RunSimpleCommand((char *)rs.c_str());
	}
}