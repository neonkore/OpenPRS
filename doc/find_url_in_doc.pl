#!/usr/local/bin/perl 

#
# This utility is used internaly by xoprs and the op-editor to find the proper
# URL in the documentation corresponding to a particular Help button in the
# Motif interface. 
#


die "No arg or too many args!\n" unless @ARGV==2 ;

$URL = $ARGV[0];

require("$URL/labels.pl");

$where = $external_labels{$ARGV[1]};

if ($where) {
  print "$external_labels{$ARGV[1]}#$ARGV[1]\n";
} else {
  print "$URL/oprs.html\n";
}
