#ifndef IRC_CHANNELS_HPP
#define IRC_CHANNELS_HPP

#include "irc-server.hpp"
//CHANNEL FLAGS
#define CH_PRIVATE 		0
#define CH_SECRET 		1
#define CH_INVITE_ONLY	2
#define CH_TOPIC		3
#define CH_NO_OTHERS	4
#define CH_MODERATED	5
//USER FLAGS
#define CH_OPERATOR		0
#define CH_VOTE			1




class Channels{

private:
		std::vector<std::pair<Users*, std::bitset<2> > >	_users;
		std::string											_password;
		Users												*_owner;
		uint64_t											_max_users;
		std::bitset<6>										_flags;
		std::string											_name;
		std::string											_topic;
		uint64_t											_size;
		bool												_isDead;
		std::vector<std::string>							_bans;
		Channels();
		

public:
	std::string 	&getName(){return _name;}
	std::string 	&getTopic(){return _topic;}
	std::string 	&getPassword(){return _password;}
	void			dropPassword(){_password.clear();}
	uint64_t		&getMaxUsers(){return _max_users;}
	void			dropMaxUsers(){_max_users = 0;}
	std::bitset<6>	&getFlags(){return _flags;}
	void			setTopic(std::string topic){_topic = topic;}
	void			setPassword(std::string password){_password = password;}
	Users* 			getOwner(){return _owner;}
	uint64_t		size(){return _size;}
	bool			isDead(){return _isDead;}
	void			setMax(uint64_t x){_max_users = x;}
	bool			isOper(Users *user);
	bool			isOper(std::string nick);
	bool			isPart(Users *user);
	bool			isPart(std::string nick);
	bool			canVote(Users *user);
	bool			canVote(std::string nick);
	void			addVote(Users *user);
	void			dropVote(Users *user);
	void			dropUser(Users *user);
	void			dropUser(std::string nick);
	void			addUser(Users *user);
	void			makeOper(Users *user);
	void			dropOper(Users *user);
	void			changeOwner();
	void			addBan(std::string nick);
	bool			isBaned(std::string nick);
	bool			isBaned(Users *user);
	void			setBan(std::string ban);
	void			dropBan(std::string ban);
	std::string		whoUsers(bool flag_O, bool flag_I, Users *user);
	std::string		userNames(Users *user);
	void			writeToUsers(std::string text, Users *user);
	std::vector< std::pair<Users*, std::bitset<2> > > :: iterator find_user(Users *user);
	std::vector< std::pair<Users*, std::bitset<2> > > :: iterator find_user(std::string nick);
	Channels	(std::string name, Users *user);

};

#endif