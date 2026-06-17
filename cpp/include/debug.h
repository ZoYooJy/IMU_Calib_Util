#pragma once

#include <iostream>

#define APP_INFO(...) std::cout << __VA_ARGS__ << std::endl
#define APP_ERROR(...) std::cerr << __VA_ARGS__ << std::endl
