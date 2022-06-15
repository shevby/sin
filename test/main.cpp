#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"
#include "sin.h"

TEST_CASE("Check setters and getters") {
  Sin a = 1;

  REQUIRE(a.asInt32() == 1);

  a["someField"] = 2000000000;
  
  REQUIRE(a["someField"].asInt32() == 2000000000);

  a = "Hello World!";

  REQUIRE(a.asString() == "Hello World!");

  a = {0};

  for(int i = 0; i < 10; i++) {
    a[i] = i * i;
  }

  for(int i = 0; i < 10; i++) {
    INFO(i);
    REQUIRE(a[i].asInt32() == i*i);
  }


  a = {1, 2, "5"};

  a[3] = {"another array"};

  // INFO(a.toString());
  REQUIRE(a[0].asInt32() == 1);
  REQUIRE(a[1].asInt32() == 2);
  REQUIRE(a[2].asString() == "5");
  REQUIRE(a[3][0].asString() == "another array");

  
  
}