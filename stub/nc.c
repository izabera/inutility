#include "lib/common.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
  options("lkntuvze:i|m|o:p|s:w|", .argleast = 2, .arglessthan = 3);
  // -t is ignored

  struct sockaddr_in server;
  int sockfd, acceptfd;
  if ((sockfd = socket(AF_INET, flag('u') ? SOCK_DGRAM : SOCK_STREAM, 0)) == -1) return 2;

  memset(&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htons(INADDR_ANY);
  server.sin_port = htons(lastnum('p'));

  if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1) return 3;
  if (listen(sockfd, 10) == -1) return 4;

  while ((acceptfd = accept(sockfd, NULL, NULL)) != -1) {
    switch (fork()) {
      case -1: return 5;
      case  0: dup2(acceptfd, 0); dup2(acceptfd, 1);
               execve(argv[1], argv+1, NULL);
               return 0;
    }
    close(acceptfd);
  }
  return 0;
}

