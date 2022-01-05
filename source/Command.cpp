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

void command_base::reset(){
	args.clear();
	num_args = 0;
	reply.clear();
	error  = 0;
	prefix.clear();
}

//#############################################//


std::string commNick::exec(users_map &users, channels_map &channels_map, void *parent){
	Users *user = (Users*)parent;
	if (!user)
		return ("");
	if (args.size() != 2){
		reply = makeErrorMsg("NICK", 461 );
		error = 461;
		throw this;
	}
	else if (users.find(args[1]) != users.end()){
		reply = makeErrorMsg("NICK", 433);
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

std::string commNotFound::exec(users_map &users, channels_map &channels_map, void *parent){
	reply = makeErrorMsg(args[0], 421);
	return ("");
}
//##################################################//

commNonAuth::commNonAuth(std::string text): command_base(text) {
	
	(void)2;
}


std::string commNonAuth::exec(users_map &users, channels_map &channels_map, void *parent){
	reply = makeErrorMsg("AUTH", 451);
	return "";
}

//#############################################//

commPass::commPass(std::string text): command_base(text) {
}


std::string commPass::exec(users_map &users, channels_map &channels_map, void *parent){
	
	Users *user = (Users*)parent;
	if (!user)
		return ("");
	if (args.size() != 2){
		reply = makeErrorMsg("PASS", 461 );
		error = 461;
		throw this;
	}
	else if (user->getPasswd() != args[1]){
		reply = makeErrorMsg("PASS", 464);
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


std::string commUser::exec(users_map &users, channels_map &channels_map, void *parent){
	
	Users *user = (Users*)parent;
	if (!user)
		return ("");;
	if (args.size() != 5){
		reply = makeErrorMsg("USER", 461 );
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


//#############################################//

commPong::commPong(std::string text): command_base(text) {
}


std::string commPong::exec(users_map &users, channels_map &channels_map, void *parent){
	Users *user = (Users*)parent;
	if (!user)
		return ("");
	if (num_args != 2)
		reply = makeErrorMsg("PING", 461);
	else
		reply = "PONG " SERVER_NAME "\n";
	return "";
}


//#############################################//

commPrivMsg::commPrivMsg(std::string text): command_base(text) {


}


std::string commPrivMsg::exec(users_map &users, channels_map &channels_map, void *parent){
	Users *user = (Users*)parent;
	if (!user)
		return ("");
	users_map :: iterator f = users.find(args[1]);
	if (num_args != 3)
		reply = makeErrorMsg("PRIVMSG", 461);
	else if (f == users.end())
		reply = makeErrorMsg("PRIVMSG", 444);
	else if ( f->second != user)
		users[args[1]]->writeMessage( ":" +user->getNick() + "!" + user->getName()+ "@" + user->gethostIp() + " PRIVMSG " + args[1] + " :" + args[2]);
	else if ( f->second == user)
		reply = ( ":" +user->getNick() + "!" + user->getName()+ "@" + user->gethostIp() + " PRIVMSG " + args[1] + " :" + args[2] + CR LF) ;

	return "";
}