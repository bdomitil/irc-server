#include "../headers/irc-server.hpp"

t_event::t_event(){
	 event_list = new struct kevent[1];
	bzero(event_list, sizeof(struct kevent));
	len = 0;
	update_len = 0;
	if ((fd = kqueue()) == -1){
		throw(ErrorException(strerror(errno)));
	}
}

void t_event::update(){
	if (update_len != len){
		try{
			delete [] event_list;
			event_list = new struct kevent[update_len];
		}
		catch(std::exception &e){
		std::cerr << e.what() << std::endl;
		event_list = new struct kevent[len];
		}
		len = update_len;
	}
}

void t_event::addReadEvent(int socket, void *udata){
	struct kevent kv = {0};
	EV_SET(&kv, socket, EVFILT_READ, EV_ADD | EV_CLEAR, 0 , 0, udata);
	if (kevent(fd, &kv, 1, NULL, 0, NULL) == -1){
		std::cerr << "ERROR adding event for write" << std::endl;
	}
	update_len++;
}

void t_event::addWriteEvent(int socket, void *udata){
	struct kevent kv = {0};
	EV_SET(&kv, socket, EVFILT_WRITE, EV_ADD | EV_CLEAR, 0 , 0, udata);
	if (kevent(fd, &kv, 1, NULL, 0, NULL) == -1){
		std::cerr << "ERROR adding event for write" << std::endl;
	}
	update_len++;
}

void t_event::disableWriteEvent(int socket, void *udata){
	struct kevent kv = {0};
	EV_SET(&kv, socket, EVFILT_WRITE, EV_DISABLE, 0 , 0, udata);
	if (kevent(fd, &kv, 1, NULL, 0, NULL) == -1){
		std::cerr << "ERROR disabling event for write" << std::endl;
	}
}

void t_event::disableReadEvent(int socket, void *udata){
	struct kevent kv = {0};
	EV_SET(&kv, socket, EVFILT_READ, EV_DISABLE, 0 , 0, udata);
	if (kevent(fd, &kv, 1, NULL, 0, NULL) == -1){
		std::cerr << "ERROR disabling event for read" << std::endl;
	}
}

void t_event::enableReadEvent(int socket, void *udata){
	struct kevent kv = {0};
	EV_SET(&kv, socket, EVFILT_READ, EV_ENABLE, 0 , 0, udata);
	if (kevent(fd, &kv, 1, NULL, 0, NULL) == -1){
		std::cerr << "ERROR enabling event for read" << std::endl;
	}
}

void t_event::enableWriteEvent(int socket, void *udata){
	struct kevent kv = {0};
	EV_SET(&kv, socket, EVFILT_WRITE, EV_ENABLE, 0 , 0, udata);
	if (kevent(fd, &kv, 1, NULL, 0, NULL) == -1){
		std::cerr << "ERROR enabling event for read" << std::endl;
	}
}

int	t_event::proc(struct kevent **list){
	int res = -1;
	struct timespec timeout = {0};

	set_time(2.5, timeout);
	res = kevent(fd, nullptr, 0, event_list, len, &timeout);
	*list = event_list;
	return res;
}