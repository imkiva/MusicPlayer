#include "Socket.h"

using namespace kiva;

Socket::Socket()
{
	connected = false;
	fd = -1;
}


Socket::Socket(const std::string &address, int port, SocketFamily family, SocketType type)
	throw (std::string)
{
	if (address.empty() || port < 0) {
		throw std::string("Invalid arguments");
	}

	size_t bufsz = -1;
	if (family == V4) {
		bufsz = 16;
	} else if (family == V6) {
		bufsz = 64;
	}

	connected = false;

	struct hostent *ent = Socket::resolveHost(address, ip, bufsz);
	ip[bufsz - 1] = '\0';

	if (!ent) {
		throw std::string("Cannot resolve host");
	}

	fd = socket((int) family, (int) type, 
		(type == TCP ? IPPROTO_TCP : IPPROTO_UDP));
  
	bzero(&addr, sizeof(addr));

	addr.sin_family = (int) family;
	addr.sin_addr = *((struct in_addr *)ent->h_addr);
	addr.sin_port = htons(port);
}


Socket::~Socket()
{
	disconnect();
}


struct hostent* Socket::resolveHost(const std::string &host, char *buf, size_t bufsz) {
	if (host.empty() || !buf || bufsz < 0) {
		return NULL;
	}

	struct hostent *ent = gethostbyname(host.c_str());

	if (!ent) {
		return NULL;
	}

	inet_ntop(ent->h_addrtype, ent->h_addr, buf, bufsz);
	return ent;
}


bool Socket::isConnected() const
{
	return this->connected;
}


std::string Socket::getIp() const
{
	return std::string(ip);
}


bool Socket::connect()
{
	if (connected) {
		return true;
	}

	if (::connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		return false;
	}

	connected = true;
	return true;
}


void Socket::disconnect()
{
	if (isConnected()) {
		close(fd);
		connected = false;
	}
}


ssize_t Socket::write(const std::string &data) const
{
	return write(data.c_str(), data.length());
}


ssize_t Socket::write(const char *buffer, size_t sz) const
{
	if (!isConnected() || !buffer || sz < 0) {
		return -1;
	}

	return ::write(fd, buffer, sz);
}


ssize_t Socket::read(std::string &dst) const
{
	char buffer[SOCKET_BUFFER_SIZE];
	
	int n = read(buffer, SOCKET_BUFFER_SIZE - 1);
	buffer[n] = '\0';

	dst = std::string(buffer);
	return n;
}


ssize_t Socket::read(char *buffer, size_t sz) const
{
	if (!isConnected() || !buffer || sz < 0) {
		return -1;
	}

	return ::read(fd, buffer, sz);
}


void Socket::readAll(std::string &dst) const
{
	std::stringstream ss;
	char buffer[SOCKET_BUFFER_SIZE - 1] = { 0 };
	char ch;

	ssize_t n;
	ssize_t count = 0;

	while ((n = read(buffer, SOCKET_BUFFER_SIZE-1)) > 0) {
		ch = buffer[n];
		if (ch == '\n' || ch == '\r') {
			break;
		}

		buffer[n] = '\0';
		ss << buffer;

		count += n;
	}

	dst = ss.str();
}

