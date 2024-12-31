#include <gtest/gtest.h>
#include <FFF/fff.h>
DEFINE_FFF_GLOBALS;

// Unit under test.
#include "bricli/bricli.h"

namespace UnitTests
{
	class ParserTest : public ::testing::Test
	{
	protected:

		ParserTest() { }

		virtual ~ParserTest() { }

		virtual void SetUp() override
		{
			
		}

		virtual void TearDown() override { }

	};

	TEST_F(ParserTest, TestName)
	{
		FAIL();
	}

}