#include "../headers/irc-server.hpp"

Message::Message(){
	_isRead = false;
	_isSend = true;
	_message.clear();
	_command = nullptr;
}

void Message::reset(){
	_isRead = false;
	_isSend = true;
	_message.clear();
	if (_command)
		delete _command;
	_command = nullptr;
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
	reset();
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
	reset();
}


void Message::exec(users_map &users_map, channels_map &channels_map, void *parent){
	Users *user = (Users*)parent;
	try{
		if (!_command)
			_command = genCommand(_buffer.getStore()[0], user->isAuth());
		_command->exec(users_map, channels_map, parent);
		_message += _command->getReply();
	}
	catch (command_base *e){
		_message =  e->getReply();
		if (!user->isAuth())
			user->reset();
	}
	catch (std::exception &e){

	}
}