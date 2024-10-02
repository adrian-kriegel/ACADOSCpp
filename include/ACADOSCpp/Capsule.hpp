
#ifndef _ACADOS_CPP_SOLVER_HPP_
#define _ACADOS_CPP_SOLVER_HPP_

#include <string>

#include <acados_c/external_function_interface.h>
#include <acados_c/ocp_nlp_interface.h>

#include <ocp_nlp/ocp_nlp_common.h>

namespace acados::solver {

// yes.
using capsule_ptr = void *;
/** acados_solve(capsule_) */
using solve_t = int(capsule_ptr);
using print_stats_t = void(capsule_ptr);
using reset_t = void(capsule_ptr, int);

/**
 * This struct contains all ACADOS related pointers.
 * I bunched them all together to make the move ctor of Capsule simpler.
 */
class ACADOSData {

public:
  const ocp_nlp_dims &dims() const { return *dims_; }

public:
  ocp_nlp_config *config_ = nullptr;

  ocp_nlp_in *in_ = nullptr;
  ocp_nlp_out *out_ = nullptr;
  ocp_nlp_solver *solver_ = nullptr;
  void *opts_ = nullptr;

  ocp_nlp_dims *dims_{};

  // ptr to acados solve function
  solve_t *solve_;

  // ptr to acados print_stats function
  print_stats_t *print_stats_;

  reset_t *reset_;
};

class Capsule : public ACADOSData {
public:
  /**
   * Create a solver capsule.
   * @param lib Name or path of OCP solver .so file.
   * @param prefix Prefix prepended to acados functions within the .so file.
   * TODO: This constructor may throw. I would prefer a factory instead
   */
  Capsule(const std::string &lib, const std::string &prefix);

  /**
   * Construct a capsule from an existing dynamic library handle.
   * IMPORTANT: The capsule will take ownership of the handle and free it on
   * destruction.
   * @param dl_handle Dynamic library handle.
   * @param prefix Prefix prepended to acados functions within the .so file.
   */
  Capsule(void *&&dl_handle, const std::string &prefix);

  Capsule() = default;

  Capsule(Capsule &&other);

  const std::string &get_prefix() const { return prefix_; }

  /**
   * @throws runtime_error TODO: create solve_exception
   */
  void solve();

  void print_stats() const;

  void reset(bool reset_qp_mem = true);

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
  void set_option(const std::string &field, const void *value);

  void set_solver_option(const std::string &field, const void *value);

  void get_output(void *out, uint stage, const std::string &field) const;

  void set_cost_model(uint stage, const std::string &field, const void *value);

  /**
   * Wrapper for ocp_nlp_get which has *zero* documentation and idk what it does
   * tbh.
   */
  void get(void *out, const std::string &field) const;

  int get_constraint_dims(uint stage, const std::string &field) const;

  /** ocp_nlp_eval_cost */
  void eval_cost();

  /** ocp_nlp_eval_residuals */
  void eval_residuals();

  /** @returns ptr to the dynamic library handle. */
  void *get_dl_handle() const { return dl_handle_; }

  /** @returns ptr to the internal ACADOS capsule. */
  capsule_ptr get_capsule_ptr() const { return capsule_; }

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

protected:
  // prefix before every symbol prepended by the codegen
  std::string prefix_;

protected:
  capsule_ptr capsule_ = nullptr;

  // dynamic library handle
  void *dl_handle_ = nullptr;

private:
  // making copy ctor private ensures ownership of capsule_ and dl_handle_
  Capsule(const Capsule &) = delete;

}; // class Capsule

} // namespace acados::solver

#include "ACADOSCpp/Capsule.ipp"

#endif // _ACADOS_CPP_SOLVER_HPP_
