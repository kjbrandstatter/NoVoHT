#!/usr/bin/perl
use strict;
use warnings;
use Getopt::Long;

my $in = '';
my $out = '';
my $type = 'UNDEF';
my $result = GetOptions ("in=s" => \$in,
                        "out=s" => \$out,
                        "type=s" => \$type);
open LOG, "<$in", or *LOG = *STDIN;
open CSV, ">>", $out, or *CSV = *STDOUT;

sub parray {
   print CSV "$type,$_[0],$_[1],$_[2],$_[3]\n";
}
my @entry;
while (<LOG>){
   chomp;
   if ($_ =~ m/^(\d+)/i){
      $entry[0] = $1;
   }
   if ($_ =~ m/^Insertion[^\d]*(\d+\.?\d*)/){
      $entry[1] = $1;
   }
   if ($_ =~ m/^Retr[^\d]*(\d+\.?\d*)/){
      $entry[2] = $1;
   }
   if ($_ =~ m/^Removal[^\d]*(\d+\.?\d*)/){
      $entry[3] = $1;
      parray @entry;
   }
}
close LOG;
close CSV;
