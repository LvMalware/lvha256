#!/usr/bin/env perl

use utf8;
use strict;
use warnings;

#Try to create colisions with strings of one character
#The goal is test if two character's have the same hash

my $count = 0;
for my $o ('A' .. 'Z', 'a' .. 'z', 0 .. 9)
{
    chomp(my $digest = `./lvha256sum '$o'`);
    for my $c ('A' .. 'Z', 'a' .. 'z', 0 .. 9)
    {
        next if $o eq $c; #ensure that it won't try the same character.
        chomp(my $digest2 = `./lvha256sum '$c'`);
        if ($digest2 eq $digest)
        {
            $count ++;
            print "Detected collision of character $o ($digest)\n".
                  "with $c ($digest2)\n";
        }
    }
}

print "Done. $count collisions detected.\n";