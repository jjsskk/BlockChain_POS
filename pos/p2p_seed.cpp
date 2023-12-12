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
                  const tcp::endpoint &endpoint, std::string port, steady_timer &timer)
      : io_service_(io_service), acceptor_(io_service, endpoint),
        socket_(io_service), port_(port), timer_(timer)
  {
    std::shared_ptr<nodes_room> ptr =
        std::make_shared<nodes_room>();
    roomlist_.push_back(ptr);
    // do_connect();
    do_accept();
  }

private:
  // void do_connect(tcp::resolver::iterator endpoint_iterator)
  // {
  //   boost::asio::async_connect(socket_, endpoint_iterator,
  //                              [this](boost::system::error_code ec, tcp::resolver::iterator)
  //                              {
  //                                if (!ec)
  //                                {
  //                                    std::shared_ptr<node_session> ptr = std::make_shared<node_session>(io_service_, std::move(socket_), roomlist_, port_);
  //                                   //  ptr->start();
  //                                    participants_life_.insert(ptr);
  //                                }
  //                              });
  // }
  void do_accept()
  {
    acceptor_.async_accept(socket_,
                           [this](boost::system::error_code ec)
                           {
                             if (!ec)
                             {

                               boost::asio::ip::tcp::endpoint remote_endpoint = socket_.remote_endpoint();
                               std::cout << "Accepted connection from: " << remote_endpoint.address().to_string()
                                         << ":" << remote_endpoint.port() << std::endl;
                               std::shared_ptr<node_session> ptr = std::make_shared<node_session>(io_service_, std::move(socket_), roomlist_, port_, timer_);
                               participants_life_.insert(ptr);
                               ptr->join_nodes_room();
                               if (chat_ptr == nullptr)
                               {
                                 chat_ptr = ptr;
                                 list++;
                               }
                               // printf("here");
                             }
                             else
                             {
                               std::cout << "accept error" << std::endl;
                             }
                             do_accept();
                           });
  }
  boost::asio::io_service &io_service_;
  tcp::acceptor acceptor_;
  tcp::socket socket_;
  std::list<std::shared_ptr<nodes_room>> roomlist_;
  std::string port_; // port to accept peer node
  steady_timer &timer_;
};

int main(int argc, char *argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: p2p_seed <port> <share>\n";
      return 1;
    }
    wallet.saveShare(atoi(argv[2]));
    boost::asio::io_service io_service;
    current_port_accept = atoi(argv[1]);

    steady_timer timer{io_service, std::chrono::seconds{15}};
    tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[1]));
    node_connection connection(io_service, endpoint, argv[1], timer);

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