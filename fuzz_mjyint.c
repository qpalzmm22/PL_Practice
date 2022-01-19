#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXLEN 1024

#define test_prog "mjyint"

int main(){
  char cur_path[MAXLEN];
  char target_path[MAXLEN];
  char in_str[MAXLEN];

  fgets(in_str, MAXLEN, stdin);
  in_str[strcspn(in_str, "\n")] = 0x0;

  if(getcwd(cur_path, MAXLEN) == NULL){
    fprintf(stderr, "Erorr in getting current dir");
    exit(1);
  }
  sprintf(target_path, "%s/%s", cur_path, test_prog);
    
  execl(target_path, target_path, in_str, NULL);
  return 0;
}
