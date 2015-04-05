#ifndef __USER_H__
#define __USER_H__
#include <string>

const int USER_AVAILABLE = 0;
const int USER_BUSY = 1;

struct online_user {
	std::string username;
	int status;
	int rivalsock;

	online_user() { status = USER_AVAILABLE; rivalsock = -1; }
	online_user(char *username, int status) {
		this->username = username;
		this->status = status;
		this->rivalsock = -1;
	}
	bool operator ==(const online_user& user) {
		return username == user.username;
	}
};

#endif
