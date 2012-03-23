#!/usr/bin/perl -w

#print sorted lines from std
#lines containing any letters are not processed
#if an argument is provided lines containing letters are discarded.
$flag = 1; 
if($#ARGV >= 0){
    $flag=0; 
}



sub grad_item_cmp{
    if( $cmp = (@$a[0] <=> @$b[0])){
	return $cmp; 
    }
    else{
	return @$a[1] ge @$b[1]; 
    }
}

my @lines; 
while ($line = <STDIN>){    
    my @items; 
    my $freq; 
    while($line =~ /([0-9]+)([\+\-]) /g){
	push @items, [$1, $2]; 
    }

    if($line =~ /(\([0-9]+\))/){
	$freq = $1; 
    }
#skip patterns of size 1 that are not outputed by glcm
    if($#items > 0){
	my $str; 
	foreach $n (sort grad_item_cmp @items){
	    $str .="@$n[0]@$n[1] ";
	}
	$str .="$freq\n"; 
	push @lines, $str; 
    }
}

foreach $l (sort @lines){
    print $l; 
}

