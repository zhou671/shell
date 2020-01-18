#include <vector>
#include <string>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <algorithm>
#include <regex>

void addstaff(std::string input, std::vector<std::string> *arr)
{
	input = std::string("bianle");
}


bool stringcomp(std::string str1, std::string str2)
{
	return str1.compare(str2);
}

int main()
{
	/*std::vector<std::string> * arr = new std::vector<std::string>();
	arr->push_back("fasdfadsgasd");

	arr->push_back("fasdfadsgasd");
	arr->push_back("fasdfadsgasd");
	arr->push_back("fasdfadsgasd");
	arr->push_back("fasdfadsgasd");
	arr->push_back("fasdfadsgasd");
	arr->push_back("fasdfadsgasd");
	std::sort(arr->begin(),arr->end(), stringcomp);
	for(auto & str: *arr)
	{
		printf("%s\n", str.c_str());
	}	
	DIR *d = opendir("test-shell");
	struct dirent * ent;
	while ( (ent = readdir(d))!= NULL) {
		
		printf("%s\n", ent->d_name);
	}
	closedir(d);*/
	std::string str = std::string("ashkka/");
	std::regex r(".*/");
	if(std::regex_match(str, r))
	{
		printf("OK\n");
	}
}
