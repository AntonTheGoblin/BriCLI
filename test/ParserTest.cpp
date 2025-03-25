// #include <gtest/gtest.h>
#include <catch2/catch_test_macros.hpp>
#include <FFF/fff.h>
DEFINE_FFF_GLOBALS;

// Unit under test.
#include <bricli/bricli.h>

SCENARIO( "Bricli can be initialised", "[init]" )
{
	GIVEN("A default setup")
	{
		Bricli_t testCli;
		BricliError_t result;
		BricliInit_t init;
		init.Eol = "\r\n";
		result = Bricli_Init(&testCli, &init);

		THEN( "The intialise completes successfully" )
		{
			REQUIRE( strcmp((const char *)testCli.Eol, "\r\n") == 0 );
			REQUIRE( result == BricliErrorOk );
		}

		WHEN ("The EOL parameter is NULL")
		{
			init.Eol = NULL;
			result = Bricli_Init(&testCli, &init);

			THEN("The initialise fails")
			{
				REQUIRE(result == BricliErrorNullArgument);
			}
		}

		WHEN ("The EOL parameter is empty")
		{
			init.Eol = "";
			result = Bricli_Init(&testCli, &init);

			THEN("The initialise fails")
			{
				REQUIRE(result == BricliErrorInavlidArgument);
			}
		}

		WHEN ("The EOL parameter is too long")
		{
			uint8_t eol[BRICLI_EOL_SIZE + 1] = {0};
			memset(eol, 'A', BRICLI_EOL_SIZE);
			init.Eol = (const char*)eol;
			result = Bricli_Init(&testCli, &init);

			THEN("The initialise fails")
			{
				REQUIRE(result == BricliErrorInavlidArgument);
			}
		}
	}
}
