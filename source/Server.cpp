#include "../headers/irc-server.hpp"

Server :: Server(char **argv){
	_ip = "0.0.0.0";
	_port = 6667;
	if (!parseConfig(argv))
		throw(ErrorException("ERROR CONFIG FILE PARSING"));
	_event_list = new struct kevent[1];
	bzero(_event_list, sizeof(struct kevent));
	_event_list_num = 1;
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
	if ((_kevFd = kqueue()) == -1){
		throw(ErrorException(strerror(errno)));
	}
	EV_SET(_event_list, _sockFd, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, NULL);
	if (kevent(_kevFd, _event_list, 1, NULL, 0, NULL) == -1){
		throw(ErrorException(strerror(errno)));
	}
	
}

bool Server::accept_new_client(){
	struct sockaddr_in sockaddr;
	socklen_t socklen = sizeof(sockaddr);
	struct kevent kv = {0};
	int cl_socket = 0;


	if ((cl_socket = accept(_sockFd, (struct sockaddr*)&sockaddr,&socklen)) == -1){
		return (false);
	}
	if (fcntl(cl_socket, F_SETFL, O_NONBLOCK) == -1) {
		return (false);
	}
	EV_SET(&kv, cl_socket, EVFILT_READ, EV_ADD | EV_CLEAR, 0 , 0, nullptr); //last arg must send to client object
	if (kevent(_kevFd, &kv, 1, NULL, 0, NULL) == -1){
		return (false);
	}
	_event_list_num += 1;
	delete[] _event_list;
	_event_list = new struct kevent[_event_list_num];
	return true;
}

void Server::startLoop(void){
	int res = -2;
	uint8_t errors = 0;
	while (1){
		struct timespec timeout = {0};
		set_time(1.05, timeout);
		res = kevent(_kevFd, nullptr, 0, _event_list, _event_list_num, &timeout);
		while (res > 0){
			int x = EVFILT_WRITE;
			if (_event_list[res - 1].ident == _sockFd){
				accept_new_client();
			}
			else if (_event_list[res - 1].flags & EV_EOF){
				(void) 2;//disconnect client
			}
			else if (_event_list[res - 1].filter == EVFILT_READ){
				(void)1; //read client
			}
			else if (_event_list[res - 1].filter == EVFILT_WRITE){
				(void)1; //write to client
			}
			res--;
		}
		if (res == -1){
			errors++;
			std::cerr << "Kevent error #" << errors << std::endl;
			if (errors == 100)
				throw(ErrorException("Too many kevent errors, dropping down"));
		}
	}
}