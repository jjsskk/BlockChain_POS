class nodes_room
{
public:
  nodes_room()
  {
    // std::cout  << " serviced" << std::endl;
  }
  ~nodes_room()
  {
    // std::cout << " terminated" << std::endl;
  }

public:
  void leave(node_participant_ptr participant)
  {
    participants_.erase(participant);
  }

  void join_user(std::string client_id, std::shared_ptr<node_participant> participant)
  {
    // clientid_list_.push_back(client_id);
    participants_.insert(participant); // new client
  }

  void deliver(struct packet &msg)
  {

    int i = 0;
    for (auto participant : participants_) // server send file read check msg to all member in room except himself
    {
      participant->deliver(msg);
      i++;
    }
    // printf("particpants number : %d\n",i);
  }

private: // 전역 변수들
  std::set<node_participant_ptr> participants_;
};
