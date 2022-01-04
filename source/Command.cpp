#include "../headers/irc-server.hpp"

std::string cutPrefix(std::string &text){
	strtrim(text);
	int i = 0;
	if (text[i++] != ':')
		return ("");
	
	while (!isspace(text[i]))
		i++;
	std::string ret = text.substr(0, i);
	text.erase(0, i);
	strtrim(text);
	return ret;
}

int getArgs(std::string &text, std::vector<std::string> &arg_store){
	int i = 0;
	int num = (bool)text.size();
	while (text[i] != ':' && text[i] != '\0'){
		if (isspace(text[i])  && text[i + 1] != '\0'){
			arg_store.insert(arg_store.end(),std::string(text.substr(0, i)));
			text.erase(0, i);
			strtrim(text);
			num++;
			i = 0;
		}
		else i++;
	}
	if (text[i] == ':')
		text.erase(i, 1);
	if (num && num != arg_store.size())
		arg_store.insert(arg_store.end(), std::string(text));
	text.erase(0, i);
	strtrim(text);
	return (num);
}


command_base::command_base(std::string text){
	prefix = cutPrefix(text);
	num_args = getArgs(text, args);
	error = false;
}


//#############################################//


std::string commNick::exec(users_map &users_map, channels_map &channels_map, void *parent){
	Users *user = (Users*)parent;
	if (args.size() != 2){
		error = 461;
		throw this;
	}
	else if (users_map.find(args[2]) != users_map.end()){
		error = 433;
		throw this;
	}
	else
		user->setNick(args[1]);
	return ("");
}


commNick::commNick(std::string text): command_base(text) {

}

//##################################################//

commNotFound::commNotFound(std::string text): command_base(text) {
}

std::string commNotFound::exec(users_map &users_map, channels_map &channels_map, void *parent){
	std::cerr << "COMMAND NOT FOUND" << std::endl;


	return ("");
}
//##################################################//

commNonAuth::commNonAuth(std::string text): command_base(text) {
	
	(void)2;
}


std::string commNonAuth::exec(users_map &users_map, channels_map &channels_map, void *parent){
	std::cerr << "NOT AUTHINTICATED" << std::endl;

	return "";
}

//#############################################//

commPass::commPass(std::string text): command_base(text) {
}


std::string commPass::exec(users_map &users_map, channels_map &channels_map, void *parent){
	
	Users *user = (Users*)parent;
	if (args.size() != 2){
		error = 461;
		throw this;
	}
	else if (user->getPasswd() != args[1]){
		error = 464;
		throw this;
	}
	else 
		user->setaPass(args[1]);
	return "";
}

//#############################################//

commUser::commUser(std::string text): command_base(text) {
}


std::string commUser::exec(users_map &users_map, channels_map &channels_map, void *parent){
	
	Users *user = (Users*)parent;
	if (args.size() != 5){
		error = 461;
		throw this;
	}
	else{ 
		user->setName(args[1]);
		user->setHostname(args[2]);
		user->setServername(args[3]);
		user->setRealname(args[4]);
	}
	return "";
}