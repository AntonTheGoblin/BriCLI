// #include <gtest/gtest.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <FFF/fff.h>
DEFINE_FFF_GLOBALS;

// Unit under test.
#include <bricli/bricli.h>

using Catch::Matchers::Equals;

extern "C"
{
	FAKE_VALUE_FUNC(BricliError_t, PingCommand, uint32_t, uint8_t**);
	FAKE_VALUE_FUNC(uint32_t, MockBspWrite, uint32_t, const uint8_t*);
	
	static inline uint32_t MockBspWriteCustomFake(uint32_t length, const uint8_t* data)
	{
		(void)data;
		if (MockBspWrite_fake.return_val == UINT32_MAX)
		{
			return 0;
		}
		else if (MockBspWrite_fake.return_val == 0)
		{
			return length;
		}
		else
		{
			return MockBspWrite_fake.return_val;
		}
	}
	
	
	static inline void ResetFakes(void)
	{
		RESET_FAKE(PingCommand);
		RESET_FAKE(MockBspWrite);

		MockBspWrite_fake.custom_fake = MockBspWriteCustomFake;
	}

	
}

SCENARIO( "Bricli can be initialised", "[init]" )
{
	GIVEN("A default setup")
	{
		Bricli_t testCli;
		BricliError_t result;
		BricliInit_t init;
		init.RxEol = (const uint8_t*)"\r\n";
		result = Bricli_Init(&testCli, &init);

		THEN( "The intialise completes successfully" )
		{
			REQUIRE( strcmp((const char *)testCli.RxEol, "\r\n") == 0 );
			REQUIRE( result == BricliErrorOk );
		}

		WHEN ("The EOL parameter is NULL")
		{
			init.RxEol = NULL;
			result = Bricli_Init(&testCli, &init);

			THEN("The initialise fails")
			{
				REQUIRE(result == BricliErrorNullArgument);
			}
		}

		WHEN ("The EOL parameter is empty")
		{
			init.RxEol = (const uint8_t *)"";
			result = Bricli_Init(&testCli, &init);

			THEN("The initialise fails")
			{
				REQUIRE(result == BricliErrorBadParameter);
			}
		}

		WHEN ("The EOL parameter is too long")
		{
			uint8_t eol[BRICLI_EOL_SIZE + 1] = {0};
			memset(eol, 'A', BRICLI_EOL_SIZE);
			init.RxEol = eol;
			result = Bricli_Init(&testCli, &init);

			THEN("The initialise fails")
			{
				REQUIRE(result == BricliErrorBadParameter);
			}
		}
	}
}

SCENARIO("Bricli can write to the terminal", "[API]")
{
	const char *testData = "Hello World";
	Bricli_t testCli = {0};
	BricliInit_t init = {0};

	init.RxEol = (const uint8_t *)"\r\n";
	
	ResetFakes();

	GIVEN("A minimal setup")
	{
		init.BspWrite = MockBspWrite;
		(void)Bricli_Init(&testCli, &init);

		WHEN("A standard write is used")
		{
			BricliWrite(&testCli, strlen(testData), (const uint8_t *)testData);

			THEN("The write function should be called exactly")
			{
				REQUIRE(MockBspWrite_fake.call_count > 0);
				REQUIRE_THAT((const char*)MockBspWrite_fake.arg1_history[0], Equals(testData));
			}
		}
	}

	GIVEN( "An invalid setup" )
	{
		(void)Bricli_Init(&testCli, &init);

		WHEN("A standard write is used")
		{
			BricliWrite(&testCli, strlen(testData), (const uint8_t *)testData);

			THEN("The write function should not be called")
			{
				REQUIRE(MockBspWrite_fake.call_count == 0);
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
		init.RxEol = (const uint8_t *)"\r\n";
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
