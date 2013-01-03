#!/usr/bin/perl -w

#print sorted lines from std
#lines containing any letters are not processed
#if an argument is provided lines containing letters are discarded.
$flag = 1; 
if($#ARGV >= 0){
    $flag=0; 
}
while (<STDIN>){
    if( not ($_ =~ /[a-zA-Z]/)){
	chomp ;
	@nums = split (/[: ]+/, $_); 
	@nums = sort{$b <=> $a}  @nums; 

	foreach $n (@nums){
	    print "$n ";
	}
	print "\n";
    }
    else{
	if($flag){
	    print ;
	}
    }
}

