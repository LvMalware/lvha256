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

#colors
use constant {
    RED     => "\033[0;31m",
    BLUE    => "\033[0;34m",
    GREEN   => "\033[0;32m",
    YELLOW  => "\033[1;33m",
    DEFAULT => "\033[0m"
};

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

#status
my $st_0 = DEFAULT . "[" . GREEN . "+" . DEFAULT . "]";
my $st_1 = DEFAULT . "[" . RED   . "-" . DEFAULT . "]";


if ($input)
{
    open(my $hashes, "< encoding(UTF-8)", $input)
        || die RED . "$0: can't open $input for reading: $!" . DEFAULT;
    while (my $hash = <$hashes>)
    {
        chomp($hash);
        my $password = crack_hash($hash);
        if (defined($password))
        {
            print "$st_0 Found password for ", YELLOW, $hash, DEFAULT, ": ";
            print "$password\n";
        }
        else
        {
            print "$st_1 No password found for ", YELLOW, $hash, DEFAULT, "\n";
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
            print "$st_0 Found password for ", YELLOW, $hash, DEFAULT, ": ";
            print "$password\n";
        }
        else
        {
            print "$st_1 No passwod found for: ", YELLOW, $hash, DEFAULT, "\n";
        }
    }
}

sub crack_hash
{
    my $hash = shift;
    print "$st_0 Cracking hash : ", YELLOW, "$hash\n"   unless $silent;
    print "$st_0 Using wordlist: ", YELLOW, "$wordlist\n" unless $silent;
    open(my $words, "< :encoding(UTF-8)", $wordlist)
        || die RED . "$0: can't open $wordlist for reading: $!" . DEFAULT;

    print "$st_0 Wordlist size : " unless $silent;
    my $size_bytes = -s $wordlist;
    print BLUE, "$size_bytes", DEFAULT," bytes\n" unless $silent;
    my $pass_bytes = 0;
    my $progress   = sprintf("%.2f%%",  100 * $pass_bytes / $size_bytes);
    print "$st_0 Progress: ", BLUE, $progress unless $silent;
    
    while (my $password = <$words>)
    {
        $pass_bytes += length($password);
        chomp($password);
        unless ($silent)
        {
            print "\b" x length($progress);
            $progress = sprintf("%.2f%%", 100 *  $pass_bytes / $size_bytes);
        }
        my $pwd = quotemeta($password);
        chomp(my $pw_hash = `./lvha256sum -s $pwd`);
        if ($pw_hash eq $hash)
        {
            close($words);
            print DEFAULT, "\n" unless $silent;
            return $password;
        }
        print $progress unless $silent;
    }
    close $words;
    print DEFAULT,"\n" unless $silent;
    undef;
}
