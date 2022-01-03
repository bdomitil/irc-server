#ifndef IRC_USERS_HPP
#define IRC_USERS_HPP
#include "irc-server.hpp"

#define U_I 0;
#define U_S 1;
#define U_O 2;
#define U_W 3;
class Users;

class Users{

private:
	int				_socket;
	std::string 	_host_ip;
	std::string		_nick;
	bool			_passwd;
	std::bitset<4>	_operations;
	bool			_isAuth;
	bool			_isIRCoperator;
	t_event			*_event;
	Command			command;
	
public:
	Users		(int fd, t_event *event);
	Users		(int fd);
	void		getMessage(uint64_t readsize);
	void		sendMessage(users_map &users_map, channels_map &channels_map);
	bool		isAuth(void){return _isAuth;}
	bool		isIRCoperator(void){return _isIRCoperator;}
	void		setHostip(std::string hostIp){_host_ip = hostIp;}
	int			getSocket(void){return _socket;}
	std::string	getNick(void){return _nick;}
	void		setKevent(t_event *event){_event = event;}
	void		auth();
	void		reset();
};

#endif