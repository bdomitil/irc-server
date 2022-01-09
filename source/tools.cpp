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

void set_time(float seconds, struct timespec &tm){
	tm.tv_sec = static_cast<int>(seconds);
	tm.tv_nsec = (seconds - tm.tv_sec) * 1000000000;
} 

std::string	ft_itoa(int x)
{
	std::stringstream	buff;
	std::string		tmp;
	buff << x;
	buff >> tmp;

	return (tmp);
}

std::string getMOTD(std::string &target){
	std::string ret;
	ret += ":" SERVER_NAME " 375 " + target + " :- " SERVER_NAME " Message of the day - \n";
	ret += ":" SERVER_NAME " 372 " + target + " :- ############################################ \n";
	ret += ":" SERVER_NAME " 372 " + target + " :- ##                                        ## \n";
	ret += ":" SERVER_NAME " 372 " + target + " :- ##   IT IS ANY TEXT THAT U CANT IMAGINE   ## \n";
	ret += ":" SERVER_NAME " 372 " + target + " :- ##                                        ## \n";
	ret += ":" SERVER_NAME " 372 " + target + " :- ############################################ \n";
	ret += ":" SERVER_NAME " 376 " + target + " :End of /MOTD command\n";
	return ret;
}

command_base *genCommand(std::string text, bool auth){
	std::vector<std::string> tmpV;
	std::string command = text;
	cutPrefix(command);
	getArgs(command, tmpV);
	if (tmpV.size())
		command  = *tmpV.begin();

	if (auth){
		if (command == "NICK")
			return new commNick(text);
		if (command == "PING")
			return new commPong(text);
		else if (command == "PRIVMSG")
			return new commPrivMsg(text);
		else if (command == "OPER")
			return new commOper(text);
		else if (command == "MODE")
			return new commMode(text);
		else if (command == "WHO")
			return new commWho(text);
		else if (command == "JOIN")
			return new commJoin(text);
		else if (command == "TOPIC")
			return new commTopic(text);
		else if (command == "NAMES")
			return new commNames(text);
		else if (command == "QUIT")
			return new commQuit(text);
	}
	else{
		if (command == "NICK")
			return new commNick(text);
		else if (command == "PASS")
			return new commPass(text);
		else if (command == "USER")
			return new commUser(text);
		
		return new commNonAuth(text);
	}
	return (new commNotFound(text));
}

std::string makeErrorMsg(std::string info, int error){
	static std::map<int, std::string> Errors;
	if (!Errors.size()){
		Errors.insert(std::pair<int, std::string> (0, "UNKNOWN REPLY"        )  );
		Errors.insert(std::pair<int, std::string> (401,  " :No such nick/channel"        )  );
		Errors.insert(std::pair<int, std::string> (403,  " :No such channel"        )  );
		Errors.insert(std::pair<int, std::string> (404,  " :Cannot send to channel"        )  );
		Errors.insert(std::pair<int, std::string> (406,  " :There was no such nickname"        )  );
		Errors.insert(std::pair<int, std::string> (412, ":No text to send"        )  );
		Errors.insert(std::pair<int, std::string> (421, " :Unknown command"        )  );
		Errors.insert(std::pair<int, std::string> (431, ":No nickname given"        )  );
		Errors.insert(std::pair<int, std::string> (433, " :Nickname is already in use"        )  );
		Errors.insert(std::pair<int, std::string> (441, " :They aren't on that channel"       )  );
		Errors.insert(std::pair<int, std::string> (442, " :You're not on that channel"        )  );
		Errors.insert(std::pair<int, std::string> (443, " :is already on channel"        )  );
		Errors.insert(std::pair<int, std::string> (444, " :User not logged in"        )  );
		Errors.insert(std::pair<int, std::string> (451, ":You have not registered"        )  );
		Errors.insert(std::pair<int, std::string> (461, " :Not enough parameters"        )  );
		Errors.insert(std::pair<int, std::string> (464, ":Password incorrect"        )  );
		Errors.insert(std::pair<int, std::string> (467,  " :Channel key already set"        )  );
		Errors.insert(std::pair<int, std::string> (471,  " :Cannot join channel (+l)"        )  );
		Errors.insert(std::pair<int, std::string> (472,  " :is unknown mode char to me"        )  );
		Errors.insert(std::pair<int, std::string> (473,  " :Cannot join channel (+i)"        )  );
		Errors.insert(std::pair<int, std::string> (474,  " :Cannot join channel (+b)"        )  );
		Errors.insert(std::pair<int, std::string> (475,  " :Cannot join channel (+k)"        )  );
		Errors.insert(std::pair<int, std::string> (481,  ":Permission Denied- You're not an IRC operator"        )  );
		Errors.insert(std::pair<int, std::string> (482,  " :You're not channel operator"        )  );
		Errors.insert(std::pair<int, std::string> (501,  " :Unknown MODE flag"        )  );
		Errors.insert(std::pair<int, std::string> (502,  " :Cant change mode for other users"       )  );
	}
	std::map<int, std::string> :: iterator f = Errors.find(error);
	if (f != Errors.end()){
		return ( info  + Errors[error] + CR LF);
	}

	return Errors[0];
}

std::string makeMessageHeader(Users *sender, std::string messageType, std::string receiverNick){
	if (!sender)
		return (" " + messageType + " " + receiverNick + " :"  );
	else if (!receiverNick.size())
		return (":" + sender->getNick() + "!" +  sender->getName()+ "@" +  sender->gethostIp() + " " + messageType + " :");

	return (":" + sender->getNick() + "!" +  sender->getName()+ "@" +  sender->gethostIp() + " " + messageType + " " + receiverNick + " :");
}

std::string makeReplyHeader(std::string senderNick, std::string receiver, int code){
	return (":" + senderNick + " " + ft_itoa(code) + " " + receiver + " ");
}


std::time_t increase_session_time(){
	time_t curr = std::time(0);
	std::tm *tmp  = localtime(&curr);
	tmp->tm_sec += SESSION_END_SEC;
	tmp->tm_hour += 1;
	curr = std::mktime(tmp);
	return (curr);
}

std::string prefix_to_sender(std::string &prefix){
	int pos ;
	pos = prefix.find('!');
	if (pos != std::string::npos){
		return  prefix.substr(1, pos -1);
	}
	return prefix;
}