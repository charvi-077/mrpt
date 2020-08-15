/* +------------------------------------------------------------------------+
   |                     Mobile Robot Programming Toolkit (MRPT)            |
   |                          https://www.mrpt.org/                         |
   |                                                                        |
   | Copyright (c) 2005-2020, Individual contributors, see AUTHORS file     |
   | See: https://www.mrpt.org/Authors - All rights reserved.               |
   | Released under BSD License. See: https://www.mrpt.org/License          |
   +------------------------------------------------------------------------+ */

#include <gtest/gtest.h>
#include <mrpt/config.h>
#include <mrpt/containers/yaml.h>

#include <algorithm>  // count()

TEST(yaml, emptyCtor)
{
	{
		mrpt::containers::yaml p;
		EXPECT_TRUE(p.empty());
	}
}

TEST(yaml, assignments)
{
	mrpt::containers::yaml p;
	p["K"] = 2.0;
	p["N"] = uint64_t(10);
	p["name"] = "Pepico";
	p["one"] = "1.0";
	p["enabled"] = true;
	p["visible"] = false;
	p["hidden"] = "true";

	EXPECT_FALSE(p.empty());
	EXPECT_TRUE(p.has("K"));

	EXPECT_FALSE(p.isScalar());
	EXPECT_TRUE(p["K"].isScalar());
	EXPECT_TRUE(p["N"].isScalar());

	EXPECT_TRUE(p["enabled"]);
	EXPECT_FALSE(p["visible"]);
	EXPECT_TRUE(p["hidden"]);

	EXPECT_EQ(p["K"].scalarType(), typeid(double));
	EXPECT_EQ(p["K"].as<double>(), 2.0);

	EXPECT_EQ(p["N"].scalarType(), typeid(uint64_t));
	EXPECT_EQ(p["N"].as<uint64_t>(), 10U);

	EXPECT_EQ(p["name"].scalarType(), typeid(std::string));
	EXPECT_EQ(p["name"].as<std::string>(), "Pepico");

	EXPECT_EQ(p["enabled"].scalarType(), typeid(bool));

	{
		mrpt::containers::yaml p2;
		EXPECT_TRUE(p2.empty());

		p2 = p;
		EXPECT_FALSE(p2.empty());
		EXPECT_TRUE(p2.has("K"));
	}

	// type conversions:
	EXPECT_EQ(p["K"].as<int>(), 2);
	EXPECT_EQ(p["N"].as<std::string>(), "10");
	EXPECT_EQ(p["N"].as<double>(), 10.0);
	EXPECT_EQ(p["one"].as<std::string>(), "1.0");
	EXPECT_EQ(p["one"].as<unsigned int>(), 1U);
	EXPECT_EQ(p["one"].as<int>(), 1);
	EXPECT_EQ(p["enabled"].as<std::string>(), "true");
	EXPECT_EQ(p["visible"].as<std::string>(), "false");
}

TEST(yaml, initializers)
{
	{
		auto p = mrpt::containers::yaml::Sequence({"K", 1.0});
		EXPECT_TRUE(p.isSequence());
	}
	{
		auto p = mrpt::containers::yaml::Map({{"K", 1.0}});
		EXPECT_TRUE(p.isMap());
	}

	{
		auto p = mrpt::containers::yaml({"K", 1.0, 10.0});
		EXPECT_TRUE(p.isSequence());
	}
	{
		auto p = mrpt::containers::yaml::Map(
			{{"K", 1.0}, {"T", 10.0}, {"Name", "bar"}});
		EXPECT_TRUE(p.isMap());
	}
}

TEST(yaml, initializerMap)
{
	const mrpt::containers::yaml p = mrpt::containers::yaml::Map(
		{{"K", 2.0}, {"book", std::string("silmarillion")}});

	EXPECT_FALSE(p.isSequence());
	EXPECT_TRUE(p.isMap());
	EXPECT_FALSE(p.empty());
	EXPECT_TRUE(p.has("K"));

	EXPECT_EQ(p["K"].scalarType(), typeid(double));
	EXPECT_EQ(p["K"].as<double>(), 2.0);

	EXPECT_EQ(p.getOrDefault("K", 1.0), 2.0);
	EXPECT_EQ(p.getOrDefault("Q", 1.0), 1.0);
	EXPECT_EQ(p.getOrDefault<uint32_t>("K", 1), 2U);

	EXPECT_EQ(p["book"].scalarType(), typeid(std::string));
	EXPECT_EQ(p["book"].as<std::string>(), "silmarillion");

	// non existing in const object:
	EXPECT_THROW(p["foo"], std::exception);
	// Access wrong type in const object:
	EXPECT_THROW(p["K"].asRef<std::string>(), std::exception);
}

