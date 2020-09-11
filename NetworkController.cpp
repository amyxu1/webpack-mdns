#include <absl/strings/str_split.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#include "NetworkController.hpp"

NetworkController::NetworkController(absl::flat_hash_set<std::string> services) 
{
  m_services = services;
  m_urlTable = new absl::flat_hash_map<std::string, absl::flat_hash_set<std::string>>();
  m_port = MDNS_PORT;

  setup_ipv4();
}

void NetworkController::setup_ipv4()
{ 
  memset(m_addr, 0, sizeof(struct sockaddr_in));
  m_addr->sin_family = AF_INET;
  m_addr->sin_addr.s_addr = INADDR_ANY;
  m_addr->sin_port = htons(MDNS_PORT);

  m_socket = mdns_socket_open_ipv4(m_addr);
  mdns_socket_setup_ipv4(m_socket, m_addr);
}

void NetworkController::listen(mdns_record_callback_fn callback)
{
  size_t capacity = 2048;
  void* buffer = malloc(capacity);
  service_record_t serv_rec;
  serv_rec.service = "mdns";
  serv_rec.hostname = m_hostname.c_str();
  serv_rec.address_ipv4 = m_addr->sin_addr.s_addr;
  serv_rec.address_ipv6 = 0;
  serv_rec.port = m_port;

  int listen_fd = epoll_create1(0);
  int running = 1;

  struct epoll_event event;
  event.events = EPOLLIN;
  event.data.fd = listen_fd;

  if (epoll_ctl(listen_fd, EPOLL_CTL_ADD, 0, &event))
  {
    fprintf(stderr, "Failed to add listen fd to epoll\n");
    close(listen_fd);
    return;
  }

  while (running)
  {
                // Or maybe I need an array of events (events[i])
    printf("\nPolling for input...\n", event.data.fd);
    running = epoll_wait(listen_fd, &event, 1, 30000);
    if (running > 0)
    {
      mdns_socket_listen(m_socket, buffer, capacity, callback, &serv_rec);
    }
  }
}

void NetworkController::query(std::string service, mdns_record_callback_fn callback)
{
  size_t capacity = 2048;
  void* buffer = malloc(capacity);
  void* user_data = 0;

  printf("Sending mDNS query: %s\n", service);

  // queryid of 0 signifies a multicast query
  int query_id = mdns_query_send(m_socket, MDNS_RECORDTYPE_PTR, service.c_str(), 
           service.length(), buffer, capacity, 0 /*queryid*/);

  if (query_id)
  {
    printf("Failed to send mDNS query: %s\n", strerror(errno));
  }

  printf("Reading mDNS query responses");
  int status = mdns_query_recv(m_socket, buffer, capacity, callback, user_data, query_id);
}

int NetworkController::query_callback(int sock, const struct sockaddr* from, 
  size_t addr_len, mdns_entry_type_t entry,
  uint16_t query_id, uint16_t rtype, uint16_t rclass, 
  uint32_t ttl, const void* data, size_t size, 
  size_t name_offset, size_t name_length, 
  size_t record_offset, size_t record_length, 
  void* user_data)
{
  static char addrbuffer[64];
  static char entrybuffer[256];
  static char namebuffer[256];

  // grab sender's address
  std::string from_addr_str = ip_address_to_string(addrbuffer, sizeof(addrbuffer), (const struct sockaddr_in*)from, addr_len);

  // get entry information
  const std::string entrytype = (entry == MDNS_ENTRYTYPE_ANSWER) ?
  "answer" :
  ((entry == MDNS_ENTRYTYPE_AUTHORITY) ? 
    "authority" : "additional");
  std::string entrystr(mdns_string_extract(data, size, &name_offset, entrybuffer, sizeof(entrybuffer)).str);

  // PTR: ip address -> domain name
  if (rtype == MDNS_RECORDTYPE_PTR) {
    std::string namestr(mdns_record_parse_ptr(data, size, record_offset, record_length,
      namebuffer, sizeof(namebuffer)).str);

    printf("%.*s : %s %.*s PTR %.*s rclass 0x%x ttl %u length %d\n",
     from_addr_str, entrytype, entrystr, namestr, rclass, ttl, 
     (int)record_length);
    if (!m_urlTable->contains(entrystr))
    {
      (*m_urlTable)[entrystr] = absl::flat_hash_set<std::string>();
    }
    (*m_urlTable)[entrystr].insert(from_addr_str);

  // SRV: info about a service. in this case, we treat a webpack as a service.
  } else if (rtype == MDNS_RECORDTYPE_SRV) {
    mdns_record_srv_t srv = 
    mdns_record_parse_srv(data, size, record_offset, record_length,
      namebuffer, sizeof(namebuffer));

    printf("%.*s : %s %.*s SRV %.*s priority %d weight %d port %d\n",
     from_addr_str, entrytype, entrystr, srv.name, srv.priority, 
     srv.weight, srv.port);

    std::pair<std::string, std::string> hostnameAndService = 
    parse_srv_name(std::string(srv.name.str));


    // Add entry to table. This may be refactored into a function later.
    if (!m_urlTable->contains(hostnameAndService.first))
    {
      (*m_urlTable)[hostnameAndService.first] = absl::flat_hash_set<std::string>();
    }

    (*m_urlTable)[hostnameAndService.first].insert(hostnameAndService.second);

  // A: domain name -> ip address
  } else if (rtype == MDNS_RECORDTYPE_A) {
    struct sockaddr_in addr;
    mdns_record_parse_a(data, size, record_offset, record_length, &addr);

    std::string addr_str =
    ip_address_to_string(namebuffer, sizeof(namebuffer), &addr, 
     sizeof(addr));

    printf("%.*s : %s %.*s A %.*s\n", from_addr_str, entrytype,
     entrystr, addr_str);

  } else {
    printf("%.*s : %s %.*s type %u rclass 0x%x ttl %u length %d\n",
     from_addr_str, entrytype, entrystr, rtype, rclass, ttl, 
     (int)record_length);
  }
  return 0;
}

