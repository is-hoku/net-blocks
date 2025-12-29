#ifndef NET_BLOCKS_FRAMEWORK_H
#define NET_BLOCKS_FRAMEWORK_H

#include "core/module.h"
#include "core/packet.h"
#include <vector>

namespace net_blocks {

extern dynamic_layout net_packet;
extern int get_headroom(void);

// Compat levels are in increasing order
// Eg. UDP_COMPAT implies ETH_COMPAT and IP_COMPAT
enum class compat_level_t {
  NO_COMPAT,
  ETH_COMPAT,
  IP_COMPAT,
  UDP_COMPAT,
  TCP_COMPAT
};

class module;

class framework {
public:
  std::vector<module *> m_registered_modules;

  std::vector<module *> m_establish_path;
  std::vector<module *> m_destablish_path;
  std::vector<module *> m_send_path;
  std::vector<module *> m_ingress_path;

  void finalize_paths(void);

  bool debug_paths = false;

  enum compat_level_t compat_level = compat_level_t::NO_COMPAT;

private:
  // Singleton class framework
  framework() = default;

public:
  static framework instance;

  void register_module(module *);

  // Implementations for various paths
  void run_establish_path(builder::dyn_var<connection_t *>,
                          builder::dyn_var<unsigned int>,
                          builder::dyn_var<unsigned int>,
                          builder::dyn_var<unsigned int>);
  void run_destablish_path(builder::dyn_var<connection_t *>);
  builder::dyn_var<int> run_send_path(builder::dyn_var<connection_t *>,
                                      builder::dyn_var<char *>,
                                      builder::dyn_var<int>);
  void run_ingress_path(packet_t);
  void run_net_init_path(void);

  // Context-based implementations for various paths
  void run_establish_path_ctx(builder::dyn_var<runtime::context_t *> ctx,
                              builder::dyn_var<connection_t *>,
                              builder::dyn_var<unsigned int>,
                              builder::dyn_var<unsigned int>,
                              builder::dyn_var<unsigned int>);
  void run_destablish_path_ctx(builder::dyn_var<runtime::context_t *> ctx,
                               builder::dyn_var<connection_t *>);
  builder::dyn_var<int>
  run_send_path_ctx(builder::dyn_var<runtime::context_t *> ctx,
                    builder::dyn_var<connection_t *>, builder::dyn_var<char *>,
                    builder::dyn_var<int>);
  void run_ingress_path_ctx(builder::dyn_var<runtime::context_t *> ctx,
                            packet_t);
  void run_net_init_path_ctx(builder::dyn_var<runtime::context_t *> ctx);

  bool isEthCompat() {
    if (compat_level == compat_level_t::ETH_COMPAT ||
        compat_level == compat_level_t::IP_COMPAT ||
        compat_level == compat_level_t::UDP_COMPAT ||
        compat_level == compat_level_t::TCP_COMPAT)
      return true;
    return false;
  }

  bool isIPCompat() {
    if (compat_level == compat_level_t::IP_COMPAT ||
        compat_level == compat_level_t::UDP_COMPAT ||
        compat_level == compat_level_t::TCP_COMPAT)
      return true;
    return false;
  }

  bool isUDPCompat() {
    if (compat_level == compat_level_t::UDP_COMPAT)
      return true;
    return false;
  }
};

} // namespace net_blocks
#endif
