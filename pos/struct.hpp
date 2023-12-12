struct ip_port_element
{
  char ip[20];
  int port;
};

struct packet
{
  struct ip_port_element element;
  short type;
  int port; // FOR accept
  char msg[BUF_SIZE];
  int share;
};