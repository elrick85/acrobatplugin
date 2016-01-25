#include "InvokeCommand.h";

extern void RunReportCommand(char * reportType, char * jsAction);

InvokeCommand::InvokeCommand(char * p_title, char * p_name, char * p_aType, char * p_command, char * p_jsAction, char * p_enableType)
{
	title = p_title;
	name = p_name;
	aType = p_aType;
	command = p_command;
	jsAction = p_jsAction;
	enableType = p_enableType;
}
void InvokeCommand::invoke(void *clientData)
{
	//char * scriptAction = "showWin(lm);";
	//char * fType = "fonts";
	RunReportCommand(command, jsAction);
}