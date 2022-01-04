#include "../headers/irc-server.hpp"

Message::Message(){

}

void Message::readMessage(int fd){
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

void Message::readMessage(int fd, uint64_t readsize){
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

std::string Message::getMessage(){
	return ("tmpmessage");
}

command_base *genCommand(std::string text, bool auth){
	std::vector<std::string> tmpV;
	std::string command = text;
	cutPrefix(command);
	getArgs(command, tmpV);
	command  = *tmpV.begin();

	if (auth){
		(void)2;
	}
	else{
		if (command == "NICK")
			return new commNick(text);
		else if (command == "PASS")
			return new commPass(text);
		else if (command == "USER")
			return new commUser(text);
		return new commNonAuth(text);
	}
	return (new commNotFound(text));
}

void Message::exec(users_map &users_map, channels_map &channels_map, void *parent){
	Users *user = (Users*)parent;
	try{
		command_base *command = genCommand(_buffer.getStore()[0], user->isAuth());
		command->exec(users_map, channels_map, parent);
	}
	catch (command_base *e){
		_message =  e->getReply();
		if (!user->isAuth())
			user->reset();
	}
	catch (std::exception &e){

	}
}