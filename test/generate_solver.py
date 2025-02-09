#!/usr/bin/env python

'''
'''

import os

import casadi as ca

import numpy as np

from acados_template import AcadosOcp, AcadosOcpSolver, AcadosModel
from acados_template.builders import CMakeBuilder

nu = 3
nx = 2*nu

model = AcadosModel()


model.name = 'test_model'

x1 = ca.MX.sym('x1', nu)
x2 = ca.MX.sym('x2', nu)

model.x = ca.vertcat(x1, x2)
model.u = ca.MX.sym('u', nu)

model.f_expl_expr = ca.vertcat(x2, model.u)

ocp = AcadosOcp()

ocp.model = model

ocp.solver_options.tf = 1.0
ocp.dims.N = 20

ocp.cost.cost_type = 'NONLINEAR_LS'

ocp.model.cost_y_expr = ocp.model.x
ocp.model.cost_y_expr_e = ocp.model.x

ocp.cost.yref = np.zeros(ocp.model.cost_y_expr.rows())
ocp.cost.W = np.eye(ocp.model.cost_y_expr.rows(), ocp.model.cost_y_expr.rows())

ocp.cost.cost_type_e = 'NONLINEAR_LS'
ocp.cost.W_e = np.eye(ocp.model.cost_y_expr_e.rows(), ocp.model.cost_y_expr_e.rows())
ocp.cost.yref_e = np.zeros(ocp.model.cost_y_expr_e.rows())

ocp.constraints.lbu = -np.ones(nu)
ocp.constraints.ubu = np.ones(nu)
ocp.constraints.idxbu = np.arange(nu)

ocp.solver_options.integrator_type = 'ERK'
ocp.solver_options.qp_solver_cond_N = 1  # Condensing steps
ocp.solver_options.nlp_solver_type = 'SQP_RTI'

code_dir = os.path.join(os.path.dirname(__file__), '../codegen/test_model')

os.makedirs(code_dir, exist_ok=True)
os.chdir(code_dir)

ocp.code_export_directory = code_dir 

builder = CMakeBuilder()

builder.options_on = [
  'BUILD_ACADOS_OCP_SOLVER_LIB',
]

builder.options_off = [
  'BUILD_EXAMPLE'
]

ocp_solver = AcadosOcpSolver(
  ocp, 
  json_file='acados_ocp.json', 
  build=True,
  generate=True,
  cmake_builder=builder
)

