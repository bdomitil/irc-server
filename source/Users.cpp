
#include "../headers/irc-server.hpp"

Users::Users(int fd){
	_flags.reset();
	_socket = fd;
	_isAuth = false;
	_isIRCoperator = false;
	_isDead = false;
}

Users::Users(int fd, t_event *event){
	_flags.reset();
	_socket = fd;
	_isAuth = false;
	_isIRCoperator = false;
	_event = event;
	_awayMsg.clear();
	event->addReadEvent(_socket, this);
	event->addWriteEvent(_socket, this);
	event->disableWriteEvent(_socket, this);
}

Users::~Users(){
	uint32_t &ev_size = _event->size();
	ev_size -= 2;
}

void Users::reset(){
	_isAuth = false;
	_host_ip.clear();
	_flags.reset();
	_nick.clear();
	_apasswd.clear();
	_awayMsg.clear();
	_hostname.clear();
	_name.clear();
	_servername.clear();
	_realname.clear();
}

bool Users::auth(users_map &users){
	if (_isAuth)
		return true;
	if (_apasswd.size() && _realname.size() && _nick.size())
	{
		_isAuth = true;
		message.addCommand(new commMODT(" "));
		users[_nick] = this;
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
	while (i != store.end()){
		message.addCommand(genCommand(*i, _isAuth));
		i++;
	}
	store.clear();
	if (message.isSend() && message.queue()){
		message.exec(users_map, channels_map, this);
		if (_isDead){
			delete this;
			return ;
		}
		auth(users_map);
		message.sendM(_socket, writesize);
	}
	else
		message.sendM(_socket, writesize);
	if (message.isSend() && !message.queue())
		_event->disableWriteEvent(_socket, this);
	else
		_event->enableWriteEvent(_socket, this);
}

void Users::writeMessage(std::string text){
	std::vector<std::string>&store = message._buffer.getStore();
	if (text.find(CR LF) == std::string::npos)
		store.insert(store.end(), text + CR LF);
	else
		store.insert(store.end(), text);
	_event->enableWriteEvent(_socket, this);
}