TEST(yaml, initializerSequence)
{
	const auto seq1 = mrpt::containers::yaml({1.0, 2.0, 3.0});

	EXPECT_FALSE(seq1.empty());
	EXPECT_TRUE(seq1.isSequence());
	EXPECT_FALSE(seq1.isMap());
	EXPECT_THROW(seq1.has("foo"), std::exception);
	EXPECT_THROW(seq1["K"], std::exception);

	EXPECT_TRUE(static_cast<double>(seq1(0)) == 1.0);
	EXPECT_EQ(seq1(0).as<double>(), 1.0);
	EXPECT_EQ(seq1(0).as<double>(), 1.0);
	EXPECT_EQ(seq1(1).as<double>(), 2.0);
	EXPECT_EQ(seq1(2).as<double>(), 3.0);

	EXPECT_THROW(seq1(3), std::out_of_range);
	EXPECT_THROW(seq1(-1), std::out_of_range);

	auto seq2 = mrpt::containers::yaml({1.0, 2.0, 3.0});
	EXPECT_EQ(seq2(1).as<double>(), 2.0);

	seq2(1) = 42.0;
	EXPECT_EQ(seq2(1).as<double>(), 42.0);

	seq2(1) = "foo";
	EXPECT_EQ(seq2(1).as<std::string>(), std::string("foo"));

	seq2(1) = mrpt::containers::yaml::Map({{"K", 1.0}});
	EXPECT_EQ(seq2(1)["K"].as<double>(), 1.0);

	seq2.push_back(std::string("foo2"));
	seq2.push_back(9.0);

	EXPECT_EQ(seq2(3).as<std::string>(), std::string("foo2"));
	EXPECT_EQ(seq2(4).as<double>(), 9.0);
}

TEST(yaml, nested)
{
	mrpt::containers::yaml p = mrpt::containers::yaml::Map({{"K", 2.0}});
	p["PID"] = mrpt::containers::yaml::Map({{"Kp", 10.0}, {"Ti", 10.0}});

	EXPECT_FALSE(p.empty());
	EXPECT_FALSE(p["PID"].empty());

	EXPECT_EQ(p["PID"]["Ti"].scalarType(), typeid(double));
	EXPECT_EQ(p["PID"]["Ti"].as<double>(), 10.0);

	EXPECT_FALSE(p.isScalar());
	EXPECT_FALSE(p["PID"].isScalar());
	EXPECT_TRUE(p["PID"]["Kp"].isScalar());

	// empty() not valid for values:
	EXPECT_THROW(p["PID"]["Ti"].empty(), std::exception);
	EXPECT_THROW(p["PID"]["Ti"].clear(), std::exception);
	EXPECT_THROW(p["PID"]["Ti"].has("xxx"), std::exception);
	EXPECT_THROW(p["PID"]["Ti"]["xxx"].scalarType(), std::exception);

	// clear and recheck;
	p["PID"].clear();
	EXPECT_TRUE(p["PID"].empty());
}

TEST(yaml, nested2)
{
	mrpt::containers::yaml p;
	p["N"] = 10;
	auto& pid = p["PID"] = mrpt::containers::yaml();
	pid["Kp"] = 0.5;
	p["PID"]["Ti"] = 2.0;
	p["PID"]["N"] = 1000;
	p["PID"]["name"] = "foo";

	EXPECT_EQ(p["PID"]["Kp"].as<double>(), 0.5);
	EXPECT_EQ(p["PID"]["Ti"].as<double>(), 2.0);
	EXPECT_EQ(p["PID"]["N"].as<uint64_t>(), 1000U);
	EXPECT_EQ(p["PID"]["name"].as<std::string>(), std::string("foo"));
}

const mrpt::containers::yaml testMap = mrpt::containers::yaml::Map(
	{{"K", 2.0},
	 {"book", "silmarillion"},
	 {"mySequence",
	  mrpt::containers::yaml::Sequence(
		  {1.0, 2.0, mrpt::containers::yaml::Map({{"P", 1.0}, {"Q", 2.0}})})},
	 {"myEmptyVal", mrpt::containers::yaml()},
	 {"myDict",
	  mrpt::containers::yaml::Map({{"A", 1.0}, {"B", 2.0}, {"C", 3.0}})}});

TEST(yaml, printYAML)
{
	// testMap.printAsYAML();
	std::stringstream ss;
	testMap.printAsYAML(ss);
	const auto s = ss.str();
	EXPECT_EQ(std::count(s.begin(), s.end(), '\n'), 13U);
}

TEST(yaml, ctorMap)
{
	mrpt::containers::yaml c1 = mrpt::containers::yaml::Map();
	c1["K"] = 2.0;
	auto& m = c1["myDict"] = mrpt::containers::yaml::Map();
	m["A"] = 1.0;
	m["B"] = 2.0;

	const mrpt::containers::yaml c2 = mrpt::containers::yaml::Map(
		{{"K", 2.0},
		 {"myDict", mrpt::containers::yaml::Map({{"A", 1.0}, {"B", 2.0}})}});

	std::stringstream ss1, ss2;
	c1.printAsYAML(ss1);
	c2.printAsYAML(ss2);
	EXPECT_EQ(ss1.str(), ss2.str());
}

