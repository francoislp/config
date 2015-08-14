//Author: Francois Leduc-Primeau
//Copyright 2013

#ifndef _config_hpp_
#define _config_hpp_

#include "CustomExceptions.hpp"
#include <string>
#include <map>
#include <vector>
#include <unordered_set>

typedef unsigned int uint;

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

  uint   parseParamUInt(std::string key) const;

  double parseParamDouble(std::string key) const;

  bool   parseParamBool(std::string key) const;

  std::string getParamString(std::string key) const;

  /**
   * Returns true if the option has been specified, false
   * otherwise. Only the option name must be provided as the key,
   * without the "--".
   */
  bool checkOption(std::string key) const;

  /**
   * Parses syntax describing a sequence of integers, and adds each element in the
   * sequence to "seqReturn".
   *@param key     Name of the key associated with the desired sequence.
   *@param seqReturn  This vector is cleared and elements are added to it.
   *@return 'true' if a valid sequence was found, 'false' otherwise (in that case 
   *        seqReturn will be empty)
   */
  bool sequenceParser(std::string key, std::vector<uint>& seqReturn) const;

  /**
   * Parses syntax describing a sequence of real numbers, and adds each element
   * in the sequence to "seqReturn".
   *@param key     Name of the key associated with the desired sequence.
   *@param seqReturn  This vector is cleared and elements are added to it.
   *@return 'true' if a valid sequence was found, 'false' otherwise (in that case 
   *        seqReturn will be empty)
   */
  bool sequenceParser(std::string key, std::vector<double>& seqReturn) const;

  /**
   * Parses a string describing a list, of the form "{<item1>,
   * <item2>, ...}". Each element in the list is parsed as an integer.
   *@param key        Name of the key for the desired list.
   *@param listReturn This vector is cleared and elements are added to it.
   *@return 'true' if a valid list is found, 'false' otherwise (in that case 
   *        "listReturn" will be empty)
   */
  bool listParser(std::string key, std::vector<int>& listReturn) const;

  /**
   * Parses a string describing a list, of the form "{<item1>,
   * <item2>, ...}". Each element in the list is parsed as a
   * double-precision floating-point number.
   *@param key        Name of the key for the desired list.
   *@param listReturn This vector is cleared and elements are added to it.
   *@return 'true' if a valid list is found, 'false' otherwise (in that case 
   *        "listReturn" will be empty)
   */
  bool listParser(std::string key, std::vector<double>& listReturn) const;

  /**
   * Parses a string describing a list, of the form "{<item1>,
   * <item2>, ...}". Each element in the list is parsed as a string.
   *@param key        Name of the key for the desired list.
   *@param listReturn This vector is cleared and elements are added to it.
   *@return 'true' if a valid list is found, 'false' otherwise (in that case 
   *        "listReturn" will be empty)
   */
  bool listParser(std::string key, std::vector<std::string>& listReturn) const;

  /**
   * Returns the file path that was passed to initFile(), or an empty
   * string if initFile() was never called.
   */
  std::string getFilePath() const { return m_filePath; }

  /**
   * Returns the name of the file passed to initFile(), without any
   * preceeding directories, or an empty string if initFile() was
   * never called.
   */
  std::string getFileName() const { return m_fileName; }

  /**
   * Adds a new string element to the configuration.
   *@throws invalidkey_exception if the key already exists
   */
  void addConfElem(std::string key, std::string val);

	/**
	 * Checks whether the specified key exists in the configuration.
	 */
	bool keyExists(std::string key) const;

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
  std::vector<std::string>& split(const std::string& s, char delim, std::vector<std::string>& elems) const;

	std::string& trim(std::string& str) const;
	std::string& ltrim(std::string& str) const;
	std::string& rtrim(std::string& str) const;

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
