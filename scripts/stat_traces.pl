#!/usr/bin/perl

# computes event delays in paraminer trace files (diff between START
# EVENT XXX end END EVENT XXX). Outputs are column formated and can be
# directly fed into stat.pl.

# USAGE: for an input file input.trace crated with the following command
# $> cat > input.trace
# 0, 0, START global
# 0, 0, START chicken
# 12, 0, END chicken
# 0, 0, START gluc
# 13, 0, END gluc
# 14, 0, START gluc
# 18, 0, END gluc
# 25, 0, END global
# 40, 0, START chicken
# 45, 0, END chicken

# The command:
# $> ./stat_traces.pl input.trace | ./stats.pl
# will output:
#
# gluc global chicken 
# COLUMN 0 : SUM = 17, SIZ = 2, AVG = 8.5, STD = 4.5
# COLUMN 1 : SUM = 25, SIZ = 1, AVG = 25, STD = 0
# COLUMN 2 : SUM = 17, SIZ = 2, AVG = 8.5, STD = 3.5
#
# Note that one or more input trace file are accepted, therefore: 
# $> ./stat_traces.pl trace_*.trace | ./stats.pl
# can be used. 
#
# see stats.pl for more details about it. 

%output; 

foreach $file (@ARGV){
    print STDERR "Reading trace file: $file.\n";
    open INPUT, "<$file" or die $!;
    while($line = <INPUT>){
	if($line =~ /^\s*#/){next;}
	chomp $line; 

	@input = split(/[\t ]*,[\t ]*/,  $line);

	if($#input != 2){
	    die "input format is <timestamp in s>, <thread id>, <event name>\n"; 
	}
	
	$time = $input[0]; 
	$thread_id = $input[1]; 
	$event_name = $input[2]; 

	if($event_name =~ /START[ \t]*(.*)/){
	    if(defined $events{$1} and $events{$1} != -1){
		print STDERR "Warning START END sequence not respected\n"; 
	    }
	    $events{$1} = $time; 
	}
	elsif($event_name =~ /END[ \t]*(.*)/){	
	    $delay = $time - $events{$1}; 
	    $events{$1} = -1; 
	    push @{$output{$1}}, $delay; 
	}
    }
    close INPUT; 
}

$max_event = 0; 

#print field names
print "#";
foreach $k (keys %output){
    print "$k "; 
    if( $#{$output{$k}}+1 > $max_event){
	$max_event = $#{$output{$k}} + 1; 
    }
}
print "\n";

#print field values
for($i = 0; $i < $max_event; $i++){
    foreach $k (keys %output){
	if(defined  $output{$k}[$i]){
	    print $output{$k}[$i]."\t"; 
	}
	else {
	    print "ukn\t";
	}
	    
    }
    print "\n"
}

