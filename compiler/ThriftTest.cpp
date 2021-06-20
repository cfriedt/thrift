#include <gtest/gtest.h>

using namespace std;

extern "C" {
extern int start_c_service(void);
extern void stop_c_service(void);
}

TEST(CGenerator, empty) {
  start_c_service();
  stop_c_service();
}