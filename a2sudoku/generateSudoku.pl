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
            push(@result, split(/ +/, $line));
        }
        $line = <>;
    }
    
    die "Unexpected known count, " . $#result . " at $." if ($#result != 80);

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
    
    die "Unexpected solution count, " . $#result . " at $." if ($#result != 80);

    return @result;
}


sub generatePuzzle
{
    my $i;
    my $result;

    for ($i = 0; $i <= $#known; $i++) {
        if ($known[$i] ne "_") {
            $solution[$i] += 16;
        }
    }

    $result = pack("C81", @solution);
    
    if (length($result) != 81) {
        die "Unexpected pack length, " . length($result);
    }
    
    return $result;
}

my $destdir=shift;

while (<>) {
    chomp;
    if (/^PUZZLE:/) {
        @known = &getKnown;
    }

    if (/^SOLUTION:/) {
        @solution = &getSolution;
    }

    if (/^RATING: 0/) {
        push(@easy, &generatePuzzle);
    } elsif (/^RATING: 1/) {
        push(@medium, &generatePuzzle);
    } elsif (/^RATING: /) {
        push(@hard, &generatePuzzle);
    }
}

open(FILE, ">$destdir/easy.puzzles.bin") || die "Unable to open easy.puzzles.bin";
print FILE pack("v", $#easy + 1);
print FILE join("", @easy);
close (FILE);

open(FILE, ">$destdir/medium.puzzles.bin") || die "Unable to open medium.puzzles.bin";
print FILE pack("v", $#medium + 1);
print FILE join("", @medium);
close (FILE);

open(FILE, ">$destdir/hard.puzzles.bin") || die "Unable to open hard.puzzles.bin";
print FILE pack("v", $#hard + 1);
print FILE join("", @hard);
close (FILE);