int
NetworkController::service_callback(int sock, const struct sockaddr* from, size_t addrlen, mdns_entry_type_t entry,
 uint16_t query_id, uint16_t rtype, uint16_t rclass, uint32_t ttl, const void* data,
 size_t size, size_t name_offset, size_t name_length, size_t record_offset,
 size_t record_length, void* user_data) {
  static char sendbuffer[256];
  static char namebuffer[256];
  static char addrbuffer[64];

// only process questions
  if (entry != MDNS_ENTRYTYPE_QUESTION)
    return 0;

// get address
  std::string fromaddrstr = ip_address_to_string(addrbuffer, sizeof(addrbuffer), (const struct sockaddr_in*)from, addrlen);
  if (rtype == MDNS_RECORDTYPE_PTR) {
    std::string service(mdns_record_parse_ptr(data, size, record_offset, record_length,
      namebuffer, sizeof(namebuffer)).str);
    printf("%.*s : question PTR %.*s\n", fromaddrstr, service);

  // if host has web bundle, respond
    if (has_service(service)) {
      uint16_t unicast = rclass & MDNS_UNICAST_RESPONSE;

      printf("  --> answer %s.%s port %d (%s)\n", m_hostname, service,
       m_port, (unicast ? "unicast" : "multicast"));

      if (!unicast)
        addrlen = 0;

      mdns_query_answer(sock, from, addrlen, sendbuffer, sizeof(sendbuffer), 
        query_id, service.c_str(), service.length(), 
        m_hostname.c_str(), m_hostname.length(), m_addr->sin_addr.s_addr,
                          0 /* ipv6 addr */, (uint16_t)m_port,
                          0 /* txt */, 0 /* txt length */);
      }
    } else if (rtype == MDNS_RECORDTYPE_SRV) {
      mdns_record_srv_t service = 
      mdns_record_parse_srv(data, size, record_offset, record_length,
        namebuffer, sizeof(namebuffer));

      printf("%.*s : question SRV %.*s\n", fromaddrstr, MDNS_STRING_FORMAT(service.name));

  // if host has service, respond
      if (has_service(std::string(service.name.str)))
      {
        mdns_query_answer(sock, from, addrlen, sendbuffer, sizeof(sendbuffer), query_id,
          service.name.str, service.name.length, m_hostname.c_str(),
                      m_hostname.length(), m_addr->sin_addr.s_addr, 0 /* ipv6 addr */, 
                      (uint16_t)m_port, 0 /* txt */, 0 /* txt length */);
        }
      }
      return 0;
    }

std::string
NetworkController::ip_address_to_string(char* buffer, size_t capacity, 
  const struct sockaddr_in* addr, size_t addrlen) {

  char host[NI_MAXHOST] = {0};
  char service[NI_MAXSERV] = {0};
  int ret = getnameinfo((const struct sockaddr*)addr, (socklen_t)addrlen, host, NI_MAXHOST,
    service, NI_MAXSERV, NI_NUMERICSERV | NI_NUMERICHOST);

  if (ret == 0) {
    if (addr->sin_port != 0)
      snprintf(buffer, capacity, "%s:%s", host, service);
    else
      snprintf(buffer, capacity, "%s", host);
  }

  return std::string(buffer);
}

std::pair<std::string, std::string> NetworkController::parse_srv_name(std::string srv_name)
{
  std::vector<std::string> v = absl::StrSplit(srv_name, ".");
  return std::pair<std::string, std::string>(v.front(), v.back());
}
