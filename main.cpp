#include <absl/container/flat_hash_set.h> 
#include <iostream>
#include <sys/epoll.h>

#include "NetworkController.hpp"

NetworkController nc;

static int wrapped_query_callback(int sock, const struct sockaddr* from, 
  size_t addr_len, mdns_entry_type_t entry,
  uint16_t query_id, uint16_t rtype, uint16_t rclass, 
  uint32_t ttl, const void* data, size_t size, 
  size_t name_offset, size_t name_length, 
  size_t record_offset, size_t record_length, 
  void* user_data)
{
  nc.query_callback(sock, from, addr_len, entry, query_id, rtype, rclass, 
    ttl, data, size, name_offset, name_length, 
    record_offset, record_length, user_data);
  return 0;
}

static int wrapped_service_callback(int sock, const struct sockaddr* from,
  size_t addr_len, mdns_entry_type_t entry,
  uint16_t query_id, uint16_t rtype, uint16_t rclass,
  uint32_t ttl, const void* data, size_t size,
  size_t name_offset, size_t name_length,
  size_t record_offset, size_t record_length,
  void* user_data)
{
  nc.service_callback(sock, from, addr_len, entry, query_id, rtype, rclass,                  
    ttl, data, size, name_offset, name_length,  
    record_offset, record_length, user_data);
  return 0;
}

// argc: [mode (listen or query), if query: wpack to query, if listen: services provided]
int main(int argc, const char* const* argv)
{
  int mode = -1;
  std::string service;
  absl::flat_hash_set<std::string> webbundle_list;
  for (int i = 0; i < argc; i++)
  {
    if (!strcmp(argv[i], "--query"))
    {
      mode = 1;
      i++;
      if (i < argc)
      {
        service = argv[i];
      }
    } else if (!strcmp(argv[i], "--listen"))
    {
      mode = 2;
      i++;
      while (i < argc)
      {
        webbundle_list.insert(argv[i]);
      	i++;
      }
    }
  }

  NetworkController nc = NetworkController(webbundle_list);
  if (mode == 1)
  {
    nc.query(service, wrapped_query_callback);
  }

  if (mode == 2)
  {
    nc.listen(wrapped_service_callback);
  }

  return 0;
}
