// =================================================================================
// Set the attributes of the primary field variables
// =================================================================================
void variableAttributeLoader::loadPostProcessorVariableAttributes(){
	// Variable 0
	set_variable_name				(0,"error");
	set_variable_type				(0,SCALAR);

	set_need_value_residual_term	(0,true);
	set_need_gradient_residual_term	(0,false);

    set_output_integral         	(0,true);

	// Variable 1
	set_variable_name				(1,"f_tot");
	set_variable_type				(1,SCALAR);

	set_need_value_residual_term	(1,true);
	set_need_gradient_residual_term	(1,false);

    set_output_integral         	(1,true);


	// Variable 1
	set_variable_name				(2,"src");
	set_variable_type				(2,SCALAR);

	set_need_value_residual_term	(2,true);
	set_need_gradient_residual_term	(2,false);

	set_output_integral         	(2,true);

	// Variable 1
	set_variable_name				(3,"n_sol");
	set_variable_type				(3,SCALAR);

	set_need_value_residual_term	(3,true);
	set_need_gradient_residual_term	(3,false);

	set_output_integral         	(3,true);

}

template <int dim,int degree>
void customPDE<dim,degree>::postProcessedFields(const variableContainer<dim,degree,dealii::VectorizedArray<double> > & variable_list,
				variableContainer<dim,degree,dealii::VectorizedArray<double> > & pp_variable_list,
												const dealii::Point<dim, dealii::VectorizedArray<double> > q_point_loc) const {

// The order parameter and its derivatives
scalarvalueType n = variable_list.get_scalar_value(0);
scalargradType nx = variable_list.get_scalar_gradient(0);

scalarvalueType f_tot = constV(0.0);

// The homogenous free energy
scalarvalueType f_chem = (n*n*n*n - 2.0*n*n*n + n*n);

// The gradient free energy
scalarvalueType f_grad = constV(0.0);

for (int i=0; i<dim; i++){
  for (int j=0; j<dim; j++){
	  f_grad += constV(0.5*KnV)*nx[i]*nx[j];
  }
}

// The total free energy
f_tot = f_chem + f_grad;

scalarvalueType source_term;
scalarvalueType n_sol;
double pi = 2.0*std::acos(0.0);

for (unsigned i=0; i<n.n_array_elements;i++){

	double t = this->currentTime;

	source_term[i] = A1*A1*A2*A2/(2.0*dealii::Utilities::fixed_power<2>(std::cosh((-r0 + q_point_loc(1)[i] - t * std::sin(B1*pi*(q_point_loc(0)[i] + C1))*A1 - std::sin(D2*pi*t) * std::sin(B2*pi*q_point_loc(0)[i])*A2 )/std::sqrt(2.0*KnV))))
				* (
					(pi/A1*(D2*std::cos(D2*pi*t)+B2*B2*KnV*MnV*pi*std::sin(D2*pi*t)) * std::sin(B2*pi*q_point_loc(0)[i]) + (1.0+B1*B1*KnV*MnV*pi*pi*t)/A2*std::sin(B1*pi*(q_point_loc(0)[i]+C1))) / (std::sqrt(2.0*KnV) * A1*A2)
					+ MnV/2.0*( (-2.0*pi*pi*B1*B1*t*t/(A2*A2)*dealii::Utilities::fixed_power<2>(std::cos(B1*pi*(q_point_loc(0)[i]+C1)))-4.0*pi*pi*B1*B2*t/(A1*A2)*std::cos(B2*pi*q_point_loc(0)[i])*std::cos(B1*pi*(q_point_loc(0)[i]+C1))*std::sin(D2*pi*t)
					- 2.0*pi*pi*B2*B2/(A1*A1)*dealii::Utilities::fixed_power<2>(std::cos(B2*pi*q_point_loc(0)[i]))  * dealii::Utilities::fixed_power<2>(std::sin(D2*pi*t))) )
					* std::tanh((-r0 + q_point_loc(1)[i] - t * std::sin(B1*pi*(q_point_loc(0)[i] + C1))*A1 - std::sin(D2*pi*t) * std::sin(B2*pi*q_point_loc(0)[i])*A2)/std::sqrt(2.0*KnV))
				);


	double perturb = r0 + t*A1 * std::sin(B1*pi*(q_point_loc(0)[i] + C1)) + std::sin(pi*t*D2)*A2 * std::sin(B2*pi*q_point_loc(0)[i]);
	n_sol[i] = 0.5 * (1.0-std::tanh((q_point_loc(1)[i]-perturb)/std::sqrt(2.0*KnV)));
}

scalarvalueType error = std::abs(n_sol - n);

// Residuals for the equation to evolve the order parameter
pp_variable_list.set_scalar_value_residual_term(0, error);

pp_variable_list.set_scalar_value_residual_term(1, f_tot);

pp_variable_list.set_scalar_value_residual_term(2, source_term);

pp_variable_list.set_scalar_value_residual_term(3, n_sol);


}
