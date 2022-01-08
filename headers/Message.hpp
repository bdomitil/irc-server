#ifndef IRC_MESSAGE_HPP
#define IRC_MESSAGE_HPP

#include "irc-server.hpp"
#define RESEND_MSG	-1
#define SEND_MSG	1
#define NEW_MSG		0
class Message{

private:
	bool						_isRead;
	std::string					_message;
	std::vector<command_base *> _commands;

public:
	Buffer<MESSAGE_BUFFER_SIZE>	_buffer;
	void						reset();
	void						readM(int fd);
	void						readM(int fd, uint64_t readsize);
	void						sendM(int fd, uint64_t writesize);
	void						sendM(int fd);
	void						setMessage(std::string text){_message = text;};
	std::string					&getMessage(){return _message;}
	bool						isRead(){return _isRead;}
	bool						isSend(){return _isSend;}
	void						exec(users_map &users_map, channels_map &channels_map, void *parent);
	void						addCommand(command_base *command){_commands.push_back(command);}
	int							queue(){return _commands.size();}
	bool						_isSend;
	Message();
};

#endif