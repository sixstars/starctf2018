#include <stdio.h>
int main(){
  char s1[] = {42, 98, 118, 101, 127, 114, 99, 54, 87, 125, 58, 84, 38, 110, 122, 105, 109, 17, 18, 19};
  char s2[20];
  int i;
  for(i = 0; i < 20; i++) {
    s2[i] = s1[i] ^ i;
  }
  printf("%s", s2);
}
