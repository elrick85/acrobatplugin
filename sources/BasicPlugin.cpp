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
#include "json.h"

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

void RunReportCommand(const char * reportType, const char * jsAction);
void RunRemoveCommand(const char * removeTarget);

Json::Value GetConfigFromFile();


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

/* Run simple js command
** ------------------------------------------------------
*/
void RunSimpleCommand(const char * comandLine)
{
	// get this plugin's name for display
	char str[256] = "There is no PDF document loaded in Acrobat.";

	AVDoc currentDoc = AVAppGetActiveDoc();
	PDDoc currentPdfDoc;

	gAcroFormHFT = Init_AcroFormHFT;

	if (currentDoc == NULL) {
		AVAlertNote(str);
	}
	else {
		currentPdfDoc = AVDocGetPDDoc(currentDoc);
		ASBool bRc = AFExecuteThisScript(currentPdfDoc, comandLine, NULL);
	}
}

/* Callback for menu items to run simple js command
** ------------------------------------------------------
*/
ACCB1 void ACCB2 RunJsCommand(void *clientData)
{
	std::string* sp = static_cast<std::string*>(clientData);
	std::string s = *sp;

	RunSimpleCommand(s.c_str());
}

/* Callback for menu items to visit web page
** ------------------------------------------------------
*/
ACCB1 void ACCB2 VisitWebPageCommand(void *clientData)
{
	std::string* sp = static_cast<std::string*>(clientData);
	std::string s = *sp;

	ShellExecute(NULL, "open", s.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

/* Callback for menu items to get report
** ------------------------------------------------------
*/
ACCB1 void ACCB2 GetReportCommand(void *clientData)
{
	Json::Value* sp = static_cast<Json::Value*>(clientData);
	Json::Value vl = *sp;

	std::string scriptAction = vl["action"].asString();
	std::string fType = vl["command"].asString();

	RunReportCommand(fType.c_str(), scriptAction.c_str());
}

/* Callback for menu items to remove some data.
** ------------------------------------------------------
*/
ACCB1 void ACCB2 GetRemoveCommand(void *clientData)
{
	std::string* sp = static_cast<std::string*>(clientData);
	std::string s = *sp;

	RunRemoveCommand(s.c_str());
}

/* Get plugin dir path
** ------------------------------------------------------
*/
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

	const char * res = ASTextGetScriptText(asText, kASEUnicodeScript);

	return res;
}

/* Get current doc path
** ------------------------------------------------------
*/
char * GetCurrentDocPath()
{
	AVDoc currentDoc = AVAppGetActiveDoc();

	ASFile fileinfo;
	ASFileSys fileSys;
	ASPathName pathname;

	if (currentDoc != NULL) {
		PDDoc currentPdfDoc = AVDocGetPDDoc(currentDoc);

		fileinfo = PDDocGetFile(currentPdfDoc);
		fileSys = ASFileGetFileSys(fileinfo);
		pathname = ASFileAcquirePathName(fileinfo);
	}

	return ASFileSysDisplayStringFromPath(fileSys, pathname);
}

char * GetTmpFilePath()
{
	DWORD dwRetVal = 0;

	char wcharPath[MAX_PATH];
	char filePath[MAX_PATH];

	dwRetVal = GetTempPath(MAX_PATH, wcharPath);

	if (dwRetVal > MAX_PATH || (dwRetVal == 0))
	{
		return "empty";
	}

	GetTempFileNameA(wcharPath, "PE_", 0, filePath);

	return filePath;
}

/* Run report process
** ------------------------------------------------------
*/
void RunReportCommand(const char * reportType, const char * jsAction)
{
	char scriptCommand[1000];
	char commandLine[1000];
	char* result = GetCurrentDocPath();

	std::string dpath(GetCurrentDirPath());

	std::string scriptManager = dpath + "\\PerkinElmer\\PdfTool.ScriptManager.exe";
	const char command[] = "%s -a %s -t %s -in \"%s\"";
	const char * scriptAction = jsAction;

	sprintf(commandLine, command, scriptManager.c_str(), "report", reportType, result);

	OutInfo outInfo = RunProcess(commandLine);

	std::string sa(scriptAction);
	std::string vr = "eval('var lm = " + outInfo.out + ";');";
	std::string rs = vr + sa;

	RunSimpleCommand((char *)rs.c_str());
}

void RunRemoveCommand(const char * removeTarget)
{
	AVDoc avDoc = AVAppGetActiveDoc();

	if (avDoc != NULL)
	{
		char scriptCommand[1000];
		char commandLine[1000];
		char _outFilePath[MAX_PATH];
		char _resultPath[MAX_PATH];

		char* result = GetCurrentDocPath();

		std::string dpath(GetCurrentDirPath());

		std::string scriptManager = dpath + "\\PerkinElmer\\PdfTool.StructureManager.exe";
		const char command[] = "%s -a %s -t %s -optim -in \"%s\" -out \"%s\"";

		char * outFilePath = GetTmpFilePath();

		sprintf(commandLine, command, scriptManager.c_str(), "remove", removeTarget, result, outFilePath);
		sprintf(_outFilePath, "%s", outFilePath);

		if (AVDocClose(avDoc, false) == false) {
			AVAlertNote("Document close cancelled");
		}
		else {
			OutInfo outInfo = RunProcess(commandLine);

			CopyFileA(_outFilePath, result, false);
			DWORD dw = GetLastError();

			char messageW[600];

			sprintf(messageW, "Task '%s' done, error code: '%d'", removeTarget, dw);

			ASFileSys fileSys = ASGetDefaultFileSys();
			const char * strPathFlag = "Cstring";
			ASPathName pathName = ASFileSysCreatePathName(fileSys, ASAtomFromString(strPathFlag), result, NULL);

			AVDocOpenFromFile(pathName, fileSys, NULL);

			AVAlertNote(messageW);
		}
	}
}