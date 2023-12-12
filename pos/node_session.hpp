class node_session
    : public std::enable_shared_from_this<node_session>,
      public node_participant
{
public:
  node_session(boost::asio::io_service &io_service, tcp::socket socket, std::list<std::shared_ptr<nodes_room>> &roomlist, std::string port, steady_timer &timer)
      : io_service_(io_service), socket_(std::move(socket)), socket_connect_(io_service), port_(port),
        roomlist_(roomlist), resolver_(io_service), timer_(timer)
  {
    // start();
    printf("new session created\n");
    // boost::asio::ip::tcp::endpoint remote_endpoint = socket_.remote_endpoint();
    // std::cout << "Accepted connection from: " << remote_endpoint.address().to_string()
    //  << ":" << remote_endpoint.port() << std::endl;
    // auto remote_endpoint = socket_.remote_endpoint();
    do_read();
  }
  ~node_session()
  {
    free(pkt_);
    socket_.close();
    std::cout << "node_session terminated" << std::endl;
  }
  void join_nodes_room();
  void timer_create_block();
  void connect_peerNode(struct ip_port_element *element);
  void receive_Ips_from_seed();
  void send_Ips();
  void send_port(std::string port);
  void receive_port();
  int userInput();
  void deliver(struct packet &msg);
  void read_transaction();
  void read_share();
  void selectValidator();
  void makeBlock();
  void deliver_block(Block &block);
  void read_block();

private:
  void do_connect(std::string &port, tcp::resolver::iterator endpoint_iterator)
  {
    boost::asio::async_connect(socket_connect_, endpoint_iterator,
                               [this, port, endpoint_iterator](boost::system::error_code ec, tcp::resolver::iterator)
                               {
                                 if (!ec)
                                 {
                                   // 클라이언트의 IP 주소와 포트 출력
                                   boost::asio::ip::tcp::endpoint remote_endpoint = socket_connect_.remote_endpoint();
                                   std::cout << "after seed, Connected connection from: " << remote_endpoint.address().to_string()
                                             << ":" << remote_endpoint.port() << std::endl;
                                   std::shared_ptr<node_session> ptr = std::make_shared<node_session>(io_service_, std::move(socket_connect_), roomlist_, port, timer_);
                                   //  new node_session(io_service_, std::move(socket_connet_), roomlist_, port_seed_);
                                   participants_life_.insert(ptr);
                                   ptr->join_nodes_room(); // shared_from_this()는 반드시 해당 객체 주소가 shared_ptr로 저장이되고 관리가 된 후에 호출해야함 안그럼 bad weak ptr 에러남

                                   // ptr->send_port(port_node_);
                                   // printf("here\n");
                                 }
                                 else
                                 {
                                   std::cout << "Connect error: " << ec.message() << std::endl;
                                   //  if (error == boost::asio::error::operation_aborted)
                                   //    do_connect(port,endpoint_iterator);
                                 }
                               });
  }
  void do_write()
  {
    // auto self(shared_from_this());
    boost::asio::async_write(socket_,
                             boost::asio::buffer(&(write_msgs_.front()), sizeof(struct packet)),
                             [this](boost::system::error_code ec, std::size_t /*length*/)
                             {
                               if (!ec)
                               {
                                 //  std::cout<<write_msgs_.front().msg<<std::endl;
                                 write_msgs_.pop_front();
                                 if (!write_msgs_.empty())
                                 {
                                   do_write();
                                 }
                               }
                               else
                               {
                                 //  std::cerr << "Exception: " << ec.what() << "\n";
                               }
                             });
  }

  void do_read()
  {
    // auto self(shared_from_this());// weak_pkt 원인 코드

    boost::asio::async_read(socket_,
                            boost::asio::buffer(pkt_, sizeof(struct packet)),
                            [this](boost::system::error_code ec, std::size_t /*length*/)
                            {
                              if (!ec)
                              {
                                if (pkt_->type == 0) // seed node read accept port from connected node
                                {
                                  receive_port();
                                  send_Ips();
                                  do_read();
                                }
                                else if (pkt_->type == 1) // read ips from seed node
                                {
                                  // std::cout << " read selectd room name :" << pkt_->selected_roomname << std::endl;
                                  receive_Ips_from_seed();
                                  do_read();
                                }
                                else if (pkt_->type == 2) // read transaction
                                {
                                  // std::cout << "here :" << pkt_->msg << std::endl;
                                  read_transaction();
                                  do_read();
                                }
                                else if (pkt_->type == 3) // read share
                                {
                                  // progress_vote = true;
                                  // std::cout << "here :" << pkt_->msg << std::endl;
                                  read_share();
                                  if (wallet.checkIfreceiveAllShare())
                                    selectValidator();
                                  do_read();
                                }
                                else if (pkt_->type == 4) // read block
                                {
                                  // progress_vote = true;
                                  // std::cout << "here :" << pkt_->msg << std::endl;
                                  read_block();
                                  do_read();
                                }
                                else if (pkt_->type == 5) // delete terminated node info
                                {
                                  //  std::cerr << "Exception: " << ec.what() << "\n";
                                  if (roomlist_.back() != nullptr) // for client  unexpected close
                                  {
                                    roomlist_.back()->leave(shared_from_this());
                                  }
                                  wallet.decreaseNode();
                                  wallet.resetShares();
                                  participants_life_.erase(shared_from_this());
                                }
                              }
                              else
                              {
                                // std::cerr << "Exception: " << ec.what() << "\n";
                                if (roomlist_.back() != nullptr) // for client  unexpected close
                                {
                                  roomlist_.back()->leave(shared_from_this());
                                }
                                wallet.decreaseNode();
                                wallet.resetShares();

                                participants_life_.erase(shared_from_this());
                              }
                            });
  }

  boost::asio::io_service &io_service_;
  boost::asio::ip::tcp::resolver resolver_;
  std::deque<struct packet> write_msgs_; // store msgs to bo going to send to corresponing client in time-order
  struct packet *pkt_ = (struct packet *)malloc(sizeof(struct packet));
  tcp::socket socket_;
  tcp::socket socket_connect_;
  // std::shared_ptr<nodes_room> current_room_; // room that user enters in currently
  std::list<std::shared_ptr<nodes_room>> &roomlist_;
  std::string port_;
  steady_timer &timer_;
};