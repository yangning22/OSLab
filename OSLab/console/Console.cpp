#include "Console.h"
#include "../log/Log.h"

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;


Console::Console() :memPool(10240, 20480, 32), procSys(memPool)
{

}


Console::~Console()
{

}

string Console::trim(string str)
{
	str.erase(0, str.find_first_not_of(" "));
	str.erase(str.find_last_not_of(" ") + 1);
	return str;
}

void Console::waitForInput()
{
	//TODO: mutex lock

	string tmp;
	cout << endl << workingDir << "$ ";
	getline(cin, tmp);

	tmp = trim(tmp);

	if (tmp != "")
		InputCut(tmp);

	//TODO: mutex unlock
}


void Console::InputCut(string input)
{
	stringstream stream;

	//put input string into a string stream
	//and then read out split string
	stream << input;
	string tmp;
	vector<string> argv;
	while (!stream.eof())
	{
		stream >> tmp;
		argv.push_back(tmp);
	}

	InputAnalyse(argv);
}

bool Console::cat(string filename)
{
	os_file* fp = Open_File(filename);
	if (!fp)
	{
		cout << "open file error\n" << endl;
		return false;
	}
	int fileSize = get_file_size(fp);
	char* dst = (char*)malloc(fileSize + 3);
	if (os_fread(dst, fileSize, fp))
	{
		dst[fileSize] = 0;
		cout << (char*)dst << endl;
		Close_File(fp);
		return true;
	}
	else
	{
		cout << "fread error" << endl;
		Close_File(fp);
		return false;
	}
}

bool Console::ls()
{
	vector<string> fileList = os_ls();
	for (auto i : fileList)
	{
		cout << i << endl;
	}
	cout << endl;
	return true;
}

bool Console::mkfile(string filename)
{
	if (!Create_File(filename, 1))
		return false;

	os_file* fp = Open_File(filename);
	if (!fp)
		return false;

	cout << "Please input the content of file: " << endl;
	string tmp;
	getline(cin, tmp);

	if (os_fwrite((char *)tmp.c_str(), tmp.size(), fp))
	{
		Close_File(fp);
		return true;
	}
	else
	{
		Close_File(fp);
		return false;
	}
}

bool Console::mkexec(string filename)
{
	if (!Create_File(filename, 1))
		return false;

	os_file* fp = Open_File(filename);
	if (!fp)
		return false;

	cout << "Please input the mem size exe will malloc: " << endl;
	int tmp;
	cin >> tmp;

	if (!os_fwrite(&tmp, 4, fp))
		return false;

	cout << "Please input the running time: " << endl;
	int tmp;
	cin >> tmp;

	if (!os_fwrite(&tmp, 4, fp))
		return false;

	cout << "Please input the size of data putted into memory: " << endl;
	int tmp;
	cin >> tmp;

	if (!os_fwrite(&tmp, 4, fp))
		return false;

	cout << "Please input the content of file: " << endl;
	string tmpData;
	getline(cin, tmpData);

	if (os_fwrite((void *)tmpData.c_str(), tmpData.size(), fp))
	{
		Close_File(fp);
		return true;
	}
	else
	{
		Close_File(fp);
		return false;
	}
}

void Console::InputAnalyse(vector<string> args)
{
	int inPos = 0;

	string command = args[0];
	//exit
	try
	{
		if (command == "exit")
			isExit = true;
		else if (command == "rmfile")
		{
			if (os_rm(args[1], workingDir))
				cout << "remove file successfully\n";
			else
				cout << "remove file error\n";
		}
		else if (command == "rmdir")
		{
			if (os_rmdir(args[1], workingDir))
				cout << "remove dir successfully\n";
			else
				cout << "remove dir error\n";
		}
		else if (command == "cd")
		{
			if (!os_cd(workingDir, args[1]))
				cout << "error filename\n";
		}
		else if (command == "mkdir")
		{
			if (Create_File(args[1], 0))
				cout << "mkdir successfully\n";
			else
				cout << "mkdir error\n";
		}
		else if (command == "ls")
		{
			ls();
		}
		else if (command == "pwd")
		{
			cout << workingDir << endl;
		}
		else if (command == "exec")
		{

		}
		else if (command == "kill")
		{

		}
		else if (command == "ps")
		{

		}
		else if (command == "cat")
		{
			if (!cat(args[1]))
				cout << "cat error\n";
		}
		else if (command == "mkfile")
		{
			if (mkfile(args[1]))
				cout << "mkfile successfully\n";
			else
				cout << "mkfile error\n";
		}
		else if (command == "mkexec")
		{
			if (mkexec(args[1]))
				cout << "mkfile successfully\n";
			else
				cout << "mkfile error\n";
		}
	}
	catch (const std::exception& e)
	{
		cout << "args error\n";
	}
}