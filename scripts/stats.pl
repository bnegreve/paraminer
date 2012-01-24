#!/usr/bin/perl
# reads column formated numeric data on stdin, and prints some simple statistics
# eg. 
# ./stats.pl < 
# 10 2 100
# 15 3 100

# outputs 
# COLUMN 0 : SUM = 25, SIZ = 2, AVG = 12.5, STD = 2.5
# COLUMN 1 : SUM = 5, SIZ = 2, AVG = 2.5, STD = 0.5
# COLUMN 2 : SUM = 200, SIZ = 2, AVG = 100, STD = 0
# where SUM is the sum of all elements in the column 
# SIZ is the # on elements in one columns 
# AVG is the average 
# STD is the standard deviation 

#comments in input file are transmited to error output
@transposed = (); 

# $line = <STDIN> or die $!; 
# @input = split(/ /, $line);

while($line = <STDIN>){
    if($line =~ /^\s*#/){print STDERR $line; next;}
    chomp $line; 
    @input = split(/[ \t]+/,  $line);
    

    for($cur = 0; $cur <= $#input; $cur++){
	if($input[$cur] =~ /[-+]?([0-9]*\.)?[0-9]+/){
	    push @{$transposed[$cur]}, $input[$cur]; 
	}
    }
}

$n = 0; 
foreach $i (@transposed){
    $sum = 0; 
    foreach $j(@{$i}){
	$sum+= $j; 
    }
    print "COLUMN $n : "; 
    print "SUM = $sum, "; 
    $size  = ($#{$i}+1); 
    print "SIZ = $size, ";
    $avg = $sum / $size; 
    print "AVG = $avg, ";
    
    $var = 0; 
    foreach $j(@{$i}){
	$var += ($avg - $j) * ($avg - $j); 
    }
    $var = $var / $size; 
    $std = sqrt($var); 
    
    print "STD = $std" ; 
    print "\n"; 
$n++; 
}


