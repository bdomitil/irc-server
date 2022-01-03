#ifndef IRC_COMMAND_HPP
#define IRC_COMMAND_HPP

#include "irc-server.hpp"

class Command{

private:
	Buffer<MESSAGE_BUFFER_SIZE>	_buffer;
	bool						_isRead;
	std::string					_message;
	uint32_t					_args;
	std::string 				_prefix;

public:
	void			readMessage(int fd);
	void			readMessage(int fd, uint64_t readsize);
	bool			isRead(){return _isRead;}
	void			exec(users_map &users_map, channels_map &channels_map);
	Command();
};

#endif