#include <stdio.h>

#include <sstream>

#include "Http.h"

using namespace kiva;


Http::Http(const std::string &host, const std::string &path, int port, HttpMethod method) throw(std::string)
	:sock(host, port), method(method), path(path)
{
	addHeader("Host", host);
}


Http::~Http()
{

}


std::string Http::urlEncode(const std::string &str)
{
	std::stringstream ss;

	size_t length = str.length();
	unsigned char ch;
	char buf[4];

	for (size_t i = 0; i < length; i++) {
		ch = str[i];

		if (((ch >= 'A') && (ch < 'Z')) ||
			((ch >= 'a') && (ch < 'z')) ||
			((ch >= '0') && (ch < '9'))) {
			ss << ch;

		} else if (ch == ' ') {
			ss << '+';

		} else if (ch == '.' || ch == '-' || ch == '_' || ch == '*') {
			ss << ch;

		} else {
			snprintf(buf, 4, "%%%02X", ch);
			ss << buf;
		}
	}

	return std::move(ss.str());
}


std::string Http::toRequestString()
{
	std::stringstream ss;

	ss << (method == GET ? "GET" : "POST") << ' ' << path << " HTTP/1.1\r\n";
	for (auto &e : header) {
		ss << e.first << ": " << e.second << "\r\n";
	}

	ss << "\r\n";

	return std::move(ss.str());
}


void Http::setMethod(HttpMethod method)
{
	this->method = method;
}


void Http::addHeader(const std::string &k, const std::string &v)
{
	header[k] = v;
}


bool Http::request()
{
	if (!sock.connect()) {
		return false;
	}

	std::string rs;

	sock.write(toRequestString());
	sock.readAll(rs);

	int pos = rs.find("\r\n\r\n");
	if (pos != std::string::npos) {
		response = std::move(rs.substr(pos));
	} else {
		response = std::move(rs);
	}

}


const std::string& Http::getResponse() const
{
	return response;
}

