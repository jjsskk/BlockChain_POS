void node_session::join_nodes_room()
{
  //  std::shared_ptr<node_participant> ptr = shared_from_this();
  //  if (shared_from_this()) {
  //           std::cout << "Successfully obtained shared_ptr" << std::endl;
  //       } else {
  //           std::cout << "Failed to obtain shared_ptr" << std::endl;
  //       }
  roomlist_.back()->join_user("test", shared_from_this()); // save client_id and socket in room
  wallet.increaseNode();
}

void node_session::timer_create_block()
{
  timer_ = steady_timer{io_service_, std::chrono::seconds{15}}; // 재사용이 안되서 재귀 호출 할때 마다 생성해야 함

  timer_.async_wait([this](const boost::system::error_code &ec)
                    { 
                                if(!ec)
                                {
                                  if(transactions.getTransactions()->size() == 0)
                                  {
                                    wallet.resetShares();
                                    timer_create_block();
                                    return;
                                  }
                                  struct packet pkt_write;
                                  //  progress_vote = true;
                                  // stop = 0; 
                                  std::cout<<"Timer end\nStart vote!!"<<std::endl;
                                  pkt_write.type =3;
                                  pkt_write.share = wallet.getShare();
                                  roomlist_.back()->deliver(pkt_write);
                                  timer_create_block();
                                }else
                                {
                                    std::cout << "timer error: " << ec.message() << std::endl;
                                } });
  //  std::thread thread1{[this]()
  //                      { io_service_.run(); }};
  //  thread1.detach();
}

void node_session::connect_peerNode(struct ip_port_element *element)
{
  // printf("connect_peerNode\n");
  auto remote_endpoint = socket_.remote_endpoint();
  // printf("connect_peerNode() %s %d\n", element->ip,element->port);
  if (strcmp(element->ip, remote_endpoint.address().to_string().c_str()) == 0 && element->port == current_port_accept)
    return;
  auto endpoint_iterator = resolver_.resolve({element->ip, std::to_string(element->port)});

  if (endpoint_iterator != boost::asio::ip::tcp::resolver::iterator())
  {
    // printf("connect peer node not null\n");
  }
  else
  {
    printf("connect peer node null\n");
  }
  std::string str(element->ip);
  do_connect(str, endpoint_iterator);
}
void node_session::receive_Ips_from_seed()
{

  struct ip_port_element *element = (struct ip_port_element *)malloc(sizeof(struct ip_port_element));
  strcpy(element->ip, pkt_->element.ip);
  element->port = pkt_->element.port;
  ip_port_table.push_back(element);
  // printf("receive :%s %d\n",element->ip,element->port);

  connect_peerNode(element);
}
void node_session::send_Ips()
{
  for (const auto &element : ip_port_table)
  {
    pkt_->type = 1;
    pkt_->element.port = element->port;
    strcpy(pkt_->element.ip, element->ip);
    socket_.write_some(boost::asio::buffer(pkt_, sizeof(struct packet)));
  }
}

void node_session::send_port(std::string port)
{
  pkt_->type = 0;
  pkt_->port = atoi(port.c_str());
  socket_.write_some(boost::asio::buffer(pkt_, sizeof(struct packet)));
}
void node_session::receive_port()
{
  // 클라이언트의 IP 주소와 포트 출력
  boost::asio::ip::tcp::endpoint remote_endpoint = socket_.remote_endpoint();
  // std::cout << "Accepted connection from: " << remote_endpoint.address().to_string()
  //   << ":" << remote_endpoint.port() << std::endl;
  struct ip_port_element *element = (struct ip_port_element *)malloc(sizeof(struct ip_port_element));
  strcpy(element->ip, remote_endpoint.address().to_string().c_str());
  element->port = pkt_->port; // peer node의 accept port
  ip_port_table.push_back(element);
}

