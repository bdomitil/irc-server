#include "../headers/irc-server.hpp"

Server :: Server(char **argv){
	_ip = "0.0.0.0";
	_port = 6667;
	if (!parseConfig(argv))
		throw(ErrorException("ERROR CONFIG FILE PARSING"));
}

bool Server:: parseConfig(char **argv){

	_password = argv[2];
	_port = atoi(argv[1]);
	if (_port < 1)
		return false;
	std::vector<std::string> srv_options;
	std::string login;
	srv_options.push_back("ip");
	srv_options.push_back("name");
	std::vector<std::string> oper_options;
	oper_options.push_back("login");
	oper_options.push_back("passwd");
	bool inServ = false, inOper = false;
	std::string buff;
	std::ifstream FILE(CONFIG_FILE);
	if (! FILE.is_open())
		return false;
	while(std::getline(FILE, buff)){
		std::string key = getKey(buff);
		std::string	value = getValue(buff);
		strtrim(key);
		strtrim(value);
			if (key == "server"){
				inServ = true;
				inOper = false;
			}
			else if (key == "operator"){
				inOper = true;
				inServ = false;
			}
			else if(inOper){
				if (key == "login")
					login = value;
				else if (key == "passwd")
					_operators[login] = value;
			}
			else if (inServ){
				if (key == "ip")
					_ip = key;
			}
	}
	return true;
}

void Server :: run(){
	int reuseaddr = 1;
	struct sockaddr_in sockaddr;

	if (inet_pton(AF_INET, _ip.c_str(), &(sockaddr.sin_addr.s_addr)) < 1)
			throw(ErrorException(strerror(errno)));
	sockaddr.sin_family = PF_INET;
	sockaddr.sin_port = htons(_port);
	_sockFd = socket(PF_INET, SOCK_STREAM, 0);
	if (_sockFd == -1) {
		throw(ErrorException(strerror(errno)));
	}
	if (setsockopt(_sockFd ,SOL_SOCKET ,SO_REUSEADDR , &reuseaddr,sizeof(int)) == -1) {
		throw(ErrorException(strerror(errno)));
	}
	if (fcntl(_sockFd, F_SETFL, O_NONBLOCK) == -1) {
		throw(ErrorException(strerror(errno)));
	}
	if (bind(_sockFd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) == -1) {
		throw(ErrorException(strerror(errno)));
	}
	if (listen(_sockFd, 50) == -1) {
		throw(ErrorException(strerror(errno)));
	}
}

void Server::startLoop(void){
	int kevFd;
	if ((kevFd = kqueue()) == -1)
		throw(ErrorException(strerror(errno)));
	
}