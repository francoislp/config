// Author: Francois Leduc-Primeau
// Copyright 2013

#include "config.hpp"
#include <regex>
#include <stdlib.h>
#include <fstream>
#include <sstream>

// Character indicating a line comment
#define COMMENTCHAR '#'

using std::string;
using std::vector;
using std::regex;
using std::regex_search;
using std::ifstream;
using std::stringstream;

void config::initCL(int argc, char** argv) {
  for(uint i=1; i<argc; i++) {
    string s(argv[i]);
    std::smatch tokens; // will return the matches as std::string objects
    regex r("([[:alpha:]_]+)[[:space:]]*=[[:space:]]*([^[:space:]]+)");
    regex r2("--([[:alpha:]]+)[[:space:]]*$"); // double [[ is required
    if(regex_search(s, tokens, r)) {
      m_argMap[ tokens[1] ] = tokens[2];
    }
    else if(regex_search(s, tokens, r2)) {
      m_argMap[ tokens[1] ] = "";
    }
    else {
      throw syntax_exception(s);
    }
  }
}

void config::initFile(string filepath) {
  ifstream ifs(filepath.c_str());
  if(!ifs.good()) throw file_exception(filepath);

  string curLine;
  while(ifs.good()) {
    getline_nc(ifs, curLine);
    if(curLine != "") {
      // similar code as in initCL(...)
      std::smatch tokens; // will return the matches as std::string objects
      regex r("([[:alpha:]_]+)[[:space:]]*=[[:space:]]*([^[:space:]]+)");
      if(regex_search(curLine, tokens, r)) {
        m_argMap[ tokens[1] ] = tokens[2];
      }
      else {
        throw syntax_exception(curLine);
      }
    }
  }
}

uint config::parseParamUInt(string key) {
  auto it = m_argMap.find(key);
  if(it != m_argMap.end()) return atoi(it->second.c_str());
  else {
    throw key_not_found(key);
  }
}

double config::parseParamDouble(string key) {
  auto it = m_argMap.find(key);
  if(it != m_argMap.end()) return atof(it->second.c_str());
  else {
    throw key_not_found(key);
  }
}

bool config::parseParamBool(string key) {
  auto it = m_argMap.find(key);
  if(it != m_argMap.end()) {
    if(it->second == "1" || it->second == "true") return true;
    else return false;
  }
  else {
    throw key_not_found(key);
  }
}

bool config::checkOption(string key) {
  auto it = m_argMap.find(key);
  if(it != m_argMap.end()) return true;
  return false;
}

bool config::sequenceParser(string key, vector<uint>& seqReturn) {
  auto it = m_argMap.find(key);
  if(it == m_argMap.end()) throw key_not_found(key);

  seqReturn.clear();
  int start, incr, end;

  std::smatch tokens;
  regex r("([[:digit:]]+):([[:digit:]]+):([[:digit:]]+)");
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

bool config::sequenceParser(string key, vector<double>& seqReturn) {
  auto it = m_argMap.find(key);
  if(it == m_argMap.end()) throw key_not_found(key);

  seqReturn.clear();

  // exponential sequence syntax: <start>*<multiplier>:<end>
  regex r("^([[:digit:]]+(\\.[[:digit:]]+)?)\\*([[:digit:]]+(\\.[[:digit:]]+)?):([[:digit:]]+(\\.[[:digit:]]+)?)$");
  // linear sequence syntax: <start>:<incr>:<end>
  regex r2("^([[:digit:]]+(\\.[[:digit:]]+)?):([[:digit:]]+(\\.[[:digit:]]+)?):([[:digit:]]+(\\.[[:digit:]]+)?)$");

  std::smatch tokens;
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
}
