#include <string.h>
#include <stdio.h>

#include <sstream>
#include <fstream>

#include "IniFile.h"

using namespace kiva;


IniValue::IniValue()
{
	type = NONE;
}


IniValue::IniValue(int i)
{
	*this = i;
}


IniValue::IniValue(bool b)
{
	*this = b;
}


IniValue::IniValue(const std::string &str)
{
	*this = str;
}


IniValue::~IniValue()
{
}
	

IniValue& IniValue::operator=(int i)
{
	type = INT;
	value.intValue = i;
}


IniValue& IniValue::operator=(bool b)
{
	type = BOOL;
	value.boolValue = b;
}


IniValue& IniValue::operator=(const std::string &str)
{
	type = STRING;
	this->str = str;
}
	

bool IniValue::isValid() const
{
	return type != NONE;
}
	

bool IniValue::asBool() const
{
	if (!isValid()) {
		return false;
	}
	
	switch(type) {
	case INT:
		return value.intValue == 1;
	case BOOL:
		return value.boolValue;
	case STRING:
		return str == "true";
	}
}


int IniValue::asInt() const
{
	if (!isValid()) {
		return 0;
	}
	
	switch(type) {
	case INT:
		return value.intValue;
	case BOOL:
		return value.boolValue ? 1 : 0;
	case STRING:
		if (str.empty()) {
			return 0;
		}
		
		int i;
		sscanf(str.c_str(), "%d", &i);
		return i;
	}
}


const std::string& IniValue::asString() const
{
	static std::string empty;
	
	if (!isValid() && type != STRING) {
		return empty;
	}
	
	return str;
}


ValueType IniValue::getType() const
{
	return type;
}



Section::Section(const std::string &name)
	:name(name)
{
}


Section::Section(const std::string &name, const Section &o)
	:name(name)
{
	*this = o;
}


Section::~Section()
{
}


int Section::size() const
{
	return data.size();
}


bool Section::contains(const std::string &key) const
{
	auto it = data.find(key);
	
	return it != data.end();
}


bool Section::isEmpty() const
{
	return size() == 0;
}


void Section::set(const std::string &key, const IniValue &value)
{
	data[key] = value;
}


IniValue& Section::get(const std::string &key)
{
	static IniValue empty;
	
	if (contains(key)) {
		return data[key];
	}
	
	return empty;
}


const std::string& Section::getName() const
{
	return name;
}


void Section::setName(const std::string &name)
{
	this->name = name;
}


const std::map<std::string, IniValue>& Section::mapping() const
{
	return data;
}


Section& Section::operator=(const Section &o)
{
	if (name.empty()) {
		name = o.name;
	}
	
	data = o.data;
	
	return *this;
}


IniValue& Section::operator[](const std::string &key)
{
	return data[key];
}


bool Section::operator==(const Section &o) const
{
	return name == o.name;
}


bool Section::operator==(const std::string &name) const
{
	return this->name == name;
}




IniFile::IniFile()
{
}


IniFile::IniFile(const std::string &path)
{
	load(path);
}


IniFile::~IniFile()
{
}


void IniFile::load(const std::string &path)
{
	this->file = path;
	parseFile(path);
}


void IniFile::parseFile(const std::string &path)
{
	std::ifstream in(path);

	if(!in.is_open()) {
		return;
	}

	std::string buf;

	do {
		std::getline(in, buf);

		if (!buf.empty()) {
			parseLine(buf);
		}
	} while (in);

	in.close();
}


void IniFile::parseLine(const std::string &line)
{
	static std::string name;
	
	if (line[0] == '[') { /* is section */
		int pos = line.find_first_of(']');
		if (pos == std::string::npos || pos == 1) {
			return;
		}
		
		name = std::move(line.substr(1, pos-1));
		if (name.empty()) {
			return;
		}
		
		addSection(name);
	
	} else { /* is property */
		if (name.empty()) {
			name = "global";
		}
		
		int pos = line.find(" = ");
		if (pos == std::string::npos || pos == 0) {
			return;
		}
		
		const std::string &k = line.substr(0, pos);
		const std::string &v = line.substr(pos+3);
		
		Section &s = (*this)[name];
		s.set(k, v);
	}
}


bool IniFile::isEmpty() const
{
	return sections.empty();
}


bool IniFile::contains(const std::string &name) const
{
	const auto &it = std::find(sections.begin(), sections.end(), name);
	
	return it != sections.end();
}


Section& IniFile::get(const std::string &name)
{
	static Section empty("");
	
	auto it = std::find(sections.begin(), sections.end(), name);
	if (it != sections.end()) {
		return *it;
	}
	
	return empty;
}


void IniFile::addSection(const std::string &name)
{
	addSection(Section(name));
}


void IniFile::addSection(const Section &s)
{
	if (contains(s.getName())) {
		return;
	}
	
	sections.push_back(s);
}


void IniFile::save() const
{
	FILE *fp = fopen(file.c_str(), "w");
	
	if (!fp) {
		return;
	}
	
	for (const Section &s : sections) {
		fprintf(fp, "[%s]\n", s.getName().c_str());
		
		const auto &map = s.mapping();
		
		for (const auto &e : map) {
			if (!e.second.isValid()) {
				continue;
			}
			
			fprintf(fp, "%s = ", e.first.c_str());
			
			switch(e.second.getType()) {
			case INT:
				fprintf(fp, "%d\n", e.second.asInt());
				break;
			
			case BOOL:
				fprintf(fp, "%s\n", e.second.asBool() ? "true" : "false");
				break;
			
			case STRING:
				fprintf(fp, "%s\n", e.second.asString().c_str());
				break;
			}
		}
		
		fprintf(fp, "\n");
	}
	
	fclose(fp);
}


Section& IniFile::operator[](const std::string &name)
{
	addSection(name);
	return get(name);
}


/*
int main(int argc, char **argv) {
	IniFile ini("1.ini");
	
	if (!ini["player"].contains("aa")) {
		printf("no aa\n");
		ini["player"]["aa"] = 156;
	} else {
		printf("aa = %d\n", ini["player"]["aa"].asInt());
	}
	
	ini.save();
	return 0;
}
*/

