# Functions to parse a configuration file and retrieve elements from it.

# Note that module Config is part of the perl installation and
# therefore this name cannot be used.
package ConfigParser;

require Exporter;
@ISA = qw(Exporter);

# available functions:
@EXPORT_OK = qw(parseConfig getKey getKeyBool keyExists getList printKey_tcl printKey_tclnoqw);

use strict;

# Parses a configuration file and returns a key-value hash table.
# Args:
#  $_[0] Path to the configuration file.
sub parseConfig {
  my $filepath = shift;
  open(FH, $filepath) || die("Could not open ".$filepath.".\n");

  my %config;
  my $line;
  my $linecnt = 1;
  while( defined($line=getLineNC(*FH)) ) {
    # Note: The key cannot contain spaces. Must make sure to exclude
    #       whitespace from the key string.
    if( $line =~ /^\s*([^\s]+)\s*=\s*(.+)$/ ) {
      my $key   = $1;
      my $value = $2;
      # convert keywords that refer to boolean values
      $value=1 if(lc($value) eq "true");
      $value=0 if(lc($value) eq "false");
      $config{$key} = $value;
    } else {
      # syntax error unless line contains only whitespace
      $line =~ /^\s*$/ ||
        die("Unexpected syntax at non-comment line #".$linecnt." of config file.\n");
    }
    $linecnt++;
  }
  close(FH);

  return %config;
}

# Retrieves the value associated with a key.
# Args
#   [0]: key name
#   [1]: config table as a hash reference
sub getKey {
  my $key = $_[0];
  my %config = %{$_[1]};

  defined($config{$key}) || die("Missing conf key: ".$key."\n");
  return $config{$key};
}

# Retrieves the boolean value associated with a key.
# Args
#   [0]: key name
#   [1]: config table as a hash reference
# Returns: 1 if the key has a true value, 0 otherwise
sub getKeyBool {
  my $val= getKey(@_);
  if( $val =~ /^\s*1\s*$/ || $val =~ /^\s*true\s*$/i ) {
    return 1;
  } elsif( $val =~ /^\s*0\s*$/ || $val =~ /^\s*false\s*$/i ) {
    return 0;
  } else {
    die("Invalid boolean value for key: ".$_[0]."\n");
  }
}

# Checks if a particular key exists in the configuration.
# Args
#   [0]: key name
#   [1]: config table as a hash reference
# Returns: true if the key exists, false otherwise.
sub keyExists {
  my $key = $_[0];
  my %config = %{$_[1]};

  return defined($config{$key});
}

# Retrieves the list corresponding to a key.
# Args
#   [0]: key name
#   [1]: config table as a hash reference
# Returns: An array.
sub getList {
  my $key = $_[0];
  my $curList = getKey($key, $_[1]);
  # Remove curly braces
  if($curList =~ /\{(.+)\}/) {
    $curList = $1;
  } else {
    die("Syntax error for key \"$key\" in config file.");
  }
  # Tokenize
  my @values = split(/\s*,\s*/, $curList);
  return @values;
}

# Print a TCL script line that sets a variable to the value specified
# in the configuration table. The value is printed with surrounding 
# double-quotes.
# Args:
#   [0]  File handle (prefix with '*')
#   [1]  key name
#   [2]  config table as a hash reference
sub printKey_tcl {
  my $fh = $_[0];
  my $key = $_[1];
  my %config = %{$_[2]};

  defined($config{$key}) || die("Missing conf key: ".$key."\n");
  print $fh "set ".$key." \"".$config{$key}."\"\n";
}

# Same as printKey_tcl, but doesn't add quotes around the value.
sub printKey_tclnoqw {
  my $fh = $_[0];
  my $key = $_[1];
  my %config = %{$_[2]};

  defined($config{$key}) || die("Missing conf key: ".$key."\n");
  print $fh "set ".$key." ".$config{$key}."\n";
}

# --------------------------------------------------
#                Private Subroutines
# --------------------------------------------------

# Retrieves the next line from a file but ignores comment lines
# Arg: 
#  [0]  File handle to the file (syntax: "getLineNC(*FILEHANDLE)")
sub getLineNC {
  my $fh = shift;
  my $commentchar = "#";
  my $line = <$fh>;
  while( defined($line) && $line =~ /^\s*$commentchar/ ) {
    $line = <$fh>;
  }
  return $line;
}
