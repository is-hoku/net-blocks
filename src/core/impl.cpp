#include "core/impls.h"
#include <fstream>

namespace net_blocks {

// Initialize context layout with member definitions
static void init_ctx_layout() {
  ctx_layout.register_member<builder::dyn_var<void *>>("nb__net_state");
  ctx_layout.register_member<builder::dyn_var<unsigned long long>>(
      "nb__time_now");
  ctx_layout.register_member<builder::dyn_var<unsigned int>>("nb__my_host_id");
  ctx_layout.register_member<builder::dyn_var<unsigned long long>>(
      "nb__my_local_host_id");
  ctx_layout.register_member<builder::dyn_var<unsigned long long>>(
      "nb__wildcard_host_identifier");
}

static void generate_headers(std::ostream &oss) {
  oss << "#include \"nb_runtime.h\"" << std::endl;
}

// ==================== Original (global) versions ====================

static builder::dyn_var<connection_t *>
establish_wrapper(builder::dyn_var<unsigned int> h,
                  builder::dyn_var<unsigned int> app,
                  builder::dyn_var<unsigned int> sa, callback_t c) {
  return interface_module::instance.establish_impl(h, app, sa, c);
}
static void generate_establish(std::ostream &oss) {
  auto ast = builder::builder_context().extract_function_ast(establish_wrapper,
                                                             "nb__establish");
  block::eliminate_redundant_vars(ast);
  block::c_code_generator::generate_code(ast, oss);
}

static void destablish_wrapper(builder::dyn_var<connection_t *> c) {
  interface_module::instance.destablish_impl(c);
}
static void generate_destablish(std::ostream &oss) {
  auto ast = builder::builder_context().extract_function_ast(destablish_wrapper,
                                                             "nb__destablish");
  block::eliminate_redundant_vars(ast);
  block::c_code_generator::generate_code(ast, oss);
}

static builder::dyn_var<int> send_wrapper(builder::dyn_var<connection_t *> c,
                                          builder::dyn_var<char *> buff,
                                          builder::dyn_var<int> len) {
  return interface_module::instance.send_impl(c, buff, len);
}
static void generate_send(std::ostream &oss) {
  auto ast =
      builder::builder_context().extract_function_ast(send_wrapper, "nb__send");
  block::eliminate_redundant_vars(ast);
  block::c_code_generator::generate_code(ast, oss);
}

static void run_ingress_step_wrapper(builder::dyn_var<void *> p,
                                     builder::dyn_var<int> len) {
  interface_module::instance.run_ingress_step(p, len);
}
static void generate_ingress_step(std::ostream &oss) {
  auto ast = builder::builder_context().extract_function_ast(
      run_ingress_step_wrapper, "nb__run_ingress_step");
  block::eliminate_redundant_vars(ast);
  block::c_code_generator::generate_code(ast, oss);
}

static void net_init_wrapper(void) {
  interface_module::instance.net_init_impl();
}
static void generate_net_init(std::ostream &oss) {
  auto ast = builder::builder_context().extract_function_ast(net_init_wrapper,
                                                             "nb__net_init");
  block::eliminate_redundant_vars(ast);
  block::c_code_generator::generate_code(ast, oss);
}

// ==================== Context-based versions ====================

static void net_init_ctx_wrapper(builder::dyn_var<runtime::context_t *> ctx) {
  interface_module::instance.net_init_impl_ctx(ctx);
}
static void generate_net_init_ctx(std::ostream &oss) {
  auto ast = builder::builder_context().extract_function_ast(
      net_init_ctx_wrapper, "nb__net_init_ctx");
  block::eliminate_redundant_vars(ast);
  block::c_code_generator::generate_code(ast, oss);
}

static builder::dyn_var<connection_t *>
establish_ctx_wrapper(builder::dyn_var<runtime::context_t *> ctx,
                      builder::dyn_var<unsigned int> h,
                      builder::dyn_var<unsigned int> app,
                      builder::dyn_var<unsigned int> sa, callback_t c) {
  return interface_module::instance.establish_impl_ctx(ctx, h, app, sa, c);
}
static void generate_establish_ctx(std::ostream &oss) {
  auto ast = builder::builder_context().extract_function_ast(
      establish_ctx_wrapper, "nb__establish_ctx");
  block::eliminate_redundant_vars(ast);
  block::c_code_generator::generate_code(ast, oss);
}

static void destablish_ctx_wrapper(builder::dyn_var<runtime::context_t *> ctx,
                                   builder::dyn_var<connection_t *> c) {
  interface_module::instance.destablish_impl_ctx(ctx, c);
}
static void generate_destablish_ctx(std::ostream &oss) {
  auto ast = builder::builder_context().extract_function_ast(
      destablish_ctx_wrapper, "nb__destablish_ctx");
  block::eliminate_redundant_vars(ast);
  block::c_code_generator::generate_code(ast, oss);
}

static builder::dyn_var<int>
send_ctx_wrapper(builder::dyn_var<runtime::context_t *> ctx,
                 builder::dyn_var<connection_t *> c,
                 builder::dyn_var<char *> buff, builder::dyn_var<int> len) {
  return interface_module::instance.send_impl_ctx(ctx, c, buff, len);
}
static void generate_send_ctx(std::ostream &oss) {
  auto ast = builder::builder_context().extract_function_ast(send_ctx_wrapper,
                                                             "nb__send_ctx");
  block::eliminate_redundant_vars(ast);
  block::c_code_generator::generate_code(ast, oss);
}

static void
run_ingress_step_ctx_wrapper(builder::dyn_var<runtime::context_t *> ctx,
                             builder::dyn_var<void *> p,
                             builder::dyn_var<int> len) {
  interface_module::instance.run_ingress_step_ctx(ctx, p, len);
}
static void generate_ingress_step_ctx(std::ostream &oss) {
  auto ast = builder::builder_context().extract_function_ast(
      run_ingress_step_ctx_wrapper, "nb__run_ingress_step_ctx");
  block::eliminate_redundant_vars(ast);
  block::c_code_generator::generate_code(ast, oss);
}

// ==================== Layout generation ====================

static void generate_connection_layout(std::string fname) {
  std::ofstream hoss(fname);
  hoss << "#pragma once" << std::endl;
  hoss << "#include \"nb_data_queue.h\"" << std::endl;
  hoss << std::endl;
  conn_layout.generate_struct_decl(hoss, "nb__connection_t");
  net_state.generate_struct_decl(hoss, "nb__net_state_t");
  hoss << "static const int nb__packet_headroom = " << get_headroom() << ";"
       << std::endl;
  hoss.close();
}

void run_nb_pipeline(std::string header_file, std::string output_file) {

  // Initialize context layout before generating code
  init_ctx_layout();

  generate_connection_layout(header_file);

  std::ofstream oss(output_file);
  generate_headers(oss);

  // Generate global (backward-compatible) versions
  generate_net_init(oss);
  generate_establish(oss);
  generate_destablish(oss);
  generate_send(oss);
  generate_ingress_step(oss);

  // Generate context-based versions
  generate_net_init_ctx(oss);
  generate_establish_ctx(oss);
  generate_destablish_ctx(oss);
  generate_send_ctx(oss);
  generate_ingress_step_ctx(oss);

  reliable_module::instance.gen_timer_callback(oss);
  oss.close();
}

} // namespace net_blocks
