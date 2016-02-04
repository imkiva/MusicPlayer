#pragma once

#include <algorithm>
#include <string>
#include <vector>
#include <map>

namespace kiva {

enum ValueType
{
	INT, STRING, BOOL, NONE
};

class IniValue
{
private:
	ValueType type;
	std::string str;
	union {
		int intValue;
		bool boolValue;
	} value;
	
public:
	IniValue();
	IniValue(int i);
	IniValue(bool b);
	IniValue(const std::string &str);
	~IniValue();
	
	IniValue& operator=(int i);
	IniValue& operator=(bool b);
	IniValue& operator=(const std::string &str);
	
	bool isValid() const;
	ValueType getType() const;
	
	bool asBool() const;
	int asInt() const;
	const std::string& asString() const;
};


class Section
{
private:
	std::string name;
	std::map<std::string, IniValue> data;

public:
	Section(const std::string &name);
	Section(const std::string &name, const Section &o);
	~Section();
	
	int size() const;
	
	bool contains(const std::string &key) const;
	bool isEmpty() const;
	
	void set(const std::string &key, const IniValue &value);
	IniValue& get(const std::string &key);
	
	const std::string& getName() const;
	void setName(const std::string &name);
	
	const std::map<std::string, IniValue>& mapping() const;
	
	Section& operator=(const Section &o);
	
	IniValue& operator[](const std::string &key);
	
	bool operator==(const Section &o) const;
	bool operator==(const std::string &name) const;
};


class IniFile
{
private:
	std::vector<Section> sections;
	std::string file;
	
	void parseFile(const std::string &path);
	void parseLine(const std::string &line);

public:
	IniFile();
	IniFile(const std::string &path);
	~IniFile();
	
	void load(const std::string &path);
	
	bool isEmpty() const;
	bool contains(const std::string &name) const;
	
	Section& get(const std::string &name);
	void addSection(const std::string &name);
	void addSection(const Section &s);
	
	void save() const;
	
	Section& operator[](const std::string &name);
};

}
