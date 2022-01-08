#ifndef IRC_USERS_HPP
#define IRC_USERS_HPP
#include "irc-server.hpp"

#define U_I 0
#define U_S 1
#define U_O 2
#define U_W 3

class Users{

private:
	int				_socket;
	std::string 	_host_ip;
	std::string		_nick;
	std::string		_passwd;
	std::string		_apasswd;
	std::string		_realname;
	std::string		_hostname;
	std::string		_servername;
	std::string		_name;
	std::bitset<4>	_flags;
	t_event			*_event;
	Message			message;
	bool			_isAuth;
	
public:
	Users		(int fd, t_event *event);
	Users		(int fd);
	void		getMessage(uint64_t readsize);
	Message		&getMessage(){return message;}
	bool		auth();
	void		reset();
	void		sendMessage(users_map &users_map, channels_map &channels_map, uint64_t writesize);
	void		writeMessage(std::string text);

public:
	bool		_isIRCoperator;
	bool		isAuth(void){return _isAuth;}
	bool		isIRCoperator(void){return _isIRCoperator;}
	void		makeIRCoperator(void){_isIRCoperator = true;}
	int			&getSocket(void){return _socket;}
	std::string	&getNick(void){return _nick;}
	std::string &getPasswd(void){return _passwd;}
	std::string &getName(void){return _name;}
	std::string &getRealName(void){return _realname;}
	std::string &getHostname(void){return _hostname;}
	std::string &gethostIp(void){return _host_ip;}
	std::bitset<4> &getflags(){return _flags;};
	bool		checkFlag(int flag){ return _flags.test(flag);}
	void		setNick(std::string nick){_nick = nick;}
	void		setPass(std::string pass){_passwd = pass;}
	void		setaPass(std::string pass){_apasswd = pass;}
	void		setHostip(std::string hostIp){_host_ip = hostIp;}
	void		setKevent(t_event *event){_event = event;}
	void		setName(std::string name){_name = name;}
	void		setRealname(std::string realname){_realname = realname;}
	void		setHostname(std::string hostname){_hostname = hostname;}
	void		setServername(std::string servername){_servername = servername;}
};

#endif