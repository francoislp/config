// Author: Francois Leduc-Primeau
// Copyright 2013

#include "config.hpp"

#ifdef USE_BOOST_REGEX
#include <boost/regex.hpp>
#else
#include <regex>
#endif

#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>

// Character indicating a line comment
#define COMMENTCHAR '#'

using std::string;
using std::vector;
using std::ifstream;
using std::stringstream;
using boost::filesystem::path;

#ifdef USE_BOOST_REGEX
using boost::regex;
using boost::regex_search;
using boost::smatch;
#else
using std::regex;
using std::regex_search;
using std::smatch;
#endif

config::config()
  : m_checkKeys(false),
    m_filePath(""),
    m_fileName("")
{}

void config::initCL(int argc, char** argv) {
  for(uint i=1; i<argc; i++) {
    string s(argv[i]);
    smatch tokens; // will return the matches as std::string objects

    const string keyValRegex= "([[:alpha:][:digit:]_:]+)[[:space:]]*=[[:space:]]*([^[:space:]]+)";
    const string r2Expr= "--([[:alpha:][:digit:]]+)[[:space:]]*$"; // double [[ is required

#ifdef USE_BOOST_REGEX
    regex r(keyValRegex, boost::regex::extended);
    regex r2(r2Expr, boost::regex::extended);
#else
    regex r(keyValRegex);
    regex r2(r2Expr); 
#endif
    
    if(regex_search(s, tokens, r)) {
      string key = tokens[1];
      if(m_checkKeys && (m_validKeys.find(key) == m_validKeys.cend()))
        throw invalidkey_exception(key);
      m_argMap[ key ] = tokens[2];
    }
    else if(regex_search(s, tokens, r2)) {
      string option = tokens[1];
      if(m_checkKeys && (m_validOptions.find(option) == m_validOptions.cend()))
        throw invalidkey_exception(option);
      m_argMap[ option ] = "";
    }
    else {
      throw syntax_exception(s);
    }
  }
}

void config::initFile(string filepath) {
  m_filePath = filepath;
  path p(filepath);
  m_fileName = p.filename().string();
  ifstream ifs(filepath.c_str());
  if(!ifs.good()) throw file_exception(filepath);

  string curLine;
  while(ifs.good()) {
    getline_nc(ifs, curLine);
    if(curLine != "") {
      // similar code as in initCL(...)
      smatch tokens; // will return the matches as std::string objects
      const string keyValRegex= "([[:alpha:]_:]+)[[:space:]]*=[[:space:]]*([^[:space:]].*)[[:space:]]*$";

#ifdef USE_BOOST_REGEX
      regex r(keyValRegex, boost::regex::extended);
#else
      regex r(keyValRegex);
#endif
      if(regex_search(curLine, tokens, r)) {
        string key = tokens[1];
        if(m_checkKeys && (m_validKeys.find(key) == m_validKeys.cend()))
          throw invalidkey_exception(key);
        string val = tokens[2];
        m_argMap[ key ] = val;
      }
      else {
        throw syntax_exception(curLine);
      }
    }
  }
}

uint config::parseParamUInt(string key) const {
  auto it = m_argMap.find(key);
  if(it != m_argMap.end()) return atoi(it->second.c_str());
  else {
    throw key_not_found(key);
  }
}

double config::parseParamDouble(string key) const {
  auto it = m_argMap.find(key);
  if(it != m_argMap.end()) return atof(it->second.c_str());
  else {
    throw key_not_found(key);
  }
}

bool config::parseParamBool(string key) const {
  auto it = m_argMap.find(key);
  if(it != m_argMap.end()) {
    if(it->second == "1" || it->second == "true") return true;
    else return false;
  }
  else {
    throw key_not_found(key);
  }
}

string config::getParamString(string key) const {
  auto it = m_argMap.find(key);
  if(it != m_argMap.end()) {
    return it->second;
  }
  else {
    throw key_not_found(key);
  }
}

bool config::checkOption(string key) const {
  auto it = m_argMap.find(key);
  if(it != m_argMap.end()) return true;
  return false;
}

bool config::sequenceParser(string key, vector<uint>& seqReturn) const {
  auto it = m_argMap.find(key);
  if(it == m_argMap.end()) throw key_not_found(key);

  seqReturn.clear();
  int start, incr, end;

  smatch tokens;
  const string seqRegex= "([[:digit:]]+):([[:digit:]]+):([[:digit:]]+)";
#ifdef USE_BOOST_REGEX
  regex r(seqRegex, boost::regex::extended);
#else
  regex r(seqRegex);
#endif
  if(regex_search(it->second, tokens, r)) {
    string s1(tokens[1]);
    string s2(tokens[2]);
    string s3(tokens[3]);
    start = atoi(s1.c_str());
    incr  = atoi(s2.c_str());
    end   = atoi(s3.c_str());
  } else {
    return false; // invalid syntax
  }

  // generate sequence
  if(incr==0) {
    seqReturn.push_back(start);
  } else if(incr>0) {
    if(end < start) return false;
    for(int i=start; i<=end; i+= incr) {
      seqReturn.push_back(i);
    }
  } else { //incr<0
    if(start < end) return false;
    for(int i=start; i>=end; i+= incr) {
      seqReturn.push_back(i);
    }
  }
  return true;
}

