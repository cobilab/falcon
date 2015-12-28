#!/usr/bin/env perl
# IT DOWNLOADS REFERENCE GENOMES USING THE NAMES FROM FILE
use strict;
use warnings;
use LWP::Simple;


my ($name, $outname, $url, $xml, $out, $count, $query_key, $webenv, $ids);
my @genomeId;
my $base = 'http://eutils.ncbi.nlm.nih.gov/entrez/eutils/';
my $limit = 'srcdb+refseq[prop]+AND+gene+in+chromosome[prop])';

my @species;
open(my $fh, "<", $ARGV[0])
    or die "Failed to open file: $!\n";
while(<$fh>) { 
    chomp; 
    push @species, $_;
} 
close $fh;

print join " ", @species;

foreach my $s (@species) {
  print STDERR "Downloading $s ... \n";
  undef @genomeId;
  $query_key = $webenv = '';
  $s =~ s/ /+/g;
  # ESearch
  $url = $base . "esearch.fcgi?db=genome&term=$s";
  $xml = get($url);
  $count = $1 if ($xml =~ /<Count>(\d+)<\/Count>/);
  if ($count > 20) {
    $url = $base . "esearch.fcgi?db=genome&term=$s&retmax=$count";
    $xml = get($url);
    }
  while ($xml =~ /<Id>(\d+?)<\/Id>/gs) {
    push(@genomeId, $1);
    }
  $ids = join(',', @genomeId);
  # ELink
  $url = $base . "elink.fcgi?dbfrom=genome&db=nuccore&cmd=neighbor_history&id=$ids&term=$limit";
  $xml = get($url);
  $query_key = $1 if ($xml =~ /<QueryKey>(\d+)<\/QueryKey>/);
  $webenv = $1 if ($xml =~ /<WebEnv>(\S+)<\/WebEnv>/);
  # EFetch
  $url = $base . "efetch.fcgi?db=nuccore&query_key=$query_key&WebEnv=$webenv&rettype=fasta&retmode=text";
  $out = get($url);
  open (FOUT, ">$s.fna");
  print FOUT $out;
  close FOUT;
  }
