#ifndef IRC_SERVER_HPP
#define IRC_SERVER_HPP

#define CONFIG_FILE "server.yaml"

#include "cpp-libs.hpp"
#include "Server.hpp"
#include "Users.hpp"
#include "Channels.hpp"
typedef std::map<std::string, Users> users_map;
typedef std::map<std::string, Channels> channels_map;
class ErrorException : public std::exception {

public :

	ErrorException(const char *msg) : errorMsg(msg), status(0) {}
	ErrorException(int st, const char *msg) : errorMsg(msg), status(st) {}
	int getStatus() const {return status;}

	virtual const char* what(void) const throw () {
		return (this->errorMsg);
	}

private:

	const char*	errorMsg;
	const int	status;

};

std::string getValue(std::string &str);
std::string getKey(std::string &str);
std::string strtrim(std::string &str);
void set_time(float seconds, struct timespec &tm);
#endif