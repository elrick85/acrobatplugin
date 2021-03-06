// testConsoleApp.cpp : Defines the entry point for the console application.
//

#include <string>
#include <iostream>
#include <windows.h>
#include "OutInfo.h"

#pragma warning( disable : 4800 ) // stupid warning about bool
#define BUFSIZE 4096
HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;
HANDLE g_hChildStd_ERR_Rd = NULL;
HANDLE g_hChildStd_ERR_Wr = NULL;

PROCESS_INFORMATION CreateChildProcess(char * commandLine);
void ReadFromPipe(PROCESS_INFORMATION);
OutInfo ReadFromPipeToObject(PROCESS_INFORMATION piProcInfo);
OutInfo RunProcess(char * command);

OutInfo RunProcess(char * command) {
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

	//ReadFromPipe(piProcInfo);
	OutInfo outInfo = ReadFromPipeToObject(piProcInfo);

	return outInfo;
}

// Create a child process that uses the previously created pipes
//  for STDERR and STDOUT.
PROCESS_INFORMATION CreateChildProcess(char * commandLine) {
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