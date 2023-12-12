class node_participant
{
public:
  virtual ~node_participant() {}
  virtual void deliver(struct packet &msg) = 0;
  //   virtual void make_roomlist() = 0;
  //   virtual struct packet* getPacket() = 0;
  // virtual std::string get_client_id() =0;
};

typedef std::shared_ptr<node_participant> node_participant_ptr;