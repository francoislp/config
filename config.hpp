//Author: Francois Leduc-Primeau
//Copyright 2013

#ifndef _config_hpp_
#define _config_hpp_

#include <exception>
#include <string>
#include <map>
#include <vector>
#include <unordered_set>

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

class syntax_exception : public std::exception {
public:
  syntax_exception(std::string& offender)
    : m_offender(offender) {}

  ~syntax_exception() throw() {}

  const char* what() const throw() { return m_offender.c_str(); }

private:
  std::string m_offender;
};

class file_exception : public std::exception {
public:
  file_exception(std::string& filepath)
    : m_filepath(filepath) {}

  ~file_exception() throw() {}

  /// Returns the filepath of the file that triggered the exception.
  const char* what() const throw() { return m_filepath.c_str(); }

private:
  std::string m_filepath;
};

class invalidkey_exception : public std::exception {
public:
  invalidkey_exception(std::string& keyName)
    : m_keyName(keyName) {}

  ~invalidkey_exception() throw() {}

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
   * Initializes the configuration from an array of c-strings (as when
   * passing arguments on the Command Line). Each element can have one
   * of two types: "key-value pair", or "option". A key-value pair is
   * "<key>=<value>", and an option is "--<option>".
   *
   *@param argc The number of elements in argv.
   *@param argv Array of c-strings.
   *@throws invalidkey_exception  If a key or option is deemed invalid.
   *@throws syntax_exception      If some element has invalid syntax.
   */
  void initCL(int argc, char** argv);

  /**
   * Initializes the configuration from a file. Keys that already exist
   * are not deleted, but the values in the file takes precedence. Only
   * the "key-value pair" syntax is allowed in the file, as well as
   * comment lines starting with "#".
   *@param filepath
   *@throws file_exception        If there is a problem reading from the file
   *                              specified by "filepath".
   *@throws invalidkey_exception  If a key is deemed invalid.
   *@throws syntax_exception      If some line has invalid syntax.
   */
  void initFile(std::string filepath);

  /**
   * Defines key "key" as being valid in a configuration. Calling this
   * method automatically activates key checking, and an exception
   * will be thrown if invalid keys are encountered while parsing a
   * configuration.
   */
  void addValidKey(std::string key) { 
    m_checkKeys=true;
    m_validKeys.insert(key);
  }

  void addValidOption(std::string option) {
    m_checkKeys=true;
    m_validOptions.insert(option);
  }

  uint   parseParamUInt(std::string key);

  double parseParamDouble(std::string key);

  bool   parseParamBool(std::string key);

  std::string getParamString(std::string key);

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

  /**
   * Parses a string describing a list, of the form "{<item1>,
   * <item2>, ...}". Each element in the list is parsed as an integer.
   *@param key        Name of the key for the desired list.
   *@param listReturn This vector is cleared and elements are added to it.
   *@return 'true' if a valid list is found, 'false' otherwise (in that case 
   *        "listReturn" will be empty)
   */
  bool listParser(std::string key, std::vector<int>& listReturn);

  /**
   * Parses a string describing a list, of the form "{<item1>,
   * <item2>, ...}". Each element in the list is parsed as a
   * double-precision floating-point number.
   *@param key        Name of the key for the desired list.
   *@param listReturn This vector is cleared and elements are added to it.
   *@return 'true' if a valid list is found, 'false' otherwise (in that case 
   *        "listReturn" will be empty)
   */
  bool listParser(std::string key, std::vector<double>& listReturn);

  /**
   * Parses a string describing a list, of the form "{<item1>,
   * <item2>, ...}". Each element in the list is parsed as a string.
   *@param key        Name of the key for the desired list.
   *@param listReturn This vector is cleared and elements are added to it.
   *@return 'true' if a valid list is found, 'false' otherwise (in that case 
   *        "listReturn" will be empty)
   */
  bool listParser(std::string key, std::vector<std::string>& listReturn);

  /**
   * Returns the file path that was passed to initFile(), or an empty
   * string if initFile() was never called.
   */
  std::string getFilePath() { return m_filePath; }

  /**
   * Returns the name of the file passed to initFile(), without any
   * preceeding directories, or an empty string if initFile() was
   * never called.
   */
  std::string getFileName() { return m_fileName; }

  /**
   * Adds a new string element to the configuration.
   *@throws invalidkey_exception if the key already exists
   */
  void addConfElem(std::string key, std::string val);

	/**
	 * Checks whether the specified key exists in the configuration.
	 */
	bool keyExists(std::string key);

private:

  /**
   * Returns the next line from the ifstream object, skipping over
   * comment lines.
   *@param ifs
   *@param line  Set to the next non-comment line, or to an empty 
   *             string if the end of stream has been reached.
   */
  void getline_nc(std::ifstream& ifs, std::string& line);

  /**
   * Tokenizes a string according to the delimiter specified. The
   * tokens are added to "elems".
   */
  std::vector<std::string>& split(const std::string& s, char delim, std::vector<std::string>& elems);

  // ---------- Data Members ----------

  std::map<std::string, std::string> m_argMap;

  /**
   * When this is true, config keys and options are checked against
   * the set m_validKeys and m_validOptions, respectively.
   */
  bool m_checkKeys;

  std::unordered_set<std::string> m_validKeys;
  std::unordered_set<std::string> m_validOptions;

  /// File path that was passed to initFile(), or an empty string.
  std::string m_filePath;

  /**
   * File name that was passed to initFile(), excluding any preceeding
   * directories. Empty string if uninitialized.
   */
  std::string m_fileName;
};

#endif
