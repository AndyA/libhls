#!/usr/bin/env perl

use strict;
use warnings;

use Getopt::Long;
use JSON;

my %O = ( name => 'json' );

GetOptions( 'n:s' => \$O{name} ) or die;

my $json = JSON->new->canonical->utf8;
STDOUT->binmode(':utf8');

my $doc = $json->encode(
  $json->decode(
    do { local $/; <> }
  )
);

print "const char *$O{name} =\n";
while ( $doc =~ s/(.{1,60})// ) {
  ( my $ln = $1 ) =~ s/([\\\"])/\\$1/g;
  print qq{  "$ln"};
  print ';' unless length $doc;
  print "\n";
}

# vim:ts=2:sw=2:sts=2:et:ft=perl

