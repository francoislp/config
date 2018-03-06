#ifndef CustomExceptions_hpp_
#define CustomExceptions_hpp_

#include <exception>
#include <string>
#include <sstream>

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

#endif
