#!/usr/bin/perl -w

if ($#ARGV != 1){
    print "Converts DAGs from gene network to gspan input format\n";
    print "usage : ./convert_to_gspan.pl <inputdir> <edgethreshold>\n";
    exit 1; 
}


my $output_filename = $ARGV[0]; 
my $edge_threshold = $ARGV[1];

$output_filename =~ s/\/./\-/g; 
$output_filename =~ s/\/$//g; 
$output_filename .= "-$edge_threshold.gspan"; 

open OUTPUT, ">$output_filename" or die $!; 

my $some_dir = $ARGV[0];
my $graph_id = 0; 
my $max_node_label = -1; 
opendir(my $dh, $some_dir) || die $!;
while(my $file = readdir $dh) {
    if(not $file =~ /^\./){
#for each dag file 
	print "$some_dir/$file\n";
	open INPUT, "$some_dir/$file" or die $!;
	print OUTPUT "t \# $graph_id\n"; 
	$graph_id++; 
	my @nodes; 
	my $tmp = <INPUT>;
	if($max_node_label == -1){
	    $max_node_label = $tmp; 
	    $#nodes = $max_node_label+1; 
	}

	my @edges; 
	my $node_id = 0; 
	# for(my $i = 0; $i <= max_nodes; $i++){
	#     $nodes[$i] = -1; 
	# }
#print edges
	while (my $line = <INPUT>){
	    if($line =~ /^([0-9]+)\s([0-9]+)\s([0-9\.]+)/){
		if($3 >= $edge_threshold){
		    if(not defined $nodes[$1]){
			$nodes[$1] = $node_id++; 
		    }
		    if(not defined $nodes[$2]){
			$nodes[$2] = $node_id++; 
		    }		    
		    push @edges, [$1, $2]; 
		}
	    }
	}
	my @tmp_nodes; 
	for (my $i = 0; $i <= $max_node_label; $i++){
	    if(defined $nodes[$i]){
		$tmp_nodes[$nodes[$i]] = $i; 
	    }
	}

	for (my $i = 0; $i <= $#tmp_nodes; $i++){
	    print OUTPUT "v ".$i." ".$tmp_nodes[$i]."\n";
	}

	foreach $e (@edges){
	    if(defined $e){
		($e1, $e2) = @{$e};
		if($e1 > $e2){
		    $tmp = $e1; 
		    $e1 = $e2; 
		    $e2 = $tmp; 
		}
		print OUTPUT "e ".$nodes[$e1]." ".$nodes[$e2]." ".($e1*($max_node_label+1)+$e2)."\n";
	    }
	}
	close INPUT; 
    }
}
closedir $dh;
close OUTPUT; 
print "OUTPUT $output_filename\n"; 
