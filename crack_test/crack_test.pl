#!/usr/bin/env perl

use utf8;
use strict;
use warnings;
use Getopt::Long;

use vars qw ( $VERSION );

$VERSION = "2020.0228.1030";

#Try a brute force attack on a given hash using the given password list

my $wordlist = '/usr/share/dict/words';
my $version;
my $silent;
my $input;
my $help;

GetOptions(
    "wordlist=s" => \$wordlist,
    "version!"   => \$version,
    "silent"     => \$silent,
    "input"      => \$input,
    "help!"      => \$help
);

if ($version)
{
    print "$VERSION\n";
    exit(0);
}

if ($help or (@ARGV == 0 and !$input))
{
    print <<HELP;

CrackTest - try breaking a LVHA256 hashed password with a brute force attack

Usage: crack_test.pl [options(s)] <hash0> <hash1> ... <hashN>
-v, --version   show program's version number and exit.
-h, --help      show this help message and exit.
-w, --wordlist  specify an wordlist to read passwords from
                if no wordlist is specified, it will use /usr/share/dict/words
-s, --silent    don't output status messages
-i, --input     specify a file to read the hashes from

This tool should be used only for test purposes.
Copyright(C) 2020 Lucas V. Araujo <lucas.vieira.ar\@disroot.org>
GitHub: https://github.com/LvMalware/lvha256

HELP
    exit(0);
}

if ($input)
{
    open(my $hashes, "< encoding(UTF-8)", $input)
        || die "$0: can't open $input for reading: $!";
    while (my $hash = <$hashes>)
    {
        chomp($hash);
        my $password = crack_hash($hash);
        if (defined($password))
        {
            print "[+] Found password for $hash : $password\n";
        }
        else
        {
            print "[-] Password for $hash not found.\n";
        }
    }
}
else
{
    for my $hash (@ARGV)
    {
        my $password = crack_hash($hash);
        if (defined($password))
        {
            print "[+] Found password for $hash : $password\n";
        }
        else
        {
            print "[-] Password for $hash not found.\n";
        }
    }
}


sub crack_hash
{
    my $hash = shift;
    print "[+] Trying to crack the hash $hash ...\n" unless $silent;
    print "[+] Reading passwords from $wordlist\n\n" unless $silent;
    open(my $words, "< :encoding(UTF-8)", $wordlist)
        || die "$0: can't open $wordlist for reading: $!";
    while (my $password = <$words>)
    {
        chomp($password);
        print "[+] Trying:  $password\n" unless $silent;
        my $pwd = quotemeta($password);
        chomp(my $pw_hash = `./lvha256sum -s $pwd`);
        if ($pw_hash eq $hash)
        {
            close($words);
            return $password;
        }
    }
    close $words;
    undef;
}