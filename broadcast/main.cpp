#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

const char *WSA_get_error(int error)
{
#define CASE_RET_STR(c) case c: return #c;
  switch (error) {
    CASE_RET_STR(WSASYSNOTREADY)
    CASE_RET_STR(WSAVERNOTSUPPORTED)
    CASE_RET_STR(WSAEINPROGRESS)
    CASE_RET_STR(WSAEPROCLIM)
    CASE_RET_STR(WSAEFAULT)
  }
  return "Unknown error";
}

SOCKET sock;

int main()
{
  WSADATA wsadata;
  int error = WSAStartup(MAKEWORD(2, 2), &wsadata);
  if (error != 0) {
    printf("WSAStartup failed (%s)\n", WSA_get_error(error));
    return 1;
  }

  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock == SOCKET_ERROR) {
    printf("failed to create socket\n");
    return 2;
  }

  sockaddr_in bind_addr;
  bind_addr.sin_family = AF_INET;
  bind_addr.sin_port = htons(27001);
  bind_addr.sin_addr.s_addr = INADDR_ANY;
  if (bind(sock, (const sockaddr *)&bind_addr, sizeof(bind_addr)) == SOCKET_ERROR) {
    printf("binding socket failed\n");
    return 3;
  }

  int broadcastEnable = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const char *)&broadcastEnable, sizeof(broadcastEnable)) < 0) {
    perror("setsockopt failed");
    return 1;
  }

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(8081);
  addr.sin_addr.s_addr = inet_addr("255.255.255.255"); //broadcast addr 255.255.255.255

  while (1) {
    const char *p_str = "Hello!";
    int len = sendto(sock, p_str, strlen(p_str), 0, (const sockaddr *)&addr, sizeof(addr));
    if (len <= 0) {
      printf("Sending failed. Returned %d\n", len);
    }
    printf("Sending %d bytes OK\n", len);
    Sleep(1000);
  }
  WSACleanup();
  return 0;
}