#include "../headers/irc-server.hpp"

Channels::Channels(std::string name, Users *user){
	_owner = user;
	_name = name;
	_size = 0;
	_flags.reset();
	_max_users = 0;
	_isDead = false;
	_users.reserve(100);
	addUser(user);
	makeOper(user);
}

void Channels::addUser(Users* user){
	if (!_users.size() || find_user(user) == _users.end()){
		std::pair<Users*, std::bitset<2> > new_user;
		new_user.first = user;
		new_user.second.reset();
		_users.push_back(new_user);
	}	
}

void Channels::makeOper(Users * user){
	std::vector< std::pair<Users*, std::bitset<2> > > :: iterator f = find_user(user);
	if (f != _users.end() && _users.size()){
		f->second.set(CH_OPERATOR);
	}
}

void Channels::dropOper(Users * user){
	std::vector< std::pair<Users*, std::bitset<2> > > :: iterator f = find_user(user);
	if (f != _users.end() && _users.size()){
		f->second.reset(CH_OPERATOR);
	}
}


void Channels::changeOwner(){
	if (_users.size() && !isOper(_users.begin()->first)){
		_owner = _users.begin()->first;
		this->makeOper(_users.begin()->first);
		_users.begin()->first->getMessage().addCommand(new commMode("MODE " + _name + " +o "  + _users.begin()->first->getNick()));
	}

}

void Channels::dropUser(Users *user){
	std::vector< std::pair<Users*, std::bitset<2> > > :: iterator f = find_user(user);
	if (f != _users.end() && _users.size()){
		_users.erase(f);
		if (_owner == user)
			changeOwner();
	}
	if (!_users.size())
		_isDead = true;
}

void Channels::dropUser(std::string nick){
	std::vector< std::pair<Users*, std::bitset<2> > > :: iterator f = find_user(nick);
	if (f != _users.end() && _users.size()){
		_users.erase(f);
		changeOwner();
	}
	if (!_users.size())
		_isDead = true;
}

bool Channels::isOper(Users *user){
	std::vector< std::pair<Users*, std::bitset<2> > > :: iterator f = find_user(user);
	if (f != _users.end() && _users.size()){
		return f->second.test(CH_OPERATOR);
	}
	return false;
}

bool Channels::isOper(std::string nick){
	std::vector< std::pair<Users*, std::bitset<2> > > :: iterator f = find_user(nick);
	if (f != _users.end() && _users.size()){
		return f->second.test(CH_OPERATOR);
	}
	return false;
}

bool Channels::isPart(Users *user){
	std::vector< std::pair<Users*, std::bitset<2> > > :: iterator f = find_user(user);
	if (f != _users.end() && _users.size())
		return true;
	return false;
}

bool Channels::isPart(std::string nick){
	std::vector< std::pair<Users*, std::bitset<2> > > :: iterator f = find_user(nick);
	if (f != _users.end() && _users.size())
		return true;
	return false;
}


void Channels::addBan(std::string nick){
	std::vector<std::string> :: iterator i = _bans.begin();
	if (_bans.size())
	while (i != _bans.end()){
		if (*i == nick)
			break;
		i++;
	}
	if (i == _bans.end())
		_bans.push_back(nick);
}

bool Channels::isBaned(Users *user){
	std::vector<std::string> :: iterator i = _bans.begin();
	if (_bans.size())
	while (i != _bans.end()){
		if (*i == user->getNick())
			return true;
		i++;
	}
	return (false);
}

bool Channels::isBaned(std::string nick){
	std::vector<std::string> :: iterator i = _bans.begin();
	if (_bans.size())
	while (i != _bans.end()){
		if (*i == nick)
			return true;
		i++;
	}
	return (false);
}

void Channels::setBan(std::string ban){
	if ( std::find(_bans.begin(), _bans.end(), ban)  != _bans.end())
		_bans.push_back(ban);
}

void Channels::dropBan(std::string ban){
	std::vector<std::string> :: iterator f = std::find(_bans.begin(), _bans.end(), ban);
	if (f != _bans.end())
		_bans.erase(f);
}

bool Channels::canVote(std::string nick){
	std::vector< std::pair<Users*, std::bitset<2> > > :: iterator f = find_user(nick);
	if (f == _users.end() && _users.size())
		return false;
	return f->second.test(CH_VOTE);
}

