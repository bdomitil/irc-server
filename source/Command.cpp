#include "../headers/irc-server.hpp"

Command::Command(){

}

void Command::readMessage(int fd){
	try{
		_buffer.readFromSocket(fd);
	}
	catch (buff_status &e){
		if (e == BUFF_FULL){
			_isRead = true;
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

void Command::readMessage(int fd, uint64_t readsize){
	try{
		_buffer.readFromSocket(fd, readsize);
	}
	catch (buff_status &e){
		if (e == BUFF_FULL){
			_isRead = true;
			_buffer.getStore();
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