int node_session::userInput() // main thread execute this method
{
  std::cout << "welcome to block chain world " << std::endl;
  char line[50];
  int quit = 0;
  // int duplicated = 0;
  struct packet pkt_write;
  memset(line, 0, strlen(line));
  std::cout << "Input <q or Q> if you wanna terminate program " << std::endl;
  std::cout << "Input <0> if you wanna create transaction " << std::endl;
  std::cout << "Input <1> if you wanna try data modification in block (hacker mode)" << std::endl;
  while (std::cin.getline(line, 50)) // remain places filled with null
  {

    if (!strcmp(line, "q") || !strcmp(line, "Q"))
    {
      // close();
      quit = 1;
      break;
    }
    else if (!strcmp(line, "0"))
    {
      std::cout << "Input transaction content you want" << std::endl;
      memset(line, 0, strlen(line));
      std::cin.getline(line, 50);
      std::string str1(line);
      Transaction transaction("Sender1", "Recipient1", 10);
      transaction.setData(str1);
      transactions.push_back(transaction);
      std::string serializedData = transaction.serializeToString();
      // std::cout << serializedData.c_str() << std::endl;
      std::cout << "you make Transaction : " << transaction.getData() << std::endl;
      memset(&pkt_write, 0, sizeof(struct packet));
      strcpy(pkt_write.msg, serializedData.c_str());
      pkt_write.type = 2; // send transaction data
      roomlist_.back()->deliver(pkt_write);

      // memcpy(pkt_write.client_id, client_id_.c_str(), client_id_.length());
      // goto point;
    }
    else if (!strcmp(line, "1"))
    {
      if (blockchain.getChain().size() < 2)
      {

        std::cout << "This function is available only when there is more than one block\n"
                  << std::endl;
        std::cout << "------------------------------------------------------------------" << std::endl;
        std::cout << "Input <q or Q> if you wanna terminate program " << std::endl;
        std::cout << "Input <0> if you wanna create  transaction " << std::endl;
        std::cout << "Input <1> if you wanna test hacker mode" << std::endl;
        continue;
      }
      memset(line, 0, strlen(line));
      std::cout << "Input data you want to modify " << std::endl;
      std::cin.getline(line, 50);
      std::string str1(line);
      std::cout << blockchain.getLatestBlock().getData() << " -> " << str1 << std::endl;
      blockchain.getLatestBlock().modifyData(str1);
    }
    else
    {

      std::cout << "Please input according to the guidelines. " << std::endl;
    }

    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "Input <q or Q> if you wanna terminate program " << std::endl;
    std::cout << "Input <0> if you wanna create  transaction " << std::endl;
    std::cout << "Input <1> if you wanna test hacker mode" << std::endl;
  }

  pkt_write.type = 5;
  roomlist_.back()->deliver(pkt_write);
  // write(pkt_write);

  return 0;
}

void node_session::deliver(struct packet &msg) // 연결된 안 모든 peer에게 msg 전송
{

  bool write_in_progress = !write_msgs_.empty();
  write_msgs_.push_back(msg); // 전송할 msg(read받은 msg) 저장
  if (!write_in_progress)     // write_in progress =true(do_write()진행중) = write_msgs가 비어있지 않음
  {
    // printf("hell2\n");
    do_write();
  }
}

void node_session::read_transaction()
{
  std::string str1(pkt_->msg);
  Transaction transaction;
  transaction.deserializeFromString(str1);
  // std::cout << str1 << std::endl;
  std::cout << "Received Latest Transaction: " << transaction.getData() << std::endl;
  transactions.push_back(transaction);
}
void node_session::read_share()
{
  wallet.storeOtherNodeShares(pkt_->share);
}

void node_session::selectValidator()
{
  if (wallet.selectValidator())
  {
    std::cout << std::endl;
    std::cout << "You(" << wallet.getShare() << ")are selected as validator" << std::endl;
    std::cout << std::endl;
    std::cout << "making block..." << std::endl;
    std::cout << std::endl;
    makeBlock();
  }
}
void node_session::makeBlock()
{
  std::string str = "";
  str += "Transactions\n";
  for (auto it : *(transactions.getTransactions()))
  {
    str += it.getData();
    str += "\n";
  }
  // std::cout<<str<<std::endl;
  Block block(blockchain.getLatestBlock().getHash(), str);
  blockchain.addBlock(block);
  transactions.resetTransactions();
  block.printBlockData();
  wallet.resetShares();
  wallet.increaseShare();
  if (!blockchain.isChainValid())
  {

    std::cout << "detected block modification!" << std::endl;
    std::cout << "are you hacker?" << std::endl;
    std::cout << "please shutdown this program!" << std::endl;
    blockchain.deleteBlock();
    struct packet pkt_write;
    pkt_write.type = 5;
    roomlist_.back()->deliver(pkt_write);
    return;
  }
  deliver_block(block);
}
void node_session::deliver_block(Block &block)
{
  struct packet pkt_write;
  std::string serializedData = block.serializeToString();
  memset(&pkt_write, 0, sizeof(struct packet));
  strcpy(pkt_write.msg, serializedData.c_str());
  pkt_write.type = 4; // send transaction data
  roomlist_.back()->deliver(pkt_write);

  // std::cout << serializedData.c_str() << std::endl;
  // timer_create_block(timer_);
}
void node_session::read_block()
{
  std::string str(pkt_->msg);
  Block block;
  block.deserializeFromString(str);
  block.printBlockData();
  blockchain.addBlock(block);
  wallet.resetShares();
  transactions.resetTransactions();
  if (!blockchain.isChainValid())
  {

    std::cout << "detected block modification!" << std::endl;
    std::cout << "are you hacker?" << std::endl;
    std::cout << "please shutdown this program!" << std::endl;
    blockchain.deleteBlock();
    struct packet pkt_write;
    pkt_write.type = 5;
    roomlist_.back()->deliver(pkt_write);
    return;
  }
  // timer_create_block(timer_);
}
