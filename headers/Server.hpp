#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP
#include "irc-server.hpp"

struct configFile{
};

class Server{

private:
	uint32_t						_port;
	std::string						_ip;
	int								_sockFd;
	std::string						_password;
	std::map <std::string, std::string> _operators;
	bool							parseConfig(char **argv);
	Users							*accept_new_user(t_event &event);
	Server();

public:
	void		setPort(uint32_t port){_port = port;}
	void		setSocket(int socket){_sockFd = socket;}
	int			getSocket(void){return _sockFd;}
	std::string	getPassword(void){return _password;}
	void		run();
	void		startLoop();
	Server(char **argv);
};


#endif