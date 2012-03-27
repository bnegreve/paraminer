#!/usr/bin/perl 

if($#ARGV != 1){
    print "usage : ./gspan_remove_1graphs.pl <gspan_out_file> <output_file>\n"; 
    print "Dumps in output_file all the graphs from gspan_out_file with at least one edge.\n";
}

$input_file = $ARGV[0]; 
$output_file = $ARGV[1]; 
$nb_graphs = 0; 
open INPUT, "$input_file" or die $!; 
open OUTPUT, ">$output_file" or die $!; 

$freq = 0; 
$has_edges = 0; 
@current_graph = ();
@nodes; 
while($line = <INPUT>){

    if($line =~ /^v\s([0-9]+)\s([0-9]+)/){
	$nodes[$1] = $2; 
    }
    elsif($line =~ /^e\s([0-9]+)\s([0-9]+)/){
	$has_edges = 1; 
	push @current_graph, "( $nodes[$1], $nodes[$2] ) ";
    }
    elsif($line =~ /^t\s#\s[0-9]+\s\*\s([0-9]+)/){
	$freq = $1; 
    }
    elsif ($line =~ /^$/){
	if($has_edges){
	    $nb_graphs++; 
	    foreach $l (@current_graph){
		print OUTPUT $l; 
	    }
	    print OUTPUT "($freq)\n";
	}
	$has_edges = 0; 
	@current_graph = (); 
	@nodes = (); 
    }

}
print "$nb_graphs graphs with at least one edge.\n";
