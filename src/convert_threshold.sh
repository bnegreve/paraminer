#!/usr/bin/perl

open INPUT, $ARGV[0] or die $!;

$thres = $ARGV[1]; 
if($thres =~ /([0-9])+(\.[0-9]+)?/){

}else{
die "not a valid threshold\n";
}


$nb_lines = 0; 
while(<INPUT>){
    if($line =~ /^\n/){}
else{
	    $nb_lines++; 
	}
}


print "NBLINES : $nb_lines\n"; 
if($thres < 1){
	print "To absolute [$nb_lines * $thres] : ".($nb_lines * $thres)."\n"; 
}

if($thres > 1){
	print "To relative [$thres / $nb_lines] : ".($thres / $nb_lines )."\n"; 
}

if($thres == 1){
print ("Ambigus !\n"); 
}