bool config::sequenceParser(string key, vector<double>& seqReturn) const {
  auto it = m_argMap.find(key);
  if(it == m_argMap.end()) throw key_not_found(key);

  seqReturn.clear();

  // exponential sequence syntax: <start>*<multiplier>:<end>
  const string expSeqRegex= "^([[:digit:]]+(\\.[[:digit:]]+)?)\\*([[:digit:]]+(\\.[[:digit:]]+)?):([[:digit:]]+(\\.[[:digit:]]+)?)$";
  // linear sequence syntax: <start>:<incr>:<end>
  const string linSeqRegex= "^([[:digit:]]+(\\.[[:digit:]]+)?):([[:digit:]]+(\\.[[:digit:]]+)?):([[:digit:]]+(\\.[[:digit:]]+)?)$";
  
#ifdef USE_BOOST_REGEX
  regex r(expSeqRegex, boost::regex::extended);
  regex r2(linSeqRegex, boost::regex::extended);
#else
  regex r(expSeqRegex);
  regex r2(linSeqRegex);
#endif

  smatch tokens;
  if(regex_search(it->second, tokens, r)) {
    // exponential sequence
    string s1(tokens[1]);
    string s2(tokens[3]);
    string s3(tokens[5]);
    double start = atof(s1.c_str());
    double mult  = atof(s2.c_str());
    double end   = atof(s3.c_str());

    // generate sequence
    if(mult<=0) return false;
    for(double x=start; x<=end; x*= mult) {
      seqReturn.push_back(x);
    }
    return true;
  } else if(regex_search(it->second, tokens, r2)) {
    // linear sequence
    string s1(tokens[1]);
    string s2(tokens[3]);
    string s3(tokens[5]);
    double start = atof(s1.c_str());
    double incr  = atof(s2.c_str());
    double end   = atof(s3.c_str());

    // generate sequence
    if(incr==0) {
      seqReturn.push_back(start);
    } else if(incr>0) {
      if(end < start) return false;
      for(double x=start; x<=end; x+= incr) {
        seqReturn.push_back(x);
      }
    } else { //incr<0
      if(start < end) return false;
      for(double x=start; x>=end; x+= incr) {
        seqReturn.push_back(x);
      }
    }
    return true;
  } else {
    // invalid syntax
    return false;
  }
}

bool config::listParser(string key, vector<int>& listReturn) const {
  auto it = m_argMap.find(key);
  if(it == m_argMap.end()) throw key_not_found(key);

  listReturn.clear();

  // check for, and then remove, the curly brackets
  regex r("\\{(.+)\\}");
  smatch regexMatch;
  string rawString= it->second;
  if(regex_search(rawString, regexMatch, r)) {
    vector<string> tokens;
    // the first element in regexMatch is the whole expression, while
    // regexMatch[1] contains the expression within parentheses.
    tokens = split(regexMatch[1], ',', tokens);
    for(int i=0; i<tokens.size(); i++) {
      listReturn.push_back(atoi(tokens[i].c_str()));
    }
    return true;
  } else {
    return false;
  }
}

bool config::listParser(string key, vector<double>& listReturn) const {
  auto it = m_argMap.find(key);
  if(it == m_argMap.end()) throw key_not_found(key);

  listReturn.clear();

  // check for, and then remove, the curly brackets
  regex r("\\{(.+)\\}");
  smatch regexMatch;
  if(regex_search(it->second, regexMatch, r)) {
    vector<string> tokens;
    // the first element in regexMatch is the whole expression, while
    // regexMatch[1] contains the expression within parentheses.
    tokens = split(regexMatch[1], ',', tokens);
    for(int i=0; i<tokens.size(); i++) {
      listReturn.push_back(atof(tokens[i].c_str()));
    }
    return true;
  } else {
    return false;
  }
}

bool config::listParser(string key, vector<string>& listReturn) const {
  auto it = m_argMap.find(key);
  if(it == m_argMap.end()) throw key_not_found(key);

  listReturn.clear();

  // check for, and then remove, the curly brackets
  regex r("\\{(.+)\\}");
  smatch regexMatch;
  if(regex_search(it->second, regexMatch, r)) {
    // the first element in regexMatch is the whole expression, while
    // regexMatch[1] contains the expression within parentheses.
    listReturn = split(regexMatch[1], ',', listReturn);
    return true;
  } else {
    return false;
  }
}

void config::addConfElem(string key, string val) {
  // make sure key does not already exist
  auto it = m_argMap.find(key);
  if(it != m_argMap.end()) throw invalidkey_exception(key);

  m_argMap[ key ] = val;
}

bool config::keyExists(string key) const {
	auto it = m_argMap.find(key);
	return it != m_argMap.end();
}

// Private

void config::getline_nc(ifstream& ifs, string& line) {
  char firstchar = COMMENTCHAR;

  while(firstchar == COMMENTCHAR) {
    if(!ifs.eof()) std::getline(ifs, line);
    else line = "";
    stringstream sstream;
    sstream << line;
    sstream >> std::ws; // extract whitespace
    firstchar = sstream.peek();
  }
  // trim leading and trailing whitespace
  line = trim(line);
}

vector<string>& config::split(const string& s, char delim,
                              vector<string>& elems) const {
  // http://stackoverflow.com/questions/236129/how-to-split-a-string-in-c
  stringstream ss(s);
  string item;
  ss >> std::ws; // extract leading whitespace
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
    ss >> std::ws; // extract leading whitespace of next element
  }
  return elems;
}

// http://stackoverflow.com/questions/25829143/c-trim-whitespace-from-a-string
std::string& config::trim(std::string & str) const
{
   return ltrim(rtrim(str));
}

std::string& config::ltrim(std::string & str) const
{
  auto it2 =  std::find_if( str.begin() , str.end() , [](char ch){ return !std::isspace<char>(ch , std::locale::classic() ) ; } );
  str.erase( str.begin() , it2);
  return str;   
}

std::string& config::rtrim(std::string & str) const
{
  auto it1 =  std::find_if( str.rbegin() , str.rend() , [](char ch){ return !std::isspace<char>(ch , std::locale::classic() ) ; } );
  str.erase( str.begin() + (  str.rend() - it1 ) , str.end() );
  return str;   
}
