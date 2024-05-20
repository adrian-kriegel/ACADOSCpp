
#include "ACADOSCpp/Capsule.hpp"

#include <dlfcn.h>
#include <stdexcept>
#include <string>

using namespace acados;
using namespace acados::solver;

Capsule::Capsule(Capsule &&other)
    : ACADOSData(other), prefix_(std::move(other.prefix_)),
      capsule_(std::move(other.capsule_)),
      dl_handle_(std::move(other.dl_handle_)) {
  other.capsule_ = nullptr;
  other.dl_handle_ = nullptr;
}

Capsule::Capsule(const std::string &lib, const std::string &prefix)
    : prefix_(prefix), dl_handle_(dlopen(lib.c_str(), RTLD_LAZY)) {

  if (!dl_handle_) {
    throw std::runtime_error("Failed to load " + lib + ": " + dlerror());
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
  reset_ = (reset_t*)get_symbol("reset");

  get_nlp(config_, "config");
  get_nlp(dims_, "dims");
  get_nlp(in_, "in");
  get_nlp(out_, "out");
  get_nlp(solver_, "solver");
  get_nlp(opts_, "opts");
}

Capsule::~Capsule() {

  if (capsule_) {
    typedef int (*free_capsule_t)(void *);

    // TODO: this may throw, causing dl_handle_ to leak
    auto free_capsule = (free_capsule_t)get_symbol("free_capsule");

    free_capsule(capsule_);
  }

  if (dl_handle_) {
    dlclose(dl_handle_);
  }
}

void *Capsule::get_symbol(const std::string &name) const {
  void *symbol = dlsym(dl_handle_, (prefix_ + "_acados_" + name).c_str());

  const char *dlsym_error = dlerror();

  if (dlsym_error) {
    throw std::runtime_error("Could not find symbol " + name);
  }

  return symbol;
}

void Capsule::solve() {

  int status = solve_(capsule_);

  if (status != ACADOS_SUCCESS) {
    throw std::runtime_error("ACADOS solve() returned status " +
                             std::to_string(status) + ".");
  }
}

void Capsule::print_stats() const { print_stats_(capsule_); }

void Capsule::reset(bool reset_qp_mem) {
  reset_(capsule_, (int)reset_qp_mem);
}

void Capsule::set_constraints_for_stage(uint stage, const std::string &field,
                                        double *values) {

  ocp_nlp_constraints_model_set(config_, dims_, in_, stage, field.c_str(),
                                values);
}

void Capsule::set_output(uint stage, const std::string &field, double *values) {
  ocp_nlp_out_set(config_, dims_, out_, stage, field.c_str(), values);
}

void Capsule::set_option(const std::string &field, void *value) {
  ocp_nlp_solver_opts_set(config_, opts_, field.c_str(), value);
}

void Capsule::get_output(void *out, uint stage,
                         const std::string &field) const {
  ocp_nlp_out_get(config_, dims_, out_, stage, field.c_str(), out);
}

void Capsule::get(void *out, const std::string &field) const {
  ocp_nlp_get(config_, solver_, field.c_str(), out);
}

void Capsule::eval_cost() {
  ocp_nlp_eval_cost(solver_, in_, out_);
}

int Capsule::get_constraint_dims(uint stage, const std::string &field) const {
  int res;

  // excuse me, but fuck me wtf
  config_->constraints[stage]->dims_get(config_->constraints[stage],
                                        dims_->constraints[stage], "ni", &res);

  return res;
}

void Capsule::set_solver_option(const std::string &field, void *value) {
  ocp_nlp_solver_opts_set(config_, opts_, field.c_str(), value);
}

void Capsule::set_cost_model(uint stage, const std::string& field, void* value) {
  ocp_nlp_cost_model_set(config_, dims_, in_, stage, field.c_str(), value);
}