bool Channels::canVote(Users *user){
	std::vector< std::pair<Users*, std::bitset<2> > > :: iterator f = find_user(user);
	if (f == _users.end() && _users.size())
		return false;
	return f->second.test(CH_VOTE);
}

void Channels::addVote(Users *user){
	std::vector< std::pair<Users*, std::bitset<2> > > :: iterator f = find_user(user);
	if ( f != _users.end() && _users.size())
		f->second.set(CH_VOTE);
}

void Channels::dropVote(Users *user){
	std::vector< std::pair<Users*, std::bitset<2> > > :: iterator f = find_user(user);
	if ( f != _users.end() && _users.size())
		f->second.reset(CH_VOTE);
}

void Channels::writeToUsers(std::string text, Users *sender){
	std::vector< std::pair<Users*, std::bitset<2> > > :: iterator i = _users.begin();
	if (_users.size())
		while (i != _users.end()){
			if (i->first != sender)
				i->first->writeMessage(text);
			i++;
		}
}

std::string Channels::whoUsers(bool flag_O, bool flag_I, Users *user){
	std::vector< std::pair<Users*, std::bitset<2> > > :: iterator i = _users.begin();
	std::string result;
	if (_users.size())
	while (i != _users.end()){
		char q;
		if (i->first->isIRCoperator())
			q = '*';
		else if (isOper(i->first))
			q = '@';
		else 
			q = ' ';
		if (flag_O && flag_I){
			if (!i->first->getflags().test(U_I) && isOper(i->first)){
				result += makeReplyHeader(SERVER_NAME, user->getNick(), 352) \
				+ getName() + " " + i->first->getName() + " "+  i->first->getHostname() + " " + \
				+ SERVER_NAME " " + i->first->getNick() + " H" + q +  " :0 " + i->first->getRealName() + CR LF;
			}
		}
		else if (flag_O){
			if (isOper(i->first))
				result += makeReplyHeader(SERVER_NAME, user->getNick(), 352) \
				+ getName() + " " + i->first->getName() + " "+  i->first->getHostname() + " " + \
				+ SERVER_NAME " " + i->first->getNick() + " H" + q +  " :0 " + i->first->getRealName() + CR LF;
		}
		else if (flag_I){
			if (!i->first->getflags().test(U_I) && !isPart(user)){
				result += makeReplyHeader(SERVER_NAME, user->getNick(), 352) \
				+ getName() + " " + i->first->getName() + " "+  i->first->getHostname() + " " + \
				+ SERVER_NAME " " + i->first->getNick() + " H" + q +  " :0 " + i->first->getRealName() + CR LF;
			}
		}
		else{
			result += makeReplyHeader(SERVER_NAME, user->getNick(), 352) \
				+ getName() + " " + i->first->getName() + " "+  i->first->getHostname() + " " + \
				+ SERVER_NAME " " + i->first->getNick() + " H" + q +  " :0 " + i->first->getRealName() + CR LF;
		}
		i++;
	}
	return result;
}


std::string Channels::userNames(Users *user){
	std::vector< std::pair<Users*, std::bitset<2> > > :: iterator i = _users.begin();
	std::string result;
	if (i != _users.end() && _users.size())
		result = makeReplyHeader(SERVER_NAME, user->getNick(), 353) + "= " + getName() + " :";
	if (_users.size())
	while (i != _users.end()){
		if (isOper(i->first))
			result += "@" + i->first->getNick() + " ";
		else 
			result += i->first->getNick() + " ";
		i++;
	}
	if (result.size())
		result += CR LF;
	return result;
}

std::vector< std::pair<Users*, std::bitset<2> > > :: iterator Channels::find_user(Users *user ){
	std::vector< std::pair<Users*, std::bitset<2> > > :: iterator i = _users.begin();
	if(_users.size())
	while (i != _users.end()){
		 if (user == i->first)
		 	return i;
		i++;
	}
	return _users.end();
}

std::vector< std::pair<Users*, std::bitset<2> > > :: iterator Channels::find_user(std::string nick){
	std::vector< std::pair<Users*, std::bitset<2> > > :: iterator i = _users.begin();
	if(_users.size())
	while (i != _users.end()){
		 if (nick == i->first->getNick())
		 	return i;
		i++;
	}
	return _users.end();
}

