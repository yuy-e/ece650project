#include "potato.h"
#include <cstdlib>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <vector>


class RingMaster : public Server {
public:
  int num_players;
  int num_hops;

  std::vector<int> client_connection_fds;

  std::vector<std::string> ip_list;
  std::vector<int> listen_port_list;
  std::vector<IpPortInfo> ip_char_list;

  fd_set readfds;
  //Potato potato;

  RingMaster(char **argv) {
    port = argv[1];
    num_players = atoi(argv[2]);
    num_hops = atoi(argv[3]);
    hostname = "0.0.0.0"; // INADDR_ANY

  }

  int connectOnePlayer();
  void connectPlayers();
  void constructInfo();
  void sendNeighborInfo();

  void transferPotato();
  void receivePotato();
  void test();

  ~RingMaster(){
    for(int i=0; i<num_players; ++i)
      close(client_connection_fds[i]);
  }
};

/* Accept one Player */
int RingMaster::connectOnePlayer() {
  // std::cout << "Waiting for connection on port " << port << std::endl;
  struct sockaddr_storage socket_addr;
  socklen_t socket_addr_len = sizeof(socket_addr);
  int client_connection_fd;
  client_connection_fd = accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);

  struct sockaddr_in * temp = (struct sockaddr_in *)&socket_addr;
  std::string ip = inet_ntoa(temp->sin_addr);
  ip_list.push_back(ip);

  // Can infer the IP address of Clients: Players
  // socket_addr->sin_addr : Internet Address
  // socket_addr->sin_port : Port Number
  if (client_connection_fd == -1) {
    std::cerr << "Error: cannot accept connection on socket" << std::endl;
    return -1;
  }
  return client_connection_fd;
}

void RingMaster::connectPlayers() {
  for (int i = 0; i < num_players; i++) {
    int client_connection_fd = connectOnePlayer();
    client_connection_fds.push_back(client_connection_fd);

    // Send information to Players
    send(client_connection_fd, &i, sizeof(i), 0);
    send(client_connection_fd, &num_players, sizeof(num_players), 0);

    // Player send back information

    // Receive Port Information from Players
    int listen_port;
    ssize_t temp = recv(client_connection_fd, &listen_port,
         sizeof(listen_port), MSG_WAITALL);
    
    if (temp == -1) {
      std::cerr << "Error: cannot receive port" << std::endl;
      exit(EXIT_FAILURE);
    }
    //std::cout << "port is " << listen_port << std::endl;

    listen_port_list.push_back(listen_port);
    std::cout << "Player " << i << " is ready to play" << std::endl;

  }
}

void RingMaster::constructInfo(){
  for(int i=0; i<client_connection_fds.size(); ++i){
    IpPortInfo info;
    strcpy(info.ip, ip_list[i].c_str());
    info.port = listen_port_list[i];
    ip_char_list.push_back(info);
  }
}

void RingMaster::sendNeighborInfo(){
  int i;
  for(i=0; i<client_connection_fds.size()-1; ++i){
    send(client_connection_fds[i], &ip_char_list[i+1], sizeof(ip_char_list[i+1]), 0);
  }
  if(i == client_connection_fds.size()-1){
    send(client_connection_fds[i], &ip_char_list[0], sizeof(ip_char_list[0]), 0);
  }
}

void RingMaster::transferPotato() {
  // initialize potato
  Potato potato;
  potato.hops = num_hops;

  // send to random player
  srand((unsigned int)time(NULL));
  int random_player = rand() % num_players;

  // buf: buf is a pointer to the data you want to send
  send(client_connection_fds[random_player], &potato, sizeof(potato), 0);
  std::cout << "Ready to start the game, sending potato to player " << random_player
       << std::endl;
}

void RingMaster::receivePotato() {
  Potato potato;
  FD_ZERO(&readfds); 

  int highest_socket_fd = INT_MIN;
  for(int i=0; i<client_connection_fds.size(); ++i){
    FD_SET(client_connection_fds[i], &readfds);
    highest_socket_fd = std::max(highest_socket_fd, client_connection_fds[i]);
  }
  
  int activity = select(highest_socket_fd+1, &readfds, NULL, NULL, NULL);

  /*
  if (activity != 1) {
    std::cout << "Activity Error" << std::endl;
    exit(EXIT_FAILURE);
  }
  */

  for (int i = 0; i < num_players; ++i) {
    int sd = client_connection_fds[i];
    if (FD_ISSET(sd, &readfds)) {
      // end of game
      
      recv(sd, &potato, sizeof(potato), MSG_WAITALL);
      //std::cout << "End of Game" << std::endl;
      //close(sd);
      break;
    }
  }
  potato.printTraces();
}

int main(int argc, char **argv) {
  if (argc != 4) {
    std::cerr << "The Input format is:ringmaster <port_num> <num_players> "
            "<num_hops> (example: ./ringmaster 1234 3 100)"
         << "\n";
  }
  RingMaster *ringmaster = new RingMaster(argv);

  std::cout << "Potato Ringmaster"
            << "\n";
  std::cout << "Players = " + std::to_string(ringmaster->num_players) << "\n";
  std::cout << "Hops = " + std::to_string(ringmaster->num_hops) << "\n";

  ringmaster->buildServer(false);

  ringmaster->connectPlayers();
  ringmaster->constructInfo();
  ringmaster->sendNeighborInfo();

  //ringmaster->test();
  
  ringmaster->transferPotato();
  ringmaster->receivePotato();

  //ringmaster->potato.printTraces();

  delete ringmaster;
  return EXIT_SUCCESS;
}

