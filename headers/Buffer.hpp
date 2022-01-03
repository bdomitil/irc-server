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
		char		*_store;
		buff_status	_status;
		int 		_readSize;
		uint64_t	_len;
public:
		Buffer(int readSize) {
			try{
				_store = new char[x];
			}
			catch(std::exception &e){
				std::cerr << e.what()<< std::endl;
			}
			_status = BUFF_AVAIL;
			_readSize = readSize;
			_len = 0;
		}
		uint64_t size(){return _len;}
		Buffer() {
			try{
				_store = new char[x];
			}
			catch(std::exception &e){
				std::cerr << e.what()<< std::endl;
			}
			_status = BUFF_AVAIL;
			_readSize = 2048;
			_len = 0;
		}
		
		void reset(){
			memset(_store, 0, _len);
			_len = 0;
			_status = BUFF_AVAIL;
		}

		bool checkEnd(){
			char *pos = nullptr;
			uint64_t i = 0;
			while (i < _len){
				if ((_store[i] == '\r' && _store[i + 1] == '\n') ||
				(_store[i] == '\n' && _store[i + 1] == '\0'))
					pos = &(_store[i]);
				i++;
			}
			if (!pos)
				return false;
			int q = _len - i;
			int w = pos - _store;
			pos += 2;
			bzero(pos, _len - i);
			return true;
		}
		char *getStore(){return _store;}

		void readFromSocket(int socket, uint64_t readSize){
			int res = -2;
			res = recv(socket, &(_store[_len]), readSize, 0);
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
			res = recv(socket, _store, _readSize, 0);
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
			res = read(socket, _store, _readSize);
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
			res = read(socket, _store, readSize);
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
			delete [] _store;
		}

};

#endif