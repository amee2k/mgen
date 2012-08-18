#!/usr/bin/perl

use warnings;
use strict;

# * A 2 .-
# * B 4 -...
# * C 4 -.-.
#
#	CODEPOINT( DIT(DAH(0)),								2),	// A
#	CODEPOINT( DAH(DIT(DIT(DIT(0)))),					4),	// B

my $characters = '';

print "static uint8_t charsigns[] = {\n";

while(<>) {
	my($c, $l, $m) = /(.) ([1-7]) ([.-]+)/;
	next unless defined $m;

	$characters .= $c;

	my $rep = '';
	foreach( split m//, $m ) {
		$rep .= ($_ eq '.' ? 'DIT(' : 'DAH(');
	}
	$rep .= '0';
	$rep .= ')' x length($m);

	$l = length($m);

	print "\tCODEPOINT( $rep, $l);\t// $c\n";
}

$characters =~ s/"/\\"/;
$characters =~ s/'/\\'/;

print "};\n\n";
print "static char charmap[] = \"$characters\";\n";



