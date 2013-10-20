<?php                                                                           
$filename = "hits.txt";                                                         
$fd = fopen ($filename , "a");                                                  
$ip = getenv("REMOTE_ADDR");                                                    
$host = gethostbyaddr($ip);                                                     
$fcounted = "\n$ip:$host:index.php" ;                                           
$fout= fwrite ($fd , $fcounted );                                               
fclose($fd);                                                                    
?>                                                                              
