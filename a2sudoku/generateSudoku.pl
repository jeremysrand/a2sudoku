#!/usr/bin/perl -w
#
# To use this generator, first you need node.js and this
# sudoku generator:
#
#    https://github.com/dachev/sudoku
#
# You should be able to get it with:
#
#     sudo npm install sudoku
#
# Once you have that, you can generate 300 puzzles fairly
# quickly with the following from the shell:
#
#     i=1 
#     while [ $i -lt 300 ]
#     do
#         node lib/node_modules/sudoku/bin/sudoku.js
#         i=`expr $i + 1`
#     done > /tmp/puzzles.txt
#
# But the output of this isn't ready for a2sudoku.  To
# turn it into the right definitions for a2sudoku, you
# need to pass it through this script:
#
#     generateSudoku.pl /tmp/puzzles.txt > /tmp/newpuzzles.c
#
# Then, you just need to copy those puzzles into puzzles.c
# in the source-code and you have refreshed the puzzles.
#


use strict;


my @known;
my @solution;
my @easy;
my @medium;
my @hard;


sub getKnown
{
    my @result;
    my $i;
    my $j;
    my $line;

    for ($i = 0; $i < 3; $i++) {
        for ($j = 0; $j < 3; $j++) {
            $line = <>;
            chomp $line;
            $line =~ s/_/P/g;
            $line =~ s/[1-9]/S/g;
            push(@result, split(/ +/, $line));
        }
        $line = <>;
    }

    return @result;
}


sub getSolution
{
    my @result;
    my $i;
    my $j;
    my $line;

    for ($i = 0; $i < 3; $i++) {
        for ($j = 0; $j < 3; $j++) {
            $line = <>;
            chomp $line;
            push(@result, split(/ +/, $line));
        }
        $line = <>;
    }

    return @result;
}


sub generatePuzzle
{
    my $i;
    my $result = 
"    {\n" .
"        {\n" .
"            ";

    for ($i = 0; $i <= $#known; $i++) {
        $result .= $known[$i] . "VAL(" . $solution[$i] . "),";

        if ($i == $#known) {
            $result .= "\n        }\n    },\n";
        } elsif (($i % 27) == 26) {
            $result .= "\n\n            ";
        } elsif (($i % 9) == 8) {
            $result .= "\n            ";
        } elsif (($i % 3) == 2) {
            $result .= " ";
        }
    }

    return $result;
}


# Turns out I run out of memory pretty quickly.  I should look at
# putting the puzzles on disk and loading them rather than building
# them in.  For now, enforce a limit to make sure we don't exceed
# limits.
my $limit = 50;
while (<>) {
    chomp;
    if (/^PUZZLE:/) {
        @known = &getKnown;
    }

    if (/^SOLUTION:/) {
        @solution = &getSolution;
    }

    if ((/^RATING: 0/) &&
        ($#easy < $limit)) {
        push(@easy, &generatePuzzle);
    } elsif ((/^RATING: 1/) &&
             ($#medium < $limit)) {
        push(@medium, &generatePuzzle);
    } elsif ((/^RATING: /) && 
             ($#hard < $limit)) {
        push(@hard, &generatePuzzle);
    }
}


print "tPuzzle easyPuzzles[] = {\n" . join("", @easy) . "};\n\n\n";
print "tPuzzle mediumPuzzles[] = {\n" . join("", @medium) . "};\n\n\n";
print "tPuzzle hardPuzzles[] = {\n" . join("", @hard) . "};\n\n\n";
