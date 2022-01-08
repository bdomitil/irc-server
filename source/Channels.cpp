#include "../headers/irc-server.hpp"

Channels::Channels(std::string name, Users *user){
	_owner = user;
	_name = name;
	addUser(user);
	_size = 0;
	_flags.reset();
	_max_users = 0;
	_isDead = false;
}

void Channels::addUser(Users* user){
	std::pair<Users*, std::bitset<2> > new_user;
	new_user.first = user;
	new_user.second.reset();
	_users.push_back(new_user);
}

void Channels::makeOper(Users * user){
	std::vector< std::pair<Users*, std::bitset<2> > > :: iterator f = find_user(user);
	if (f != _users.end()){
		f->second.set(CH_OPERATOR);
	}
}

void Channels::dropOper(Users * user){
	std::vector< std::pair<Users*, std::bitset<2> > > :: iterator f = find_user(user);
	if (f != _users.end()){
		f->second.reset(CH_OPERATOR);
	}
}


void Channels::changeOwner(){
	if (_users.size())
		_owner = _users.begin()->first;
}

void Channels::dropUser(Users *user){
	std::vector< std::pair<Users*, std::bitset<2> > > :: iterator f = find_user(user);
	if (f != _users.end()){
		_users.erase(f);
		changeOwner();
	}
	if (!_users.size())
		_isDead = true;
}

void Channels::dropUser(std::string nick){
	std::vector< std::pair<Users*, std::bitset<2> > > :: iterator f = find_user(nick);
	if (f != _users.end()){
		_users.erase(f);
		changeOwner();
	}
	if (!_users.size())
		_isDead = true;
}

bool Channels::isOper(Users *user){
	std::vector< std::pair<Users*, std::bitset<2> > > :: iterator f = find_user(user);
	if (f != _users.end()){
		return f->second.test(CH_OPERATOR);
	}
	return false;
}

bool Channels::isOper(std::string nick){
	std::vector< std::pair<Users*, std::bitset<2> > > :: iterator f = find_user(nick);
	if (f != _users.end()){
		return f->second.test(CH_OPERATOR);
	}
	return false;
}

bool Channels::isPart(Users *user){
	std::vector< std::pair<Users*, std::bitset<2> > > :: iterator f = find_user(user);
	if (f != _users.end())
		return true;
	return false;
}

bool Channels::isPart(std::string nick){
	std::vector< std::pair<Users*, std::bitset<2> > > :: iterator f = find_user(nick);
	if (f != _users.end())
		return true;
	return false;
}


void Channels::addBan(std::string nick){
	std::vector<std::string> :: iterator i = _bans.begin();
	while (i != _bans.end()){
		if (*i == nick)
			break;
		i++;
	}
	if (i == _bans.end())
		_bans.push_back(nick);
}


void Channels::dropBan(std::string nick){
	std::vector<std::string> :: iterator i = _bans.begin();
	while (i != _bans.end()){
		if (*i == nick)
			break;
		i++;
	}
	if (i != _bans.end())
		_bans.erase(i);
}

bool Channels::isBaned(Users *user){
	std::vector<std::string> :: iterator i = _bans.begin();
	while (i != _bans.end()){
		if (*i == user->getNick())
			return true;
		i++;
	}
	return (false);
}

bool Channels::isBaned(std::string nick){
	std::vector<std::string> :: iterator i = _bans.begin();
	while (i != _bans.end()){
		if (*i == nick)
			return true;
		i++;
	}
	return (false);
}

void Channels::writeToUsers(std::string text, Users *sender){
	std::vector< std::pair<Users*, std::bitset<2> > > :: iterator i = _users.begin();
	while (i != _users.end()){
		if (i->first != sender)
			i->first->writeMessage(text);
		i++;
	}
}


std::vector< std::pair<Users*, std::bitset<2> > > :: iterator Channels::find_user(Users *user ){
	std::vector< std::pair<Users*, std::bitset<2> > > :: iterator i = _users.begin();
	while (i != _users.end()){
		 if (user == i->first)
		 	return i;
		i++;
	}
	return _users.end();
}

std::vector< std::pair<Users*, std::bitset<2> > > :: iterator Channels::find_user(std::string nick){
	std::vector< std::pair<Users*, std::bitset<2> > > :: iterator i = _users.begin();
	while (i != _users.end()){
		 if (nick == i->first->getNick())
		 	return i;
		i++;
	}
	return _users.end();
}

