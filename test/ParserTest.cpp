// #include <gtest/gtest.h>
#include <catch2/catch_test_macros.hpp>
#include <FFF/fff.h>
DEFINE_FFF_GLOBALS;

// Unit under test.
#include <bricli/bricli.h>

extern "C"
{
	FAKE_VALUE_FUNC(uint32_t, BspWrite, uint32_t, const uint8_t*);
	static inline void ResetFakes(void)
	{
		RESET_FAKE(PingCommand);
		RESET_FAKE(BspWrite);
	}
}

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

SCENARIO("Bricli can parse simple lines", "[parser]")
{
	ResetFakes();
	
	GIVEN("A nominal setup")
	{
		Bricli_t testCli;
		BricliError_t result;
		BricliInit_t init;
		init.Eol = "\r\n";
		(void)Bricli_Init(&testCli, &init);

		WHEN( "A registered command is received" )
		{
			const char *testCommand = "ping\r\n";
			result = Bricli_Parse((uint8_t *)testCommand, strlen(testCommand));

			THEN( "The command handler is called" )
			{
				REQUIRE(PingCommand_fake.call_count > 0);
			}
		}
	}
}
