#include "headers/irc-server.hpp"

int main(int argc, char **argv){
	try{
		Server irServer(argv);
		irServer.run();
		irServer.startLoop();
	}
	catch (std::exception &e){
		std::cerr << e.what() << std::endl;
	}
	return (0);
}