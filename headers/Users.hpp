#ifndef IRC_USERS_HPP
#define IRC_USERS_HPP
#include "irc-server.hpp"

#define U_I 0;
#define U_S 1;
#define U_O 2;
#define U_W 3;


class Users{

private:
	int				_socket;
	std::string 	_host_ip;
	std::string		_nick;
	std::bitset<4>	_operations;
	bool			_isAuth;
	bool			_isIRCoperator;	
public:
	Users(std::string host_ip);
	bool	isAuth(void){return _isAuth;}
	bool	isIRCoperator(void){return _isIRCoperator;}
	void	auth(std::string info);
	void	reset();
	void	execCommand(users_map &users_map, channels_map &channels_map, std::string command);
};

#endif