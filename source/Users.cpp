#include "../headers/irc-server.hpp"

Users::Users(std::string host_ip){
	_operations.reset();
	_host_ip = host_ip;
	_isAuth = false;
	_isIRCoperator = false;
}

void Users::reset(){
	_isAuth = false;
	_host_ip.clear();
	_operations.reset();
	_nick.clear();
}

void Users::auth(std::string info){

}