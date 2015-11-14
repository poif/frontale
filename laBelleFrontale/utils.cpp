#include "utils.h"

int check_prime(int a)
{
   int c;
 
   for ( c = 2 ; c <= a - 1 ; c++ )
   { 
      if ( a%c == 0 )
	 return 0;
   }
   if ( c == a )
      return 1;
}

Huge bigger_prime(Huge n){

  int ret =  0;
  ret = check_prime(n);

 while(ret == 0 ){
  n  = n-1; 
  ret = check_prime(n);
 }
 return n;
}