// testConsoleApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "json\json.h"
#include <string>
#include <iostream>
#include <fstream>
#include <windows.h>
#include "OutInfo.h"
#include <direct.h>

#pragma warning( disable : 4800 ) // stupid warning about bool

#define BUFSIZE 4096
#define GetCurrentDir _getcwd

#import "..\WinLb\bin\Debug\WinLb.tlb" raw_interfaces_only

HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;
HANDLE g_hChildStd_ERR_Rd = NULL;
HANDLE g_hChildStd_ERR_Wr = NULL;

char * GetCurrentDirPath();
PROCESS_INFORMATION CreateChildProcess(wchar_t commandLine[]);
void ReadFromPipe(PROCESS_INFORMATION);
OutInfo ReadFromPipeToObject(PROCESS_INFORMATION piProcInfo);
OutInfo RunProcess(char * argv[], wchar_t command[]);

using namespace WinLb;

int _tmain(int argc, char * argv[])
{
	
	// Initialize COM.
	HRESULT hr = CoInitialize(NULL);

	// Create the interface pointer.
	ITestClassPtr pICalc(__uuidof(TestClass));

	std::wstring s1;
	BSTR ss = SysAllocStringLen(s1.data(), s1.size());

	pICalc->GetStr(&ss);

	std::wstring ws(ss, SysStringLen(ss));

	std::wcout << "_ss:" << ws << std::endl;

	/*Json::Value root;

	std::ifstream config_doc("D:\\Dev\\perkinelmer\\projects\\engine-gc\\package.json", std::ifstream::binary);
	config_doc >> root;

	Json::Value jName = root["name"];
	std::string name = jName.asString();

	std::cout << "name:" << name << std::endl;*/

	// The remaining open handles are cleaned up when this process terminates. 
	// To avoid resource leaks in a larger application, 
	//   close handles explicitly.

	// Uninitialize COM.
	CoUninitialize();

	return 0;
}

char * GetCurrentDirPath()
{
	char* buffer;

	if ((buffer = _getcwd(NULL, 0)) == NULL)
		return "";
	else
	{
		return buffer;
	}
}

OutInfo RunProcess(char * argv[], wchar_t command[]) {
	SECURITY_ATTRIBUTES sa;

	// Set the bInheritHandle flag so pipe handles are inherited. 
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	// Create a pipe for the child process's STDERR. 
	if (!CreatePipe(&g_hChildStd_ERR_Rd, &g_hChildStd_ERR_Wr, &sa, 0)) {
		exit(1);
	}

	// Ensure the read handle to the pipe for STDERR is not inherited.
	if (!SetHandleInformation(g_hChildStd_ERR_Rd, HANDLE_FLAG_INHERIT, 0)){
		exit(1);
	}

	// Create a pipe for the child process's STDOUT. 
	if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &sa, 0)) {
		exit(1);
	}

	// Ensure the read handle to the pipe for STDOUT is not inherited
	if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)){
		exit(1);
	}

	// Create the child process. 
	PROCESS_INFORMATION piProcInfo = CreateChildProcess(command);

	// Read from pipe that is the standard output for child process. 
	printf("\n->Contents of child process STDOUT:\n\n", argv[1]);

	//ReadFromPipe(piProcInfo);
	OutInfo outInfo = ReadFromPipeToObject(piProcInfo);

	return outInfo;
}

// Create a child process that uses the previously created pipes
//  for STDERR and STDOUT.
PROCESS_INFORMATION CreateChildProcess(wchar_t commandLine[]) {
	// Set the text I want to run

	PROCESS_INFORMATION piProcInfo;
	STARTUPINFO siStartInfo;
	bool bSuccess = FALSE;

	// Set up members of the PROCESS_INFORMATION structure. 
	ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

	// Set up members of the STARTUPINFO structure. 
	// This structure specifies the STDERR and STDOUT handles for redirection.
	ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.hStdError = g_hChildStd_ERR_Wr;
	siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

	// Create the child process.
	bSuccess = CreateProcess(
		NULL,
		commandLine,     // command line 
		NULL,          // process security attributes 
		NULL,          // primary thread security attributes 
		TRUE,          // handles are inherited 
		0,             // creation flags 
		NULL,          // use parent's environment 
		NULL,          // use parent's current directory 
		&siStartInfo,  // STARTUPINFO pointer 
		&piProcInfo);  // receives PROCESS_INFORMATION

	CloseHandle(g_hChildStd_ERR_Wr);
	CloseHandle(g_hChildStd_OUT_Wr);

	// If an error occurs, exit the application. 
	if (!bSuccess) {
		exit(1);
	}

	return piProcInfo;
}

// Read output from the child process's pipe for STDOUT
// and write to the parent process's pipe for STDOUT. 
// Stop when there is no more data. 
OutInfo ReadFromPipeToObject(PROCESS_INFORMATION piProcInfo) {
	DWORD dwRead;
	CHAR chBuf[BUFSIZE];
	bool bSuccess = FALSE;

	std::string out = "", err = "";

	for (;;) {
		bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);

		if (!bSuccess || dwRead == 0)
		{
			break;
		}

		std::string s(chBuf, dwRead);
		out += s;
	}

	dwRead = 0;

	for (;;) {
		bSuccess = ReadFile(g_hChildStd_ERR_Rd, chBuf, BUFSIZE, &dwRead, NULL);

		if (!bSuccess || dwRead == 0)
		{
			break;
		}

		std::string s(chBuf, dwRead);
		err += s;
	}

	OutInfo info;
	info.out = out;
	info.err = err;

	return info;
	//std::cout << "stdout:" << out << std::endl;
	//std::cout << "stderr:" << err << std::endl;
}

// Read output from the child process's pipe for STDOUT
// and write to the parent process's pipe for STDOUT. 
// Stop when there is no more data. 
void ReadFromPipe(PROCESS_INFORMATION piProcInfo) {
	DWORD dwRead;
	CHAR chBuf[BUFSIZE];
	bool bSuccess = FALSE;

	std::string out = "", err = "";

	for (;;) {
		bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);

		if (!bSuccess || dwRead == 0)
		{
			break;
		}

		std::string s(chBuf, dwRead);
		out += s;
	}

	dwRead = 0;

	for (;;) {
		bSuccess = ReadFile(g_hChildStd_ERR_Rd, chBuf, BUFSIZE, &dwRead, NULL);

		if (!bSuccess || dwRead == 0)
		{
			break;
		}

		std::string s(chBuf, dwRead);
		err += s;
	}
	std::cout << "stdout:" << out << std::endl;
	std::cout << "stderr:" << err << std::endl;
}