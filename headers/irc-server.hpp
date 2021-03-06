#ifndef IRC_SERVER_HPP
#define IRC_SERVER_HPP

#define CONFIG_FILE "server.yaml"
#define CR "\r"
#define LF "\n"
#define MESSAGE_BUFFER_SIZE 512
#define MESSAGE_SEND_SIZE	2048
#define SESSION_END_SEC		180
#include "cpp-libs.hpp"
//TODO INVITE 
//TODO KICK

class Users;
class Channels;
class Server;
class Message;
typedef std::map<std::string, Users*> users_map;
typedef std::map<std::string, Channels*> channels_map;
std::string	strtrim(std::string &str);
std::string	cutPrefix(std::string &text);
int			getArgs(std::string &text, std::vector<std::string> &arg_store);


#include "event.hpp"
#include "Command.hpp"
#include "Buffer.hpp"
#include "Message.hpp"
#include "Users.hpp"
#include "Channels.hpp"
#include "Server.hpp"

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
std::string getMOTD(std::string &target);
void set_time(float seconds, struct timespec &tm);
command_base *genCommand(std::string text, bool auth);
std::string makeErrorMsg(std::string info, int error);
std::string	ft_itoa(int x);
std::string makeReplyHeader(std::string senderNick, std::string receiver, int code);
std::string makeMessageHeader(Users *sender, std::string messageType, std::string receiverNick);
std::time_t increase_session_time();
std::string prefix_to_sender(std::string &prefix);
#endif