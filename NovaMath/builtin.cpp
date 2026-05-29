#include "builtin.h"
#include <cmath>
void sinFunc(VM& vm, std::vector<double>& args) {
	double value = std::sin(args[0]);
	RoundDouble(value);
	args[0] = value;
}
void cosFunc(VM& vm, std::vector<double>& args) {
	double value = std::cos(args[0]);
	RoundDouble(value);
	args[0] = value;
}
void tanFunc(VM& vm, std::vector<double>& args) {
	double value = std::tan(args[0]);
	RoundDouble(value);
	args[0] = value;
}
void sqrtFunc(VM& vm, std::vector<double>& args) {
	double value = std::sqrt(args[0]);
	RoundDouble(value);
	args[0] = value;
}
void absFunc(VM& vm, std::vector<double>& args) {
	double value = std::abs(args[0]);
	args[0] = value;
}
void RoundDouble(double& value)
{
	if (std::abs(value) < 1e-15) {
		value = 0.0;
	}
}
