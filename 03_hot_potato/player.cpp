#include "potato.h"
#include <sys/socket.h>

class Player : public Server {
public:
  int player_id;
  int num_players;

  fd_set readfds;

  Player(char **argv) {
    hostname = argv[1];
    port = argv[2];
  }

  void initPlayer();
  void connectNeighbors();

  void test();
  void play();
  void endGame();
  void playGame();

  ~Player() {
    close(socket_fd);
    close(left_fd);
    close(right_fd);
  }
};

void Player::initPlayer() {
  recv(socket_fd, &player_id, sizeof(player_id), MSG_WAITALL);
  recv(socket_fd, &num_players, sizeof(num_players), MSG_WAITALL);

  int connectPort = getServerPlayerPort();
  // std::cout << "Player Port as Server is: " << connectPort << std::endl;
  send(socket_fd, &connectPort, sizeof(connectPort), 0);
  freeaddrinfo(host_info_list);

  std::cout << "Connected as player " << player_id << " out of " << num_players
            << " total players" << std::endl;
}

void Player::connectNeighbors() {
  IpPortInfo info;
  recv(socket_fd, &info, sizeof(info), MSG_WAITALL);
  // std::cout << "Receive Neigh Info" << std::endl;
  // std::cout << "The neigh IP is: " << info.ip << std::endl;
  // std::cout << "The neigh Port is: " << info.port << std::endl;

  std::string s = std::to_string(info.port);
  const char *port_temp = s.c_str();
  connectServerforPlayer(info.ip, port_temp);
  // std::cout << info.ip << ":" << port_temp << std::endl;
}


void Player::play() {
  // Listening for Potato
  srand((unsigned int)time(NULL) + player_id);
  Potato potato;
  while (true) {
    //Potato potato;
    FD_ZERO(&readfds); 

    int highest_socket_fd = INT_MIN;
    
    FD_SET(socket_fd, &readfds);
    FD_SET(left_fd, &readfds);
    FD_SET(right_fd, &readfds);
    highest_socket_fd = std::max(socket_fd, left_fd);
    highest_socket_fd = std::max(highest_socket_fd, right_fd);
    
    //std::cout << "waiting: " << std::endl;
    int activity = select(highest_socket_fd + 1, &readfds, NULL, NULL, NULL);

    if (FD_ISSET(socket_fd, &readfds)) {
      if(recv(socket_fd, &potato, sizeof(potato), MSG_WAITALL) == 0) return;
      //std::cout << "Recieve Potato" << std::endl;
      // potato.traces.push_back(player_id);
      //std::cout << "The hops is :" << potato.hops << std::endl;
      if (potato.hops == 0) {
        return;
      }
    }
    // Potato from Neighbor
    else if (FD_ISSET(left_fd, &readfds)) {
      if(recv(left_fd, &potato, sizeof(potato), MSG_WAITALL) == 0) return;
      //std::cout << "Recieve Potato from left_fd" << std::endl;
      //std::cout << "The hops is :" << potato.hops << std::endl;
      if (potato.hops == 0) {
        return;
      }
    } else if (FD_ISSET(right_fd, &readfds)) {
      if(recv(right_fd, &potato, sizeof(potato), MSG_WAITALL) == 0) return;
      //std::cout << "Recieve Potato from right_fd" << std::endl;
      //std::cout << "The hops is: " << potato.hops << std::endl;
      if (potato.hops == 0) {
        return;
      }
    }
    potato.traces[potato.len] = player_id;
    potato.len++;
    potato.hops--;

    if (potato.hops == 0) {
      send(socket_fd, &potato, sizeof(potato), 0);
      std::cout << "I'm it" << std::endl;
      return;
    }
    // Send Potato

    int dest = rand() % 2;
    if (dest == 0) {
      send(left_fd, &potato, sizeof(potato), 0);
      std::cout << "Sending potato to "
                << (player_id - 1 + num_players) % num_players << std::endl;
    } else {
      send(right_fd, &potato, sizeof(potato), 0);
      std::cout << "Sending potato to " << (player_id + 1) % num_players
                << std::endl;
    }
  }
}

/*
void Player::endGame() {
  close(socket_fd);
  close(left_fd);
  close(right_fd);
}
*/

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cerr << "The Input format is:player <machine_name> <port_num> "
                 "(example: ./player vcm-xxxx.vm.duke.edu 1234)"
              << "\n";
  }
  // Connect with Ringmaster

  Player *player = new Player(argv);
  player->buildServerforPlayer(); // set left_fd
  player->connectServer(argv[1],
                        argv[2]); // connect with Master -> set socket_fd
  player->initPlayer();           // send connectPort(left_fd) to Master

  player->connectNeighbors();
  player->acceptConnectforPlayer();

  std::cout << "Test Success" << std::endl;

  player->play();
  // player->endGame();
  delete player;
}