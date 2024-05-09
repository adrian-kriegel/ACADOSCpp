
#ifndef _ACADOS_CPP_SOLVER_HPP_
#define _ACADOS_CPP_SOLVER_HPP_

#include <string>

#include <acados_c/external_function_interface.h>
#include <acados_c/ocp_nlp_interface.h>
#include <ocp_nlp/ocp_nlp_common.h>

namespace acados::solver {

class Capsule {
public:
  /**
   * Create a solver capsule.
   * @param lib Name or path of OCP solver .so file.
   * @param prefix Prefix prepended to acado functions within the .so file.
   * TODO: This constructor may throw. I would prefer a factory instead
   */
  Capsule(const std::string &lib, const std::string &prefix);

  /**
   * @throws runtime_error TODO: create solve_exception
   */
  void solve() const;

  void print_stats() const;

  void set_constraints_for_stage(uint stage, const std::string &field,
                                 double *values);

  /**
   * Sets fields in the output struct of an nlp solver, used to initialize the
   * solver.
   */
  void set_output(uint stage, const std::string &field, double *values);

  /**
   * Sets an option on the opts_ struct.
   */
  void set_option(const std::string &field, void *value);

  void set_solver_option(const std::string &field, void *value);

  void get_output(void *out, uint stage, const std::string &field) const;

  /**
   * Wrapper for ocp_nlp_get which has *zero* documentation and idk what it does
   * tbh.
   */
  void get(void *out, const std::string &field) const;

  int get_constraint_dims(uint stage, const std::string &field) const;

  ~Capsule();

protected:
  /**
   * Get symbol from the dl prefixed by <prefix>_acado_
   */
  void *get_symbol(const std::string &name) const;

  /**
   * Shorthand for T target = <prefix>_acado_nlp_get_<name>(capsule_);
   */
  template <typename T> void get_nlp(T &target, const std::string &name) const {
    typedef T (*getter_t)(decltype(capsule_));

    getter_t getter = (getter_t)get_symbol("get_nlp_" + name);

    target = getter(capsule_);
  }

public:
  const ocp_nlp_dims &dims() const { return *dims_; }

  ocp_nlp_config *config_ = nullptr;

  ocp_nlp_in *in_ = nullptr;
  ocp_nlp_out *out_ = nullptr;
  ocp_nlp_solver *solver_ = nullptr;
  void *opts_ = nullptr;

protected:
  ocp_nlp_dims *dims_{};

protected:
  // yes.
  using capsule_ptr = void *;

  /** acados_solve(capsule_) */
  using solve_t = int(capsule_ptr);
  using print_stats_t = void(capsule_ptr);

  capsule_ptr capsule_ = nullptr;

  // ptr to acados solve function
  solve_t *solve_;

  // ptr to acados print_stats function
  print_stats_t *print_stats_;

protected:
  // prefix before every symbol prepended by the codegen
  std::string prefix_;

  // dynamic library handle
  void *dl_handle_;

}; // class Capsule

} // namespace acados::solver

#endif // _ACADOS_CPP_SOLVER_HPP_
