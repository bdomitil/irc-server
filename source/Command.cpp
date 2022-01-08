#include "../headers/irc-server.hpp"

extern std::map <std::string, std::string> *g_opers;

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
	request = text;
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
	std::cerr << reply << std::endl; //TODO debug
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
		users[args[1]]->writeMessage(makeMessageHeader(user, "PRIVMSG", args[1]) + args[2]);
	else if ( f->second == user)
		reply = prefix + " " + args[0] + " " + args[1] + " :" + args[2] + CR LF;

	return "";
}

//#############################################//

commOper::commOper(std::string text): command_base(text) {


}


std::string commOper::exec(users_map &users, channels_map &channels_map, void *parent){
	Users *user = (Users*)parent;
	if (!user)
		return ("");
	if (num_args != 3){
		reply = makeErrorMsg("OPER", 461);
		error = 461;
		throw this;
	}
	else if (g_opers->find(args[1]) != g_opers->end() && g_opers->find(args[1])->second == args[2])
		user->makeIRCoperator();
	else{
		reply = makeErrorMsg("", 464 );
		error = 461;
		throw this;
	}

	return "";
}

commMode::commMode(std::string text): command_base(text) {


}


std::string commMode::exec(users_map &users, channels_map &channels_map, void *parent){
	Users *user = (Users*)parent;
	if (!user)
		return ("");
	if (args[1][0] == '#'){

	}
	else {
		if (num_args != 3)
			reply = makeErrorMsg("MODE", 461);
		else if ((args[2][0] != '+' && args[2][0] != '-')  || args[2].size() != 2)
			reply = makeErrorMsg("MODE", 501);
		else if (args[1] != user->getNick())
			reply = makeErrorMsg("MODE", 502);
		std::bitset<4>&flags  = user->getflags();
		if (args[2][0] == '+'){
			if (args[2][1] == 's'){
				flags.set(U_S);
			}
			else if (args[2][1] == 'i'){
				flags.set(U_I);
			}
			else if (args[2][1] == 'o'){
				(void)"do nothing";
			}
			else if (args[2][1] == 'w'){
				flags.set(U_W);
			}
			else
				reply = makeErrorMsg("MODE", 501);
			if (!reply.size())
				reply = makeMessageHeader(user, "MODE", user->getNick()) + "+" + args[2][1] + CR LF;
		}	
		else if (args[2][0] == '-'){
			if (args[2][1] == 's'){
				flags.reset(U_S);
			}
			else if (args[2][1] == 'i'){
				flags.reset(U_I);
			}
			else if (args[2][1] == 'o'){
				flags.reset(U_O);
				user->_isIRCoperator = false;
			}
			else if (args[2][1] == 'w'){
				flags.reset(U_W);
			}
			else
				reply = makeErrorMsg("MODE", 501);
			if (!reply.size())
				reply = makeMessageHeader(user, "MODE", user->getNick()) + "-" + args[2][1] + CR LF;
		}
	}


	return "";
}


//#############################################//

commWho::commWho(std::string text): command_base(text) {
}


std::string commWho::exec(users_map &users, channels_map &channels_map, void *parent){
	Users *user = (Users*)parent;
	if (!user)
		return ("");
	if (num_args > 3 || num_args < 2){
		reply = makeErrorMsg("WHO", 461);
		throw this;
		return "";
	}
	else if (num_args == 2  && args[1][0] == '#' ){
		if (channels_map.find(args[1]) != channels_map.end())
			reply = channels_map[args[1]]->whoUsers(false, false, user);
	}
	else if (num_args == 3 && args[1][0] == '#' && args[2] == "0"){
		if (channels_map.find(args[1]) != channels_map.end())
			reply = channels_map[args[1]]->whoUsers(true, false, user);
	}
	else if (num_args == 2 && args[2] == "o"){
		if (channels_map.find(args[1]) != channels_map.end())
			reply = channels_map[args[1]]->whoUsers(true, true, user);
	}
	else if (args[1] == "*"){
		channels_map::iterator i = channels_map.begin();
		while (i != channels_map.end()){
			reply += i->second->whoUsers(false, true, user);
			i++;
		}
	}
	else if (users.find(args[1]) != users.end() ){
		char q;
		if (users[args[1]]->isIRCoperator())
			q = '*';
		else 
			q = ' ';
		reply = makeReplyHeader(SERVER_NAME, user->getNick(), 352) \
			+ users[args[1]]->getName() + " "+  users[args[1]]->getHostname() + " " + \
			 + SERVER_NAME " " + users[args[1]]->getNick() + " H" + q +  " :0 " + users[args[1]]->getRealName() + CR LF;
	}
	reply += makeReplyHeader(SERVER_NAME, user->getNick(), 315) + user->getNick() + " :End of /WHO list" CR LF;
	return "";
}


