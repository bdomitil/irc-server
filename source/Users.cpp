
#include "../headers/irc-server.hpp"

Users::Users(int fd){
	_operations.reset();
	_socket = fd;
	_isAuth = false;
	_isIRCoperator = false;
	_passwd = false;
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
	_passwd = false;
}

void Users::auth(){

	
}


void Users::getMessage(uint64_t readsize){
	command.readMessage(_socket, readsize);
	if (command.isRead()){
		_event->disableReadEvent(_socket, this);
		_event->enableWriteEvent(_socket, this);
	}
}

void Users::sendMessage(users_map &users_map, channels_map &channels_map){
	
}