#include "../headers/irc-server.hpp"

Message::Message(){
	_isRead = false;
	_isSend = true;
	_message.clear();
	_commands.clear();
}

void Message::reset(){
	_isRead = false;
	_isSend = true;
	_message.clear();
	for (int i = 0;i < _commands.size(); i++)
		delete _commands[i];
	_commands.clear();
}

void Message::readM(int fd){
	try{
		_buffer.readFromSocket(fd);
	}
	catch (buff_status &e){
		if (e == BUFF_FULL){
			_isRead = true;
			_message = std::string(_buffer.getBuff());
		}
		else if (e == BUFF_ERROR){
			std::cerr << "ERROR FILLING MESSAGE BUFFER" << std::endl;
			_buffer = Buffer<MESSAGE_BUFFER_SIZE>();
		}
		else if (e == BUFF_OVERFLOW){
			std::cerr << "MESSAGE BUFFER OVERFLOW; AVAIL SIZE " << MESSAGE_BUFFER_SIZE << std::endl;
			_buffer = Buffer<MESSAGE_BUFFER_SIZE>();
		}
	}
}

void Message::readM(int fd, uint64_t readsize){
	try{
		_buffer.readFromSocket(fd, readsize);
	}
	catch (buff_status &e){
		if (e == BUFF_FULL){
			_isRead = true;
			_buffer.reset();
		}
		else if (e == BUFF_ERROR){
			std::cerr << "ERROR FILLING MESSAGE BUFFER" << std::endl;
			_buffer = Buffer<MESSAGE_BUFFER_SIZE>();
		}
		else if (e == BUFF_OVERFLOW){
			std::cerr << "MESSAGE BUFFER OVERFLOW; AVAIL SIZE " << MESSAGE_BUFFER_SIZE << std::endl;
			_buffer = Buffer<MESSAGE_BUFFER_SIZE>();
		}
	}
}

void Message::sendM(int fd, uint64_t writesize){
	if (_message.size()){
		if (writesize > _message.size())
			writesize = _message.size();
		uint64_t res = send(fd, _message.c_str(), writesize, 0);
		if (res > 0){
			_message.erase(0, res);
			if (_message.size()){
				_isSend = false;
				return ;
			}
		}
	}
	if (_commands.size() == 1 && !_buffer.getStore().size())
		reset();
	else{
		_isRead = true;
		if (_commands.size()){
			delete _commands[0];
			_commands.erase(_commands.begin());
		}
	}
}

void Message::sendM(int fd){
	uint64_t writesize = MESSAGE_SEND_SIZE;
	if (writesize > _message.size())
			writesize = _message.size();
	if (_message.size()) {
		uint64_t res = send(fd, _message.c_str(), MESSAGE_SEND_SIZE, 0);
		if (res > 0){
			_message.erase(0, res);
			if (_message.size()){
				_isSend = false;
				return ;
			}
		}
	}
	if (_commands.size() == 1 && !_buffer.getStore().size())
		reset();
	else{
		_isRead = true;
		delete _commands[0];
		_commands.erase(_commands.begin());
	}
}


void Message::exec(users_map &users_map, channels_map &channels_map, void *parent){
	Users *user = (Users*)parent;
	try{
		_commands[0]->exec(users_map, channels_map, parent);
		_message += _commands[0]->getReply();
	}
	catch (command_base *e){
		_message =  e->getReply();
		if (!user->isAuth())
			user->reset();
	}
	catch (std::exception &e){

	}
}