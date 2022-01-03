#ifndef IRC_EVENT_HPP
#define IRC_EVENT_HPP
#include "irc-server.hpp"

class t_event{
private:
	uint32_t		len;
	int				fd;
	uint32_t		update_len;
	struct kevent	*event_list;
public:
	void update();
	void addReadEvent(int fd, void *udata);
	void addWriteEvent(int fd, void *udata);
	void disableReadEvent(int fd, void *udata);
	void disableWriteEvent(int fd, void *udata);
	void enableWriteEvent(int fd, void *udata);
	void enableReadEvent(int fd, void *udata);

	int	 proc(struct kevent **list);
	t_event();
};

#endif 