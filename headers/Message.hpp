#ifndef IRC_MESSAGE_HPP
#define IRC_MESSAGE_HPP

#include "irc-server.hpp"

class Message{

private:
	bool						_isRead;
	std::string					_message;

public:
	Buffer<MESSAGE_BUFFER_SIZE>	_buffer;
	void						readMessage(int fd);
	void						readMessage(int fd, uint64_t readsize);
	std::string					getMessage();
	bool						isRead(){return _isRead;}
	void						exec(users_map &users_map, channels_map &channels_map, void *parent);
	Message();
};

#endif