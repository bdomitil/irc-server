
#include "../headers/irc-server.hpp"

Users::Users(int fd){
	_operations.reset();
	_socket = fd;
	_isAuth = false;
	_isIRCoperator = false;
}

Users::Users(int fd, t_event *event){
	_operations.reset();
	_socket = fd;
	_isAuth = false;
	_isIRCoperator = false;
	_event = event;
	event->addReadEvent(_socket, this);
	event->addWriteEvent(_socket, this);
	event->disableWriteEvent(_socket, this);
}

void Users::reset(){
	_isAuth = false;
	_host_ip.clear();
	_operations.reset();
	_nick.clear();
	_passwd.clear(); 
}

bool Users::auth(std::string passw){
	if (_apasswd.size() && _realname.size() && _nick.size())
	{
		_isAuth = true;
		return true;
	}
	return false;
}


void Users::getMessage(uint64_t readsize){
	message.readMessage(_socket, readsize);
	if (message.isRead()){
		std::cout << message.getMessage() << std::endl;
		_event->enableWriteEvent(_socket, this);
	}
}

void Users::sendMessage(users_map &users_map, channels_map &channels_map, std::string &passw){
	std::vector<std::string>&store = message._buffer.getStore();
	std::vector<std::string>::iterator i  = store.begin();
	while (i != store.end()){
		message.exec(users_map, channels_map, this);
	if (!_isAuth)
		if (auth(passw))
			users_map.insert(std::pair<std::string, Users*>(_nick, this));

		store.erase(i);
		std::vector<std::string>::iterator i  = store.begin();
	}
	
}
