#pragma once

#include <string>
#include <map>

#include "Socket.h"

namespace kiva {

enum HttpMethod
{
	GET, POST
};


class Http {
private:
	HttpMethod method;
	Socket sock;
	
	std::map<std::string, std::string> header;
	std::string response;
	std::string path;

	std::string toRequestString();

public:
	static std::string urlEncode(const std::string &str);

public:
	Http(const std::string &host, const std::string &path, int port = 80, HttpMethod method = GET) throw(std::string);
	~Http();

	const std::string& getResponse() const;
	
	void setMethod(HttpMethod method);
	void addHeader(const std::string &k, const std::string &v);
	bool request();
};


}
