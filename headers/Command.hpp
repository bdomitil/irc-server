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
		std::string					request;

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

//#############################################//


class commOper : public command_base{

	public:
		commOper(std::string text);
		std::string exec(users_map &users_map, channels_map &channels_map, void *parent);
};

//#############################################//


class commMode : public command_base{

	public:
		commMode(std::string text);
		std::string exec(users_map &users_map, channels_map &channels_map, void *parent);
};

//#############################################//


class commWho : public command_base{

	public:
		commWho(std::string text);
		std::string exec(users_map &users_map, channels_map &channels_map, void *parent);
};

//#############################################//


class commJoin : public command_base{

	public:
		commJoin(std::string text);
		std::string exec(users_map &users_map, channels_map &channels_map, void *parent);
};

//#############################################//


class commTopic : public command_base{

	public:
		commTopic(std::string text);
		std::string exec(users_map &users_map, channels_map &channels_map, void *parent);
};


//#############################################//


class commNames : public command_base{

	public:
		commNames(std::string text);
		std::string exec(users_map &users_map, channels_map &channels_map, void *parent);
};

//#############################################//


class commQuit : public command_base{

	public:
		commQuit(std::string text);
		std::string exec(users_map &users_map, channels_map &channels_map, void *parent);
};

//#############################################//


class commMODT : public command_base{

	public:
		commMODT(std::string text);
		std::string exec(users_map &users_map, channels_map &channels_map, void *parent);
};

//#############################################//


class commAway : public command_base{

	public:
		commAway(std::string text);
		std::string exec(users_map &users_map, channels_map &channels_map, void *parent);
};

//#############################################//


class commInvite : public command_base{

	public:
		commInvite(std::string text);
		std::string exec(users_map &users_map, channels_map &channels_map, void *parent);
};

//#############################################//


class commKick : public command_base{

	public:
		commKick(std::string text);
		std::string exec(users_map &users_map, channels_map &channels_map, void *parent);
};





#endif