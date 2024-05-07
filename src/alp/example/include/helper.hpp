#ifndef EXAMPLE_HELPER_HPP
#define EXAMPLE_HELPER_HPP

namespace example {
inline  void fill_random_data(double* in, size_t tuples_to_generate, uint8_t precision) {
	std::uniform_real_distribution<double> unif(100, 300);
	std::default_random_engine             re;
	re.seed(42);
	uint8_t      doubles_intrinsic_precision = precision;
	const double precision_multiplier        = std::pow(10.0, doubles_intrinsic_precision);
	for (size_t i = 0; i < tuples_to_generate; i++) {
		double random_value = unif(re);
		double fixed_precision_random_value =
			std::round(random_value * precision_multiplier) / precision_multiplier;
		in[i] = fixed_precision_random_value;
	}
}

}

#endif //EXAMPLE_HELPER_HPP
