#ifndef IRC_BUFFER_HPP
#define IRC_BUFFER_HPP
#include "irc-server.hpp"

#define BUFF_FULL	0
#define BUFF_ERROR	-1
#define BUFF_AVAIL	1
#define BUFF_OVERFLOW 2
typedef int buff_status;


template  <int x>
class Buffer{
private: 
		char						*_buff;
		std::vector<std::string>	_store;
		buff_status					_status;
		int 						_readSize;
		uint64_t					_len;
public:
		Buffer(int readSize) {
			try{
				_buff = new char[x];
			}
			catch(std::exception &e){
				std::cerr << e.what()<< std::endl;
			}
			_status = BUFF_AVAIL;
			_readSize = readSize;
			_len = 0;
		}
		Buffer() {
			try{
				_buff = new char[x];
			}
			catch(std::exception &e){
				std::cerr << e.what()<< std::endl;
			}
			_status = BUFF_AVAIL;
			_readSize = 2048;
			_len = 0;
		}
		
		void reset(){
			memset(_buff, 0, _len);
			_len = 0;
			_status = BUFF_AVAIL;
		}

		bool checkEnd(){
			uint64_t i = 0;
			uint64_t pos = 0;
			while (i < _len){
				if ((_buff[i] == '\r' && _buff[i + 1] == '\n') ||
				(_buff[i] == '\n' && _buff[i + 1] == '\0')){
					std::string tmp(strndup(&(_buff[pos]), i - pos));
					strtrim(tmp);
					if (tmp.size())
						_store.insert(_store.end(), tmp);
					pos = i + 1;
				}
				i++;
			}
			if (!pos)
				return false;
			bzero(&(_buff[pos]), _len - pos);
			return true;
		}

		void readFromSocket(int socket, uint64_t readSize){
			int res = -2;
			res = recv(socket, &(_buff[_len]), readSize, 0);
			if (res == -1)
				throw BUFF_ERROR;
			_len += res;
			if (_len > x)
				throw BUFF_OVERFLOW;
			if (checkEnd()){
				_status = BUFF_FULL;
				throw BUFF_FULL;
			}
		}

		void readFromSocket(int socket){
			int res = -2;
			res = recv(socket, _buff, _readSize, 0);
			if (res == -1)
				throw BUFF_ERROR;
			_len += res;
			if (_len > x)
				throw BUFF_OVERFLOW;
			if (checkEnd()){
				_status = BUFF_FULL;
				throw BUFF_FULL;
			}
		}

		void readFromFd(int socket){
			int res = -2;
			res = read(socket, _buff, _readSize);
			if (res == -1)
				throw BUFF_ERROR;
			_len += res;
			if (_len > x)
				throw BUFF_OVERFLOW;
			if (checkEnd()){
				_status = BUFF_FULL;
				throw BUFF_FULL;
			}
		}

		void readFromFd(int socket, uint64_t readSize){
			int res = -2;
			res = read(socket, _buff, readSize);
			if (res == -1)
				throw BUFF_ERROR;
			_len += res;
			if (_len > x)
				throw BUFF_OVERFLOW;
			if (checkEnd()){
				_status = BUFF_FULL;
				throw BUFF_FULL;
			}
		}

		~Buffer(){
			delete [] _buff;
			
		}

		std::vector<std::string> &getStore(){return _store;}
		char *getBuff(void){return _buff;}
		uint64_t size(){return _len;}

};

#endif