#!/usr/bin/perl
$ARGC = @ARGV;
if ( $ARGC < 1 ){
  printf ("transnum.pl: output-table-file  [separator] < input-file > output-file\n");
  exit (1);
}
open ( TABLEFILE, ">$ARGV[0]" );
@lines = <STDIN>;
$count = 0;
%numbers = ();

$sep = " ";
if ( $ARGC >= 2 ){ $sep = $ARGV[1]; } 
print $sep;

foreach $trans( @lines ) {
    @eles = split($sep, $trans);
    $all = @eles;
    $c = 0;
    foreach $item( @eles ) {
        $str = "$item.$c";
        $item =~ s/\r//;
        $item =~ s/\n//;
        if (!exists $numbers{$str}) { 
            $numbers{$str} = $count;
            print TABLEFILE "$count $str\n";
            $count++;
        }
        print "$numbers{$str}" ;
        $c++;
        if ($c<$all){ print " ";}

    }
    print "\n"
}
