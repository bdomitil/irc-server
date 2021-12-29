#include "../headers/irc-server.hpp"
std::string getKey(std::string &str){
	std::string tmp;
	if (str.find(':') != std::string::npos){
		tmp = str.substr(0, str.find(':'));
	}
	return tmp;
}

std::string getValue(std::string &str){
	std::string tmp;
	if (str.find(':') != std::string::npos){
		tmp = str.erase(0, str.find(':') + 1);
	}
	return tmp;
}

std::string strtrim(std::string &str){
	std::string:: iterator i = str.begin();
	while (i != str.end() && ((*i >= 0 && *i <= 32) || *i == 127)){
		str.erase(i);
		i = str.begin();
	}
	i = str.end() - 1;
	while (*i && i != str.begin() && ((*i >= 0 && *i <= 32) || *i == 127)){
		str.erase(i);
		i = str.end() - 1;
	}
	return str;
}