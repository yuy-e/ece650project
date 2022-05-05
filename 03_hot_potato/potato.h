#include <cstdio>
#include <cstdlib>

#include <climits>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

class IpPortInfo {
public:
  char ip[255];
  int port;
};

class Server {
public:
  int status;
  int socket_fd; // listening master port
  struct addrinfo host_info;
  struct addrinfo *host_info_list;

  const char *hostname;
  const char *port;

  int left_fd;  // server port - listen
  int right_fd; // connect port - transfer

  void buildServer(bool isPlayer);
  void buildServerforPlayer();
  void connectServer(const char *hostname, const char *port);
  void connectServerforPlayer(const char *hostname, const char *port);
  int getServerPlayerPort();
  void acceptConnectforPlayer();

  ~Server() { close(socket_fd); }
};

class Potato {
public:
  int hops;
  int traces[512];
  int len;

  Potato() : hops(0),len(0){};
  void printTraces();
};

void Potato::printTraces() {
  std::cout << "Trace of potato:" << std::endl;
  for (int i = 0; i < len; i++) {
    std::cout << traces[i];
    if (i == len - 1) {
      std::cout << std::endl;
    }
    std::cout << ",";
  }
}

void Server::buildServerforPlayer() {
  hostname = "0.0.0.0";
  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;
  host_info.ai_flags = AI_PASSIVE;
  // assign the address of my local host to the socket structures.

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    std::cerr << "Error: cannot get address info for host 1" << std::endl;
    std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
    exit(EXIT_FAILURE);
  }

  // OS assign Port
  struct sockaddr_in *addr_in = (struct sockaddr_in *)(host_info_list->ai_addr);
  addr_in->sin_port = 0;

  left_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
                   host_info_list->ai_protocol);
  if (left_fd == -1) {
    std::cerr << "Error: cannot create socket" << std::endl;
    std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
    exit(EXIT_FAILURE);
  }

  int yes = 1;
  status = setsockopt(left_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  status = bind(left_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    std::cerr << "Error: cannot bind socket" << std::endl;
    std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
    exit(EXIT_FAILURE);
  }

  status = listen(left_fd, 100);
  if (status == -1) {
    std::cerr << "Error: cannot listen on socket" << std::endl;
    std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
    exit(EXIT_FAILURE);
  }

  // freeaddrinfo(host_info_list);
}

void Server::buildServer(bool isPlayer) {
  hostname = "0.0.0.0";
  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;
  host_info.ai_flags = AI_PASSIVE;
  // assign the address of my local host to the socket structures.

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    std::cerr << "Error: cannot get address info for host 1" << std::endl;
    std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (isPlayer) {
    // OS assign Port
    struct sockaddr_in *addr_in =
        (struct sockaddr_in *)(host_info_list->ai_addr);
    addr_in->sin_port = 0;
  }

  socket_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
                     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    std::cerr << "Error: cannot create socket" << std::endl;
    std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
    exit(EXIT_FAILURE);
  }

  int yes = 1;
  status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    std::cerr << "Error: cannot bind socket" << std::endl;
    std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
    exit(EXIT_FAILURE);
  }

  status = listen(socket_fd, 100);
  if (status == -1) {
    std::cerr << "Error: cannot listen on socket" << std::endl;
    std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
    exit(EXIT_FAILURE);
  }

  freeaddrinfo(host_info_list);
}

void Server::connectServerforPlayer(const char *hostname, const char *port) {
  struct addrinfo host_info;
  struct addrinfo *host_info_list;

  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    std::cerr << "Error: cannot get address info for host 3" << std::endl;
    std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
    exit(EXIT_FAILURE);
  }

  right_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
                    host_info_list->ai_protocol);
  if (right_fd == -1) {
    std::cerr << "Error: cannot create socket" << std::endl;
    std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
    exit(EXIT_FAILURE);
  }

  status =
      connect(right_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    std::cerr << "Error: cannot connect to socket" << std::endl;
    std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
    exit(EXIT_FAILURE);
  }
  // freeaddrinfo(host_info_list);
}

void Server::connectServer(const char *hostname, const char *port) {
  struct addrinfo host_info;
  struct addrinfo *host_info_list;

  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    std::cerr << "Error: cannot get address info for host 3" << std::endl;
    std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
    exit(EXIT_FAILURE);
  }

  socket_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
                     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    std::cerr << "Error: cannot create socket" << std::endl;
    std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
    exit(EXIT_FAILURE);
  }

  status =
      connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    std::cerr << "Error: cannot connect to socket" << std::endl;
    std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
    exit(EXIT_FAILURE);
  }
  // freeaddrinfo(host_info_list);
}

void Server::acceptConnectforPlayer() {
  struct sockaddr_storage socket_addr;
  socklen_t socket_addr_len = sizeof(socket_addr);
  left_fd = accept(left_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
  if (left_fd == -1) {
    std::cerr << "Error: cannot accept connection on socket" << std::endl;
    exit(EXIT_FAILURE);
  }
}

int Server::getServerPlayerPort() {
  // Get Port Number of Player
  struct sockaddr_in addr;
  socklen_t len = sizeof(addr);
  if (getsockname(left_fd, (struct sockaddr *)&addr, &len) == -1) {
    std::cerr << "Error: cannot get socket Port" << std::endl;
    std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
    exit(EXIT_FAILURE);
  }
  int connectPort = ntohs(addr.sin_port);
  std::cout << "connectPort : " << connectPort << std::endl;
  return connectPort;
}
