#include "../headers/irc-server.hpp"
std::map <std::string, std::string> *g_opers;

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
	g_opers = &_operators;
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

Users *Server::accept_new_user(t_event &event){
	struct sockaddr_in sockaddr;
	socklen_t socklen = sizeof(sockaddr);
	int cl_socket = 0;

	if ((cl_socket = accept(_sockFd, (struct sockaddr*)&sockaddr,&socklen)) == -1){
		return (nullptr);
	}
	if (fcntl(cl_socket, F_SETFL, O_NONBLOCK) == -1) {
		return (nullptr);
	}
	Users *new_User = new Users(cl_socket, &event);
	new_User->setHostip(std::string(inet_ntoa(sockaddr.sin_addr)));
	new_User->setPass(_password);
	return new_User;
}

void Server::startLoop(void){
	users_map users;
	channels_map channels;
	t_event event;

	int res = -2;
	int errors = 0;
	struct kevent *event_list;
	Users *user = nullptr;


	event.addReadEvent(_sockFd, this);
	while (1){
		event.update();
		res = event.proc(&event_list);
		int i = 0;
		while (i < res){
			if (event_list[i].ident == _sockFd){
				if (!(user = accept_new_user(event)))
					std::cerr << "ERROR ACCEPTING NEW CONNECTION; PROBLEM :" << strerror(errno) << std::endl;
			}
			else if (event_list[i].flags & EV_EOF ){
				user = static_cast < Users*>(event_list[i].udata);
				if (!user->isDead()){
					commQuit quit("QUIT :I am tired");
					quit.exec(users, channels,user);
				}
			}
			else if (event_list[i].filter == EVFILT_READ){
				user = static_cast < Users*>(event_list[i].udata);
				if (!user->isDead())
					user->getMessage(event_list[i].data);
			}
			else if (event_list[i].filter == EVFILT_WRITE){
				user = static_cast < Users*>(event_list[i].udata);
				if (!user->isDead())
					user->sendMessage(users, channels, event_list[i].data);
			}
			i++;
		}
		i  = 0;
		if (res == -1){
			errors++;
			std::cerr << "Kevent error #" << errors << std::endl;
			if (errors == 100)
				throw(ErrorException("Too many kevent errors, dropping down"));
		}
	}
}