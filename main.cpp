#include "headers/irc-server.hpp"

int main(int argc, char **argv){
	if (argc != 3){
		std::cerr << "ERROR IN ARGS" << std::endl;
		std::cerr << "TRY: ./irc-server [port] [password]" << std::endl;
		return (1);
	}
	try{
		Server irServer(argv);
		irServer.run();
		irServer.startLoop();
	}
	catch (std::exception &e){
		std::cerr << e.what() << std::endl;
		return (1);
	}
	return (0);
}