//#############################################//

commJoin::commJoin(std::string text): command_base(text) {
}


std::string commJoin::exec(users_map &users, channels_map &channels_map, void *parent){
	Users *user = (Users*)parent;
	if (!user)
		return ("");
	if (num_args > 3 || num_args < 2){
		reply = makeErrorMsg("JOIN", 461);
		throw this;
	}
	if (channels_map.find(args[1]) == channels_map.end() && (args[1][0] != '#' && args[1][0] != '&')){
		error = 403;
		reply = makeErrorMsg(args[1], 403);
	}
	else if (channels_map.find(args[1]) == channels_map.end()) {
		Channels *new_channel = new Channels(args[1], user);
		channels_map[args[1]] = new_channel;
		if (num_args == 3)
			new_channel->setPassword(args[2]);
		//call TOPIC
		//call NAMES
		reply = makeMessageHeader(user, "JOIN", "") + args[1] + CR LF;
	}
	else if (channels_map.find(args[1])->second->isPart(user)){
		reply = prefix + " " + args[0] + " :" + args[1] + CR LF;
		return "";
	}
	else{
		Channels *channel  = channels_map[args[1]];
		if (channel->getPassword().size() && num_args != 3){
			error = 461;
			reply = makeErrorMsg("JOIN", 461);
		}else if (channel->getPassword().size() && channel->getPassword() != args[2]){
			error = 475;
			reply = makeErrorMsg(args[1], 475);
		}
		else if (channel->getMaxUsers() && channel->size() >= channel->getMaxUsers()){
			error = 471;
			reply = makeErrorMsg(args[1], 471);
		}
		else if (channel->getFlags().test(CH_INVITE_ONLY)){
			error = 473;
			reply = makeErrorMsg(args[1], 473);
		}
		else if (channel->isBaned(user->getNick())){
			error = 474;
			reply = makeErrorMsg(args[1], 474);
		}
		channel->addUser(user);
		reply = makeMessageHeader(user, "JOIN", "") + args[1] + CR LF;
	}
	channels_map[args[1]]->writeToUsers(reply, user);
	user->getMessage().addCommand(new commTopic(request));
	return "";
}


//#############################################//

commTopic::commTopic(std::string text): command_base(text) {
}


std::string commTopic::exec(users_map &users, channels_map &channels_map, void *parent){
	Users *user = (Users*)parent;
	if (!user)
		return ("");
	if (num_args > 3 || num_args < 2){
		error = 461;
		reply = makeErrorMsg("TOPIC", 461);
	}else if (channels_map.find(args[1]) == channels_map.end() ||
				!channels_map[args[1]]->isPart(user)){
		error = 442;
		reply = makeErrorMsg(args[1], 442);
	}else if(channels_map[args[1]]->getFlags().test(CH_TOPIC) && !channels_map[args[1]]->isOper(user)){
		error = 482;
		reply = makeErrorMsg(args[1], 482);
	}else if (num_args == 3){
		channels_map[args[1]]->setTopic(args[2]);
		reply = makeReplyHeader(SERVER_NAME, user->getNick(), 332) + args[1] + " :" + channels_map[args[1]]->getTopic() + CR LF;
	} 
	else if (!channels_map[args[1]]->getTopic().size()){
		reply = makeReplyHeader(SERVER_NAME, user->getNick(), 331) + args[1] + " :No topic is set" CR LF;
	}
	else 
		reply = makeReplyHeader(SERVER_NAME, user->getNick(), 332) + args[1] + " :" + channels_map[args[1]]->getTopic() + CR LF;
		
	return "";
}
