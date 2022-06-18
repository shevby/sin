#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"
#include "sin.h"

TEST_CASE("Check setters and getters")
{
  Sin a = 1;

  REQUIRE(a.asInt32() == 1);

  a["someField"] = 2000000000;

  REQUIRE(a["someField"].asInt32() == 2000000000);

  a = "Hello World!";

  REQUIRE(a.asString() == "Hello World!");

  a = {0};

  for (int i = 0; i < 10; i++)
  {
    a[i] = i * i;
  }

  for (int i = 0; i < 10; i++)
  {
    INFO(i);
    REQUIRE(a[i].asInt32() == i * i);
  }

  a = {1, 2, "5"};

  a[3] = {"another array"};

  // INFO(a.toString());
  REQUIRE(a[0].asInt32() == 1);
  REQUIRE(a[1].asInt32() == 2);
  REQUIRE(a[2].asString() == "5");
  REQUIRE(a[3][0].asString() == "another array");
}

TEST_CASE("Ceck default initializers")
{
  Sin a;

  a = {};

  REQUIRE(a.type() == "Object");

  a = Sin::Object();

  REQUIRE(a.type() == "Object");

  a = Sin::Array();

  REQUIRE(a.type() == "Array");

  REQUIRE(a.asArray().size() == 0);

  a = {1, 2, 3};

  REQUIRE(a.type() == "Array");

  REQUIRE(a.asArray().size() == 3);
}

TEST_CASE("Check Parser and stringification matching")
{
  Sin a = {};
  REQUIRE(Sin::parse(a.toString()).toString() == a.toString());

  a = Sin::Object();
  REQUIRE(Sin::parse(a.toString()).toString() == a.toString());

  a = Sin::Array();
  REQUIRE(Sin::parse(a.toString()).toString() == a.toString());

  a = {1, 2, 3, 4, 5};
  REQUIRE(Sin::parse(a.toString()).toString() == a.toString());
  REQUIRE(Sin::parse(a.toString()).type() == "Array");

  a = Sin::Object();
  a["a"] = 1;
  a["b"] = "hi";
  a["c"] = {1, 2, uint64_t{500000}, int64_t{10000}};
  a["d"] = {};
  a["d"]["de"] = float{5};

  Sin g = {};

  g["dfgh"] = 123;
  g["dfgi"] = float{2000000000};
  g["dfgj"] = "hi there :3";
  g["dfgk"] = {5, 5.5, 6, (int64_t)-5000000000, "and there"};
  g["dfgl"] = "and there!!!";

  a["d"]["df"] = {double{6}, 5.5, 10, g};

  INFO(a.toString());

  REQUIRE(Sin::parse(a.toString()).toString() == a.toString() + "1");
}