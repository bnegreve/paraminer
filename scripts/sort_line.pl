#!/usr/bin/perl -w

#print sorted lines from std
#lines containing any letters are not processed
#if an argument is provided lines containing letters are discarded.
$flag = 1; 
if($#ARGV >= 0){
    $flag=0; 
}
while ($line = <STDIN>){
    if( not ($line =~ /[a-zA-Z]/)){
	chomp $line ;
	if($line =~ /(.*)(\([0-9]+\))/){
	    # if frequency is provided.
	    $line = $1; 
	    $freq = $2;
	}
	
	@nums = split (/[: ]+/, $line); 
	@nums = sort{$b <=> $a}  @nums; 

	foreach $n (@nums){
	    print "$n ";
	}
	print "$freq \n";
    }
    else{
	if($flag){
	    print ;
	}
    }
}

