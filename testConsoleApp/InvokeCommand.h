
class InvokeCommand
{
private:
	char * title;
	char * name;
	char * aType;
	char * command;
	char * jsAction;
	char * enableType;
public:
	InvokeCommand(char * p_title, char * p_name, char * p_aType, char * p_command, char * p_jsAction, char * p_enableType);
	void invoke();
};