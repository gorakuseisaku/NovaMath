#pragma once
#include <iostream>
#include <string>
inline void Error(const std::string& msg) {
	throw std::runtime_error("Error: " + msg);
}