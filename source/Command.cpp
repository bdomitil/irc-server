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
	else{
		if (users.find(user->getNick()) != users.end()){
			users.erase(user->getNick());
			users[args[1]] = user;
		}
		user->setNick(args[1]);
	}
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
	else if (user->isAuth()){
		reply = makeErrorMsg("", 462);
		error = 462;
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
		reply = "PONG " + args[1] + CR LF;
	return "";
}


//#############################################//

commPrivMsg::commPrivMsg(std::string text): command_base(text) {


}


std::string commPrivMsg::exec(users_map &users, channels_map &channels_map, void *parent){
	Users *user = (Users*)parent;
	if (!user)
		return ("");
	else if (num_args != 3)
		reply = makeErrorMsg(args[0], 461);
	else if (num_args == 3 && !args[2].size())
		reply = makeErrorMsg(args[0], 412);
	else if (prefix.size() ){
		if (args[0] == "PRIVMSG" && user->getAwayMsg().size() && args[1][0] != '#'){
			std::string sender = prefix_to_sender(prefix);
			if (users.find(sender) != users.end()){
				users[sender]->writeMessage(makeReplyHeader(SERVER_NAME, sender , 301) + user->getNick() + " :" + user->getAwayMsg());
			}
		}
		reply = request;
	}
	else if(args[1][0] != '#') {
		users_map :: iterator f = users.find(args[1]);
		if (f == users.end())
			reply = makeErrorMsg(args[0], 444);
		else if ( f->second != user)
			users[args[1]]->writeMessage(makeMessageHeader(user, args[0], args[1]) + args[2]);
	}
	else{
		channels_map::iterator f = channels_map.begin();
		if (f == channels_map.end())
			reply = makeErrorMsg(args[0], 401);
		else if (f->second->getFlags().test(CH_NO_OTHERS) && !f->second->isPart(user))
			reply = makeErrorMsg(args[0], 442);
		else if (f->second->getFlags().test(CH_MODERATED) && (!f->second->canVote(user) && !f->second->isOper(user)))
			reply = makeErrorMsg(f->second->getName(), 404);
		else{
			f->second->writeToUsers(makeMessageHeader(user, args[0], args[1]) + args[2] + CR LF, user);
		}
	}

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

//#############################################//

commMode::commMode(std::string text): command_base(text) {


}


std::string commMode::exec(users_map &users, channels_map &channels_map, void *parent){
	Users *user = (Users*)parent;
	if (prefix.size()){
		reply = request;
		return "";
	}
	if (!user)
		return ("");
	if (num_args < 2){
		reply = makeErrorMsg("MODE", 461);
		error = 461;
		return "";
	}
	if (args[1][0] == '#'){
		if (channels_map.find(args[1]) == channels_map.end())
			reply = makeErrorMsg(args[1], 403);
		else if (!channels_map[args[1]]->isPart(user))
			reply = makeErrorMsg(args[1], 442);
		else if (!channels_map[args[1]]->isOper(user))
			reply = makeErrorMsg(args[1], 482);
		else if (args[2][0] == '+' || args[2][0] == '-'){
			std::string ::iterator i = args[2].begin();
			i++;
			while (i != args[2].end()){

				if (*i == 'o'){
					if (args[2][0] == '+')
						channels_map[args[1]]->makeOper(users[args[3]]);
					else
						channels_map[args[1]]->dropOper(users[args[3]]);
				}else if (*i == 'p'){
					if (args[2][0] == '+')
						channels_map[args[1]]->getFlags().set(CH_PRIVATE);
					else
						channels_map[args[1]]->getFlags().reset(CH_PRIVATE);
				}else if (*i == 's'){
					if (args[2][0] == '+')
						channels_map[args[1]]->getFlags().set(CH_SECRET);
					else
						channels_map[args[1]]->getFlags().reset(CH_SECRET);
				}else if (*i == 'i'){
					if (args[2][0] == '+')
						channels_map[args[1]]->getFlags().set(CH_INVITE_ONLY);
					else
						channels_map[args[1]]->getFlags().reset(CH_INVITE_ONLY);
				}else if (*i == 't'){
					if (args[2][0] == '+')
						channels_map[args[1]]->getFlags().set(CH_TOPIC);
					else
						channels_map[args[1]]->getFlags().reset(CH_TOPIC);
				}else if (*i == 'n'){
					if (args[2][0] == '+')
						channels_map[args[1]]->getFlags().set(CH_NO_OTHERS);
					else
						channels_map[args[1]]->getFlags().reset(CH_NO_OTHERS);
				}else if (*i == 'm'){
					if (args[2][0] == '+')
						channels_map[args[1]]->getFlags().set(CH_MODERATED);
					else
						channels_map[args[1]]->getFlags().reset(CH_MODERATED);
				}else if (*i == 'l'){

					if (num_args != 4){
						reply = makeErrorMsg(args[1], 461);
						return "";
					}
					if (args[2][0] == '+')
						channels_map[args[1]]->setMax(atoi(args[3].c_str()));
					else
						channels_map[args[1]]->dropMaxUsers();
				}else if (*i == 'b'){
					if (num_args != 4){
						reply = makeErrorMsg(args[1], 461);
						return "";
				}
					if (args[2][0] == '+')
						channels_map[args[1]]->addBan(args[3]);
					else
						channels_map[args[1]]->dropBan(args[3]);
				}else if (*i == 'v'){

					if (num_args != 4){
						reply = makeErrorMsg(args[1], 461);
						return "";
					}else if (users.find(args[3]) == users.end()){
						reply = makeErrorMsg(args[1], 401);
						return "";
					}
					else if (args[2][0] == '+')
						channels_map[args[1]]->addVote(users[args[3]]);
					else
						channels_map[args[1]]->dropVote(users[args[3]]);
				}else if (*i == 'k'){

					if (num_args != 4){
						reply = makeErrorMsg(args[1], 461);
						return "";
					}else if (channels_map[args[1]]->getPassword().size()){
						reply = makeErrorMsg(args[1], 467);
						return "";
					}else if (args[2][0] == '+')
						channels_map[args[1]]->setPassword(args[3]);
					else
						channels_map[args[1]]->dropPassword();
				}
				i++;
			}
			if (!reply.size() && (args[2][0] == '+' || args[2][0] == '-') && num_args == 4){
				reply = makeMessageHeader(user, "MODE", args[1]);
				reply.pop_back();
				reply += args[2] + " "+ args[3] + CR LF;
				channels_map[args[1]]->writeToUsers(reply, user);
			}
			else if (!reply.size() && (args[2][0] == '+' || args[2][0] == '-')){
				reply = makeMessageHeader(user, "MODE", args[1]);
				reply.pop_back();
				reply += args[2] + CR LF;
				channels_map[args[1]]->writeToUsers(reply, user);
			}
		}
	}
	else {
		if ((args[2][0] != '+' && args[2][0] != '-')  || args[2].size() != 2)
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
		while (channels_map.size() && i != channels_map.end()){
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
	if (channels_map.find(args[1]) == channels_map.end() && args[1][0] != '#'){
		error = 403;
		reply = makeErrorMsg(args[1], 403);
	}
	else if (channels_map.find(args[1]) == channels_map.end()) {
		Channels *new_channel = new Channels(args[1], user);
		channels_map[args[1]] = new_channel;
		if (num_args == 3)
			new_channel->setPassword(args[2]);
		reply = makeMessageHeader(user, "JOIN", "") + args[1] + CR LF;
	}
	else if (channels_map.find(args[1])->second->isPart(user) && prefix != ":INVITE"){
		reply = request;
		return "";
	}
	else{
		Channels *channel  = channels_map[args[1]];
		if (channel->getPassword().size() && num_args != 3){
			error = 461;
			reply = makeErrorMsg("JOIN", 461);
			throw this;
		}else if (channel->getPassword().size() && channel->getPassword() != args[2] && prefix != ":INVITE"){
			error = 475;
			reply = makeErrorMsg(args[1], 475);
			throw this;
		}
		else if (channel->getMaxUsers() && channel->size() >= channel->getMaxUsers()){
			error = 471;
			reply = makeErrorMsg(args[1], 471);
			throw this;
		}
		else if (channel->getFlags().test(CH_INVITE_ONLY) && prefix != ":INVITE"){
			error = 473;
			reply = makeErrorMsg(args[1], 473);
			throw this;
		}
		else if (channel->isBaned(user->getNick())){
			error = 474;
			reply = makeErrorMsg(args[1], 474);
			throw this;
		}
		channel->addUser(user);
		reply = makeMessageHeader(user, "JOIN", "") + args[1] + CR LF;
	}
	channels_map[args[1]]->writeToUsers(reply, user);
	user->getMessage().addCommand(new commTopic( "WHO " + channels_map[args[1]]->getName()));
	user->getMessage().addCommand(new commNames("NAMES " + channels_map[args[1]]->getName()));
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


//#############################################//

commNames::commNames(std::string text): command_base(text) {


}


std::string commNames::exec(users_map &users, channels_map &channels_map, void *parent){
	Users *user = (Users*)parent;
	if (!user)
		return ("");
	if (num_args < 2){
		reply = makeErrorMsg("NAME", 461);
		error = 461;
		throw this;
	}else if(channels_map.size() && channels_map.find(args[1]) != channels_map.end() &&
	! channels_map[args[1]]->getFlags().test(CH_SECRET)	 && 
	! channels_map[args[1]]->getFlags().test(CH_PRIVATE) ){
			reply = channels_map[args[1]]->userNames(user);
	}
	reply += makeReplyHeader(SERVER_NAME, user->getNick(), 366) + channels_map[args[1]]->getName() +  " :End of /NAMES list" CR LF;
	return "";
}


//#############################################//

commQuit::commQuit(std::string text): command_base(text) {


}


std::string commQuit::exec(users_map &users, channels_map &channels_map, void *parent){
	Users *user = (Users*)parent;
	if (!user)
		return ("");
	if (num_args < 2){
		reply = makeErrorMsg("QUIT", 461);
		error = 461;
		throw this;
	}
	if (prefix.size())
		reply = request;
	else{
		channels_map::iterator i = channels_map.begin();
		close(user->getSocket());
		user->kill();
		users.erase(user->getNick());
		if(channels_map.size())
		for (; i != channels_map.end(); i++){
			if (i->second->isPart(user)){
				reply = makeMessageHeader(user, "QUIT", "") + args[1];
				i->second->writeToUsers(reply, user);
				i->second->dropUser(user);
				if (i->second->isDead()){
					delete i->second;
					channels_map.erase(i);
					i = channels_map.begin();
				}
			}
		}
	}
	return "";
}


//#############################################//

commMODT::commMODT(std::string text): command_base(text) {


}


std::string commMODT::exec(users_map &users, channels_map &channels_map, void *parent){
	Users *user = (Users*)parent;
	if (!user)
		return ("");
	reply = getMOTD(user->getNick());
	return "";
}


//#############################################//

commAway::commAway(std::string text): command_base(text) {


}


std::string commAway::exec(users_map &users, channels_map &channels_map, void *parent){
	Users *user = (Users*)parent;
	if (!user)
		return ("");
	if (prefix.size() && args[0] == "301")
		reply = request;
	else if (num_args == 1){
		user->getAwayMsg().clear();
		reply = makeReplyHeader(SERVER_NAME, user->getNick(), 305) + ":You are no longer marked as being away" CR LF;
	}
	else if (num_args > 1){
		user->setAwayMsg(args[1]);
		reply = makeReplyHeader(SERVER_NAME, user->getNick(), 306) + ":You have been marked as being away" CR LF;
	}
	return "";
}


//#############################################//

commInvite::commInvite(std::string text): command_base(text) {


}


std::string commInvite::exec(users_map &users, channels_map &channels_map, void *parent){
	Users *user = (Users*)parent;
	if (!user)
		return ("");
	if (prefix.size())
		reply = request;
	else if (num_args < 3)
		reply = makeErrorMsg(args[0], 461);
	else if (users.find(args[1]) == users.end() || channels_map.find(args[2]) == channels_map.end())
		reply = makeErrorMsg(args[2], 401);
	else if (!channels_map[args[2]]->isPart(user->getNick()))
		reply = makeErrorMsg(args[2], 442);
	else if (!channels_map[args[2]]->getFlags().test(CH_INVITE_ONLY) && !channels_map[args[2]]->isOper(user->getNick()))
		reply = makeErrorMsg(args[2], 482);
	else if (channels_map[args[2]]->isPart(args[1]))
		reply = makeErrorMsg(args[1] + " " + args[2], 443);
	else if (users[args[1]]->getAwayMsg().size())
		reply  = makeReplyHeader(SERVER_NAME, user->getNick(), 301) + users[args[1]]->getNick() + " :" +  users[args[1]]->getAwayMsg() + CR LF;
	else{
		channels_map[args[2]]->addUser(users[args[1]]);
		users[args[1]]->getMessage().addCommand(new commJoin(":INVITE JOIN " + args[2]));
		reply = makeReplyHeader(SERVER_NAME, user->getNick(), 341) + args[1] + " " + args[2] + CR LF;
		users[args[1]]->writeMessage(makeMessageHeader(user, args[0], args[1]) + args[2]);
	}
	return "";
}


//#############################################//

commKick::commKick(std::string text): command_base(text) {


}


std::string commKick::exec(users_map &users, channels_map &channels_map, void *parent){
	Users *user = (Users*)parent;
	if (!user)
		return ("");
	if (prefix.size())
		reply = request;
	else if (num_args < 3)
		reply = makeErrorMsg(args[0], 461);
	else if (channels_map.find(args[1]) == channels_map.end())
		reply = makeErrorMsg(args[1], 403);
	else if (!channels_map[args[1]]->isPart(user->getNick()))
		reply = makeErrorMsg(args[1], 442);
	else if (!channels_map[args[1]]->isOper(user->getNick()))
		reply = makeErrorMsg(args[1], 482);
	else{
		if (args[2][args[2].size() - 1] == ',')
			args[2].pop_back();
		if (num_args == 4)
			reply = makeMessageHeader(user, "KICK", args[1] + " " + args[2]) + args[3] + CR LF;
		else
			reply = makeMessageHeader(user, "KICK", args[1] + " " + args[2]) + CR LF;
		channels_map[args[1]]->writeToUsers(reply, user);
		channels_map[args[1]]->dropUser(args[2]);
		if (channels_map[args[1]]->isDead()){
			delete channels_map[args[1]];
			channels_map.erase(args[1]);
		}
	}
	return "";
}


//#############################################//

commPart::commPart(std::string text): command_base(text) {


}


std::string commPart::exec(users_map &users, channels_map &channels_map, void *parent){
	Users *user = (Users*)parent;
	if (!user)
		return ("");
	if (prefix.size())
		reply  = request;
	else if (num_args < 2)
		reply = makeErrorMsg(args[0], 461);
	else if (channels_map.find(args[1]) == channels_map.end())
		reply = makeErrorMsg(args[1], 403);
	else if (!channels_map[args[1]]->isPart(user->getNick()))
		reply = makeErrorMsg(args[1], 442);
	else{
		reply = makeMessageHeader(user, args[0], args[1]);
		reply.pop_back();
		reply.pop_back();
		reply += CR LF;
		channels_map[args[1]]->writeToUsers(reply, user);
		channels_map[args[1]]->dropUser(user);
		if (channels_map[args[1]]->isDead()){
			delete channels_map[args[1]];
			channels_map.erase(args[1]);
		}
	}
	return "";
}

//#############################################//

commList::commList(std::string text): command_base(text) {


}


std::string commList::exec(users_map &users, channels_map &channels_map, void *parent){
	Users *user = (Users*)parent;
	if (!user)
		return ("");
	reply = makeReplyHeader(SERVER_NAME, user->getNick(),321) + "Channel  :Users Name" CR LF;
	std::string end = makeReplyHeader(SERVER_NAME, user->getNick(),323) + "End of /LIST" CR LF;
	channels_map :: iterator i = channels_map.begin();
	if (num_args > 1)
		i = channels_map.find(args[1]);
	while (channels_map.size() && i != channels_map.end()){
		if (i->second->getFlags().test(CH_SECRET) && !i->second->isPart(user))
			(void)2;
		else if (i->second->getFlags().test(CH_PRIVATE) && !i->second->isPart(user))
			reply += makeReplyHeader(SERVER_NAME, user->getNick(),322) + \
				"Prv " + ft_itoa(i->second->size()) + " :[+n]" CR LF;
		else
			reply += makeReplyHeader(SERVER_NAME, user->getNick(),322) + \
				i->second->getName() + " " + ft_itoa(i->second->size()) + " :[+n] " + i->second->getTopic() + CR LF;
		if (num_args == 1)
			i++;
		else
			i = channels_map.end();
	}
	reply += end;
	return "";
}