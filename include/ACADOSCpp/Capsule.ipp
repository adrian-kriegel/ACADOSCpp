
#ifndef _ACADOS_CPP_SOLVER_CAPSULE_IPP_
#define _ACADOS_CPP_SOLVER_CAPSULE_IPP_

#include "ACADOSCpp/Capsule.hpp"

#include <dlfcn.h>
#include <stdexcept>
#include <string>

namespace acados::solver {

inline Capsule::Capsule(Capsule &&other)
    : ACADOSData(other), prefix_(std::move(other.prefix_)),
      capsule_(std::move(other.capsule_)),
      dl_handle_(std::move(other.dl_handle_)) {
  other.capsule_ = nullptr;
  other.dl_handle_ = nullptr;
}

inline Capsule::Capsule(void *&&dl_handle, const std::string &prefix)
    : prefix_(prefix), dl_handle_(std::move(dl_handle)) {

  //
  if (!dl_handle_) {
    throw std::runtime_error("Failed to load solver library: " +
                             std::string(dlerror()));
  }
  // reset error state
  dlerror();

  typedef void *(*create_capsule_t)(void);
  typedef int (*create_t)(void *);

  auto create_capsule = (create_capsule_t)get_symbol("create_capsule");
  auto create = (create_t)get_symbol("create");

  capsule_ = create_capsule();

  int status = create(capsule_);

  if (status != ACADOS_SUCCESS) {
    throw std::runtime_error("create(capsule) returned status " +
                             std::to_string(status));
  }

  solve_ = (solve_t *)get_symbol("solve");
  print_stats_ = (print_stats_t *)get_symbol("print_stats");
  reset_ = (reset_t *)get_symbol("reset");

  get_nlp(config_, "config");
  get_nlp(dims_, "dims");
  get_nlp(in_, "in");
  get_nlp(out_, "out");
  get_nlp(solver_, "solver");
  get_nlp(opts_, "opts");
}

inline Capsule::Capsule(const std::string &lib, const std::string &prefix)
    : Capsule(dlopen(lib.c_str(), RTLD_NOW), prefix) {}

inline Capsule::~Capsule() {

  if (capsule_) {
    using free_capsule_t = int(void *);

    // TODO: this may throw, causing dl_handle_ to leak
    auto free_capsule = (free_capsule_t *)get_symbol("free_capsule");

    free_capsule(capsule_);
  }

  if (dl_handle_) {
    dlclose(dl_handle_);
  }
}

inline void *Capsule::get_symbol(const std::string &name) const {
  void *symbol = dlsym(dl_handle_, (prefix_ + "_acados_" + name).c_str());

  const char *dlsym_error = dlerror();

  if (dlsym_error) {
    throw std::runtime_error("Could not find symbol " + name + ": " +
                             std::string(dlsym_error));
  }

  return symbol;
}

inline void Capsule::solve() {

  SolveResult status = solve_noexcept();

  if (status != SolveResult::SUCCESS) {
    throw std::runtime_error("ACADOS solve() returned status " +
                             std::to_string((int)status) + ".");
  }
}

inline SolveResult Capsule::solve_noexcept() noexcept {
  return (SolveResult)solve_(capsule_);
}

inline void Capsule::set_rti_phase(RTIPhase phase) {
  set_option("rti_phase", &phase);
}

inline void Capsule::print_stats() const { print_stats_(capsule_); }

inline void Capsule::reset(bool reset_qp_mem) {
  reset_(capsule_, (int)reset_qp_mem);
}

inline void Capsule::set_constraints_for_stage(uint stage,
                                               const std::string &field,
                                               const double *values) {
  // contents of value are always copied during the function call. (
  // See https://discourse.acados.org/t/life-time-of-pointers/1795
  // So we can use const_cast.
  ocp_nlp_constraints_model_set(config_, dims_, in_, stage, field.c_str(),
                                const_cast<double *>(values));
}

inline void Capsule::set_output(uint stage, const std::string &field,
                                const double *values) {
  ocp_nlp_out_set(config_, dims_, out_, stage, field.c_str(),
                  const_cast<double *>(values));
}

inline void Capsule::set_option(const std::string &field, const void *value) {

  ocp_nlp_solver_opts_set(config_, opts_, field.c_str(),
                          const_cast<void *>(value));
}

inline void Capsule::get_output(void *out, uint stage,
                                const std::string &field) const {
  ocp_nlp_out_get(config_, dims_, out_, stage, field.c_str(), out);
}

namespace internal {

/**
 * Compatibility helper for ocp_nlp_get from acados 0.3 to 0.4.
 */
template <typename Config, typename Solver>
auto ocp_nlp_get_compat(Config config, Solver solver, const char *field,
                        void *return_value)
    -> decltype(ocp_nlp_get(config, solver, field, return_value)) {
  ocp_nlp_get(config, solver, field, return_value);
}

template <typename Config, typename Solver>
auto ocp_nlp_get_compat(Config config, Solver solver, const char *field,
                        void *return_value)
    -> decltype(ocp_nlp_get(solver, field, return_value)) {
  ocp_nlp_get(solver, field, return_value);
}

} // namespace internal

inline void Capsule::get(void *out, const std::string &field) const {

  internal::ocp_nlp_get_compat(config_, solver_, field.c_str(), out);
}

inline void Capsule::eval_cost() { ocp_nlp_eval_cost(solver_, in_, out_); }

inline void Capsule::eval_residuals() {
  ocp_nlp_eval_residuals(solver_, in_, out_);
}

inline int Capsule::get_constraint_dims(uint stage,
                                        const std::string &field) const {
  return ocp_nlp_dims_get_from_attr(config_, dims_, out_, stage, field.c_str());
}

inline void Capsule::set_solver_option(const std::string &field,
                                       const void *value) {
  ocp_nlp_solver_opts_set(config_, opts_, field.c_str(),
                          const_cast<void *>(value));
}

inline void Capsule::set_cost_model(uint stage, const std::string &field,
                                    const void *value) {
  ocp_nlp_cost_model_set(config_, dims_, in_, stage, field.c_str(),
                         const_cast<void *>(value));
}

} // namespace acados::solver

#endif // _ACADOS_CPP_SOLVER_CAPSULE_IPP_