TEST(yaml, iterate)
{
	try
	{
		std::set<std::string> foundKeys;
		for (const auto& kv : testMap.asMap())
		{
			std::cout << kv.first << ":" << kv.second.typeName() << "\n";

			foundKeys.insert(kv.first);
		}
		EXPECT_EQ(foundKeys.size(), 5U);

		foundKeys.clear();
		for (const auto& kv : testMap["myDict"].asMap())
		{
			std::cout << kv.first << ":" << kv.second.typeName() << "\n";
			foundKeys.insert(kv.first);
		}
		EXPECT_EQ(foundKeys.size(), 3U);

		EXPECT_EQ(testMap["mySequence"].asSequence().size(), 3U);
	}
	catch (const std::exception& e)
	{
		std::cerr << mrpt::exception_to_str(e);
		GTEST_FAIL();
	}
}

TEST(yaml, macros)
{
	try
	{
		mrpt::containers::yaml p;
		p["K"] = 2.0;
		p["Ang"] = 90.0;
		p["name"] = "Pepico";
		p["PID"] = mrpt::containers::yaml::Map({{"Kp", 1.0}, {"Td", 0.8}});

		double K, Td, Foo = 9.0, Bar, Ang, Ang2 = M_PI;
		std::string name;
		MCP_LOAD_REQ(p, K);
		EXPECT_EQ(K, 2.0);

		MCP_LOAD_REQ(p, name);
		EXPECT_EQ(name, "Pepico");

		MCP_LOAD_REQ(p["PID"], Td);
		EXPECT_EQ(Td, 0.8);

		MCP_LOAD_REQ_DEG(p, Ang);
		EXPECT_NEAR(Ang, 0.5 * M_PI, 1e-6);

		MCP_LOAD_OPT_DEG(p, Ang2);
		EXPECT_NEAR(Ang2, M_PI, 1e-6);

		MCP_LOAD_OPT(p, Foo);
		EXPECT_EQ(Foo, 9.0);

		EXPECT_THROW(MCP_LOAD_REQ(p, Bar), std::exception);
	}
	catch (const std::exception& e)
	{
		std::cerr << mrpt::exception_to_str(e);
		GTEST_FAIL();
	}
}

void foo(mrpt::containers::yaml& p)
{
	p["K"] = 2.0;
	p["N"] = 2;
}

TEST(yaml, assignmentsInCallee)
{
	mrpt::containers::yaml p;

	auto& pp = p["params"] = mrpt::containers::yaml::Map();
	foo(pp);

	EXPECT_FALSE(p.empty());
	EXPECT_TRUE(p["params"].isMap());

	EXPECT_TRUE(p["params"].has("K"));
	EXPECT_TRUE(p["params"].has("N"));

	EXPECT_TRUE(p["params"]["K"].isScalar());
	EXPECT_EQ(p["params"]["K"].scalarType(), typeid(double));
	EXPECT_EQ(p["params"]["K"].as<double>(), 2.0);
	EXPECT_EQ(p["params"]["N"].as<int>(), 2);
}

// clang-format off
const auto sampleYamlBlock = std::string(R"xxx(
# blah blah
mySeq:
  - "first"
  - "second"
  - "third"
  - ~
myMap:
  K: 10.0
  P: -5.0
  Q: ~
  nestedMap:
    a: 1
    b: 2
    c: 3
)xxx");
// clang-format on

#if MRPT_HAS_YAMLCPP
TEST(yaml, fromYAML)
{
	auto p = mrpt::containers::yaml::FromYAMLText(sampleYamlBlock);
	EXPECT_EQ(p["mySeq"](0).as<std::string>(), "first");
	EXPECT_EQ(p["myMap"]["P"].as<double>(), -5.0);
	EXPECT_EQ(p["myMap"]["K"].as<double>(), 10.0);

	EXPECT_TRUE(p["mySeq"](3).isScalar());
	EXPECT_TRUE(p["myMap"]["Q"].isScalar());

	EXPECT_FALSE(p.isNullNode());
	EXPECT_FALSE(p["myMap"].isNullNode());

// Avoid a bug in yamlcpp < 0.6.2
// see: https://github.com/jbeder/yaml-cpp/issues/590
#if MRPT_YAMLCPP_VERSION >= 0x062
	EXPECT_TRUE(p["mySeq"](3).isNullNode());
	EXPECT_TRUE(p["myMap"]["Q"].isNullNode());
	EXPECT_FALSE(p["myMap"]["K"].isNullNode());
#endif
}
#endif
