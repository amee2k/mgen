#!/usr/bin/perl

use warnings;
use strict;

my $characters = '';

print "#define _CHARDB_INTERNAL 1\n";
print "#include \"chardb.h\"\n\n";
print "uint8_t charsigns[] = {\n";

my $maxlen = 5 * 8 + 1;

while(<>) {
	# strip comments
	s/\/\/.+$//;
	next if /^[ \t]*$/;
	my($c, $m) = /(.)[ \t]+([.-]+)/;
	next unless defined $m;

	$characters .= $c;

	my $l = length($m);

	my $rep = '';
	foreach( split m//, $m ) {
		$rep .= ($_ eq '.' ? 'DIT(' : 'DAH(');
	}
	$rep .= '0';
	$rep .= ')' x $l;

	my $pad = ' ' x ($maxlen - length($rep));

	print "\tCODEPOINT( $rep,$pad $l),\t// $c\n";
}

$characters =~ s/"/\\"/;

print "};\n\n";
print "char charmap[] = \"$characters\";\n";
print "int chardbsize = sizeof(charmap) + sizeof(charsigns);\n";


