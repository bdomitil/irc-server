#ifndef IRC_COMMAND_HPP
#define IRC_COMMAND_HPP
#include "irc-server.hpp"

class command_base{
protected:
		std::string					prefix;
		uint32_t					num_args;
		uint32_t					maxArgs;
		std::vector<std::string>	args;
		std::string					reply;
		int							error;

public:
		virtual std::string exec(users_map &users_map, channels_map &channels_map, void *parent) = 0;
		std::string getReply(){return reply;}
		command_base(std::string text);
		command_base();
		int	getError(void){return error;}
		virtual ~command_base(){ };
		void reset();
};


//#############################################//


class commNick : public command_base{

	public:
		commNick(std::string text);
		std::string exec(users_map &users_map, channels_map &channels_map, void *parent);
};


//#############################################//
class commNotFound : public command_base{

	public:
		commNotFound(std::string text);
		std::string exec(users_map &users_map, channels_map &channels_map, void *parent);
};


//#############################################//
class commNonAuth : public command_base{
	
	public:
		commNonAuth(std::string text);
		std::string exec(users_map &users_map, channels_map &channels_map, void *parent);
};

//#############################################//

class commPass : public command_base{

	public:
		commPass(std::string text);
		std::string exec(users_map &users_map, channels_map &channels_map, void *parent);
};

//#############################################//


class commUser : public command_base{

	public:
		commUser(std::string text);
		std::string exec(users_map &users_map, channels_map &channels_map, void *parent);
};

//#############################################//


class commPong : public command_base{

	public:
		commPong(std::string text);
		std::string exec(users_map &users_map, channels_map &channels_map, void *parent);
};


//#############################################//


class commPrivMsg : public command_base{

	public:
		commPrivMsg(std::string text);
		std::string exec(users_map &users_map, channels_map &channels_map, void *parent);
};



#endif