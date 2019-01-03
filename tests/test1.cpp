#include "config/config.hpp"

#include <string>
#include <iostream>

using std::cerr;
using std::endl;

void setAuthorizedKeys(config& conf) {
	conf.addValidKey("config");
	conf.addValidKey("key_string");
	conf.addValidKey("key_int");
	conf.addValidKey("key_float");
	conf.addValidKey("mylist");
	conf.addValidKey("key2");
}

int main(int argc, char** argv) {
	config conf;

	setAuthorizedKeys(conf);

	// parse the command line arguments
  try {
    conf.initCL(argc, argv);
  } catch(syntax_exception& e) {
    cerr << "Invalid syntax in cmd-line arguments: "<<e.what()<<endl;
    return 1;
  } catch(invalidkey_exception& e) {
    cerr << "Invalid key or option: "<<e.what()<<endl;
    return 1;
  }

  // if a configuration file is specified, load its content
  try {
    std::string confpath = conf.getParamString("config");
    conf.initFile(confpath);
  } catch(key_not_found& e) {
	  //cerr << "Warning: No configuration file specified, using command line arguments only." << endl;
  } catch(invalidkey_exception& e) {
    cerr << "Invalid key: "<<e.what()<<endl;
    return 1;
  } catch(file_exception& e) {
    cerr << "Error reading from file at " << e.what() << endl;
    return 1;
  } catch(syntax_exception& e) {
    cerr << "Invalid syntax: "<<e.what() << endl;
    return 1;
  }

  if(conf.getParamString("key_string") != "val" ||
     conf.parseParamUInt("key_int") != 42 ||
     conf.parseParamDouble("key_float") != 3.14159) {
	  cerr<< "TEST FAILS!" <<endl;
	  return 1;
  }

  if(conf.parseParamUInt("key2") != 99) {
	  cerr<< "TEST FAILS!" <<endl;
	  return 1;
  }	  

  std::vector<int> mylist;
  if(!conf.listParser("mylist", mylist)) {
	  cerr<< "TEST FAILS!" <<endl;
	  return 1;
  }
  if(mylist.size() != 5 ||
     mylist[0] != 5 ||
     mylist[1] != 4 ||
     mylist[2] != 3 ||
     mylist[3] != 2 ||
     mylist[4] != 1) {
	  cerr<< "TEST FAILS!" <<endl;
	  return 1;
  }

  cerr<< "TEST PASS" <<endl;
  return 0;
}
