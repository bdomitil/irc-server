
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

bool Users::auth(){
	if (_apasswd.size() && _realname.size() && _nick.size())
	{
		_isAuth = true;
		message.setMessage(getMOTD(_nick));
		return true;
	}
	return false;
}


void Users::getMessage(uint64_t readsize){
	message.readM(_socket, readsize);
	if (message.isRead()){
		_event->enableWriteEvent(_socket, this);
	}
}

void Users::sendMessage(users_map &users_map, channels_map &channels_map, uint64_t writesize){
	std::vector<std::string>&store = message._buffer.getStore();
	std::vector<std::string>::iterator i  = store.begin();
	if (message.isSend()){
		while (i != store.end() ){
			message.exec(users_map, channels_map, this);
			if (!_isAuth){
				if (auth())
					users_map.insert(std::pair<std::string, Users*>(_nick, this));
			}
			message.sendM(_socket, writesize);
			store.erase(i);
			std::vector<std::string>::iterator i  = store.begin();
		}
	}
	else
		message.sendM(_socket, writesize);
	if (message.isSend())
		_event->disableWriteEvent(_socket, this);
}

void Users::writeMessage(std::string text){
	_hasMessage = true;
	std::vector<std::string>&store = message._buffer.getStore();
	store.insert(store.end(), text + CR LF);
	// message.setMessage(text + CR LF);
	_event->enableWriteEvent(_socket, this);
}
