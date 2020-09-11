#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>

#include "mdns/mdns.h"

typedef struct {
  const char* service;
  const char* hostname;
  uint32_t address_ipv4;
  uint8_t* address_ipv6;
  int port;
} service_record_t;

class NetworkController
{
public:
  NetworkController(absl::flat_hash_set<std::string> services);

  /**
   *  listen for incoming queries and responses. updates table and answers
   *  queries as able.
   */
  void listen(mdns_record_callback_fn callback);

  /** 
   *  send a query
   */ 
  void query(std::string service, mdns_record_callback_fn callback);

  /**
   *  process an answer to a query.
   */
  int query_callback(int sock, const struct sockaddr* from, 
    size_t addr_len, mdns_entry_type_t entry,
    uint16_t query_id, uint16_t rtype, uint16_t rclass, 
    uint32_t ttl, const void* data, size_t size, 
    size_t name_offset, size_t name_length, 
    size_t record_offset, size_t record_length, 
    void* user_data);

  /**
   *  process a question -- send a response if needed.
   */
  int service_callback(int sock, const struct sockaddr* from, 
    size_t addrlen, mdns_entry_type_t entry,
    uint16_t query_id, uint16_t rtype, uint16_t rclass, 
    uint32_t ttl, const void* data, size_t size, 
    size_t name_offset, size_t name_length, 
    size_t record_offset, size_t record_length, 
    void* user_data);

private:
  /**
   *  sets up port and socket for sending and receiving mDNS queries.
   *  IPv4 only.
   */
  void setup_ipv4();

  /**
   *  convert an ip address to string format.
   */
  std::string ip_address_to_string(char* buffer, size_t capacity, 
    const struct sockaddr_in* addr,
    size_t addrlen);

  /**
   *  parse name returned in srv record into service name (web bundle name)
   *  and host name.
   */
  std::pair<std::string, std::string> parse_srv_name(std::string srv_name);

  /**
   *  return true if host has service, false otherwise.
   */
  bool has_service(std::string srv_name)
  {
    return m_services.contains(srv_name);
  }

  /** table mapping from hostname to available web bundles */
  absl::flat_hash_map<std::string, absl::flat_hash_set<std::string>> *m_urlTable;

  /** services (web bundles) that this host has */
  absl::flat_hash_set<std::string> m_services;

  /** network socket */
  int m_socket;

  /** name of host */
  std::string m_hostname;

  /** port of host */
  int m_port;

  /** ipv4 address of host */
  struct sockaddr_in* m_addr;
};
