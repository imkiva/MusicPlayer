#pragma once

#include <string>
#include <sstream>
#include <algorithm>
#include <functional>
#include <exception>
#include <thread>
#include <mutex>

#include <cstring>
#include <cstdlib>

#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define SOCKET_BUFFER_SIZE 1024

namespace kiva {

enum SocketFamily
{
	V4 = AF_INET, 
	V6 = AF_INET6,
};

enum SocketType
{
	TCP = SOCK_STREAM,
	UDP = SOCK_DGRAM,
};



class Socket
{
private:
	int fd;
	struct sockaddr_in addr;
	bool connected;

	/* ipv4: 15, ipv6:63 */
	char ip[64];

	Socket();

public:
	static struct hostent* resolveHost(const std::string &host, char *buf, size_t bufsz);

public:
	Socket(const std::string &addr, int port, 
		SocketFamily family = V4, SocketType type = TCP) throw (std::string);
	virtual ~Socket();
	
	bool isConnected() const;
	std::string getIp() const;
	
	bool connect();
	void disconnect();

	ssize_t write(const std::string &data) const;
	ssize_t write(const char *buffer, size_t sz) const;

	ssize_t read(std::string &dst) const;
	ssize_t read(char *buffer, size_t sz) const;
	void readAll(std::string &dst) const;
};

};
