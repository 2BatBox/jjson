#pragma once

#include <cstdio>

#define TEST_TRACE {printf("-> %s::%s()\n", typeid(*this).name(), __FUNCTION__);}
#define TEST_PASS {printf("%sPASS%s\n", TermColor::front(TermColor::GREEN), TermColor::reset()); }

#define ASSERT(exp) { \
	if(not (exp)) { \
		printf("Assertion failure : %s:%d", __FUNCTION__, __LINE__); \
		fflush(stdout); \
		fflush(stderr); \
		exit(EXIT_FAILURE); \
	} \
};
