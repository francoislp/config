//Author: Francois Leduc-Primeau
//Copyright 2013

#ifndef _config_hpp_
#define _config_hpp_

#include <string>
#include <map>
#include <vector>

typedef unsigned int uint;

class key_not_found : public std::exception {
public:
  key_not_found(std::string& keyName)
    : m_keyName(keyName) {}

  //note: the exception specification "throw()" is required because it
  //      is declared in the parent class std::exception.
  ~key_not_found() throw() {}

  const char* what() const throw() { return m_keyName.c_str(); }

private:
  std::string m_keyName;
};

/**
 * Parsing and retrieving values from a configuration.
 */
class config {

public:

  config();

  /**
   * Initialize the configuration from an array of c-strings (as when
   * passing arguments on the Command Line). Each element can have one
   * of two types: "key-value pair", or "option". A key-value pair is
   * "<key>=<value>", and an option is "--<option>".
   *
   *@param argc The number of elements in argv.
   *@param argv Array of c-strings.
   *@return True if parsing was successful, false otherwise.
   */
  bool initCL(int argc, char** argv);

  //TODO: in the future, could support initializing from a file

  uint   parseParamUInt(std::string key);

  double parseParamDouble(std::string key);

  bool   parseParamBool(std::string key);

  /**
   * Returns true if the option has been specified, false
   * otherwise. Only the option name must be provided as the key,
   * without the "--".
   */
  bool checkOption(std::string key);

  /**
   * Parses syntax describing a sequence of integers, and adds each element in the
   * sequence to "seqReturn".
   *@param key     Name of the key associated with the desired sequence.
   *@param seqReturn  This vector is cleared and elements are added to it.
   *@return 'true' if a valid sequence was found, 'false' otherwise (in that case 
   *        seqReturn will be empty)
   */
  bool sequenceParser(std::string key, std::vector<uint>& seqReturn);

  /**
   * Parses syntax describing a sequence of real numbers, and adds each element
   * in the sequence to "seqReturn".
   *@param key     Name of the key associated with the desired sequence.
   *@param seqReturn  This vector is cleared and elements are added to it.
   *@return 'true' if a valid sequence was found, 'false' otherwise (in that case 
   *        seqReturn will be empty)
   */
  bool sequenceParser(std::string key, std::vector<double>& seqReturn);


private:

  std::map<std::string, std::string> m_argMap;
};

#endif
