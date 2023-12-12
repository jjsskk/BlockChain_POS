#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/asio.hpp>
#include <vector>
#include <sstream>
#include <iomanip>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <unordered_map>
#include <random>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <sstream>

#include "block.hpp"
#include "wallet.hpp"
#include "transaction.hpp"
// #include "pos_algo.cpp"

using boost::asio::ip::tcp;
using namespace boost::asio;

#define BUF_SIZE 500
#define NAME_SIZE 20
#define FILE_SIZE 500

#include "struct.hpp"

std::list<struct ip_port_element *> ip_port_table;

int list = 0;

int current_port_accept;
// bool progress_vote = false;
Wallet wallet;
Transactions transactions;
Blockchain blockchain;

#include "node_participant.hpp"
#include "nodes_room.cpp"
std::set<node_participant_ptr> participants_life_; // to manage node_session object life cycle
#include "node_session.hpp"
#include "node_session.cpp"

std::shared_ptr<node_session> chat_ptr = nullptr;
class node_connection
{
public:
  node_connection(boost::asio::io_service &io_service,
                  const tcp::endpoint &endpoint, std::string ip, std::string port_seed, std::string port_node, steady_timer &timer)
      : io_service_(io_service), acceptor_(io_service, endpoint),
        socket_accept_(io_service), socket_connet_(io_service), port_seed_(port_seed), port_node_(port_node), resolver_(io_service), timer_(timer)
  {

    std::shared_ptr<nodes_room> ptr =
        std::make_shared<nodes_room>();
    roomlist_.push_back(ptr);
    endpoint_iterator = resolver_.resolve({ip, port_seed});

    if (endpoint_iterator != boost::asio::ip::tcp::resolver::iterator())
    {
      printf("not null\n");
    }
    do_accept();
    do_connect();
    //  boost::asio::connect(socket_connet_, endpoint_iterator);
  }

private:
  void do_connect()
  {
    boost::asio::async_connect(socket_connet_, endpoint_iterator,
                               [this](boost::system::error_code ec, tcp::resolver::iterator)
                               {
                                 if (!ec)
                                 {
                                   boost::asio::ip::tcp::endpoint remote_endpoint = socket_connet_.remote_endpoint();
                                   std::cout << "Connected connection from: " << remote_endpoint.address().to_string()
                                             << ":" << remote_endpoint.port() << std::endl;
                                   std::shared_ptr<node_session> ptr = std::make_shared<node_session>(io_service_, std::move(socket_connet_), roomlist_, port_seed_, timer_);
                                   //  new node_session(io_service_, std::move(socket_connet_), roomlist_, port_seed_);
                                   ptr->send_port(port_node_);
                                   participants_life_.insert(ptr);
                                   ptr->join_nodes_room();
                                   if (chat_ptr == nullptr)
                                   {
                                     chat_ptr = ptr;
                                     list++;
                                   }
                                   // printf("here\n");
                                 }
                                 else
                                 {
                                   std::cout << "connect error" << std::endl;
                                 }
                               });
  }
  void do_accept()
  {
    acceptor_.async_accept(socket_accept_,
                           [this](boost::system::error_code ec)
                           {
                             if (!ec)
                             {
                               // 클라이언트의 IP 주소와 포트 출력
                               boost::asio::ip::tcp::endpoint remote_endpoint = socket_accept_.remote_endpoint();
                               std::cout << "Accepted connection from: " << remote_endpoint.address().to_string()
                                         << ":" << port_node_ << std::endl;

                               std::shared_ptr<node_session> ptr = std::make_shared<node_session>(io_service_, std::move(socket_accept_), roomlist_, port_node_, timer_);
                               participants_life_.insert(ptr);
                               ptr->join_nodes_room(); // shared_from_this()는 반드시 해당 객체 주소가 shared_ptr로 저장이되고 관리가 된 후에 호출해야함 안그럼 bad weak ptr 에러남
                               if (chat_ptr == nullptr)
                               {
                                 chat_ptr = ptr;
                                 list++;
                               }
                             }
                             else
                             {
                               std::cout << "accept error" << std::endl;
                             }

                             do_accept();
                           });
  }
  boost::asio::io_service &io_service_;
  boost::asio::ip::tcp::resolver resolver_;
  tcp::resolver::iterator endpoint_iterator;
  tcp::acceptor acceptor_;
  tcp::socket socket_accept_;
  tcp::socket socket_connet_;
  std::list<std::shared_ptr<nodes_room>> roomlist_;
  std::string port_seed_; // port to connet seed node
  std::string port_node_; // port to accept peer node
  steady_timer &timer_;
};

//----------------------------------------------------------------------

int main(int argc, char *argv[])
{
  if (argc != 4)
  {
    std::cerr << "Usage: p2p <seed_node_IP> <seed_node_port> <share>\n";
    return 1;
  }
  boost::asio::io_service io_service;
  try
  {
    wallet.saveShare(atoi(argv[3]));
    steady_timer timer{io_service, std::chrono::seconds{15}};
    std::srand(std::time(NULL));
    int port_node_int = atoi(argv[2]) + (rand() % 400 + 1);
    current_port_accept = port_node_int;
    // std::cout << "rand : " << rand() << std::endl;
    tcp::endpoint endpoint(tcp::v4(), port_node_int); // for accept port
    node_connection connection(io_service, endpoint, argv[1], argv[2], std::to_string(port_node_int), timer);

    std::thread t([&io_service]()
                  { io_service.run(); });

    t.detach();
    while (list == 0) // globla variable -> until there is one more connection
    {
    }

    chat_ptr->timer_create_block();
    while (1)
    {
      // printf("display\n");
      printf("----------------------------------------------\n");
      if (chat_ptr->userInput() == 0)
        break;
    }

    io_service.stop();
    // t.join();
    // io_service.run();
  }
  catch (std::exception &e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}