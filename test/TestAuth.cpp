#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include <FFF/fff.h>
DEFINE_FFF_GLOBALS;

#include <bricli/bricli.h>

// Setup fake functions
FAKE_VALUE_FUNC(int, BspWrite, uint32_t, const char*);
FAKE_VALUE_FUNC(int, NoAuth_Handler, uint32_t, char **);
FAKE_VALUE_FUNC(int, User_Handler, uint32_t, char **);
FAKE_VALUE_FUNC(int, Admin_Handler, uint32_t, char **);
FAKE_VALUE_FUNC(int, Custom_Handler, uint32_t, char **);

#define CUSTOM_AUTH_SCOPE (1 << 16)

static char _stringHistory[50][80] = {0};
static uint8_t _stringHistoryCount = 0;

int CustomBspWriteFake(uint32_t length, const char* data)
{
	printf("%s", data);

	memset(&_stringHistory[_stringHistoryCount][0], 0, 80);
	memcpy(&_stringHistory[_stringHistoryCount][0], data, length);
	_stringHistoryCount++;

	return BspWrite_fake.return_val;
}

namespace Cli {

    class AuthTest: public ::testing::Test
    {
    protected:
        BricliCommand_t _commandList[5] =
        {
            {"anonymous", NoAuth_Handler, "No auth", BricliScopeAll},
            {"user", User_Handler, "User level command", BricliScopeUser},
            {"admin", Admin_Handler, "admin level command", BricliScopeAdmin },
            {"custom", Custom_Handler, "custom level command", CUSTOM_AUTH_SCOPE },
			BRICLI_COMMAND_LIST_TERMINATOR
        };
		BricliAuthEntry_t _authList[3] =
		{
			{"local", "pass", BricliScopeUser},
			{"admin", "admin", (BricliScopeAdmin | BricliScopeUser)},
			{"custom", "abcdef", CUSTOM_AUTH_SCOPE},
		};
		BricliHandle_t _cli;
        char _buffer[100] = {0};

        AuthTest() { }
        virtual ~AuthTest() { }

        virtual void SetUp()
        {
			// Reset fake functions.
            RESET_FAKE(BspWrite);
            RESET_FAKE(User_Handler);
            RESET_FAKE(Admin_Handler);
            RESET_FAKE(Custom_Handler);

			// Reset string history
			memset(&_stringHistory[0][0], 0, (50 * 80));
			_stringHistoryCount = 0;
			
            // Pre-load return values for the fakes.
            BspWrite_fake.return_val = (int)BricliOk;
			BspWrite_fake.custom_fake = CustomBspWriteFake;
            User_Handler_fake.return_val = (int)BricliOk;
			Admin_Handler_fake.return_val = (int)BricliOk;
			Custom_Handler_fake.return_val = (int)BricliOk;
			
            // Configure our default BriCLI settings.
			BricliInit_t init = {0};
			init.AuthList = _authList;
			init.CommandList = _commandList;
            init.RxBuffer = _buffer;
            init.RxBufferSize = 100;
            init.BspWrite = BspWrite;
            init.LocalEcho = true;

			Bricli_Init(&_cli, &init);
		}

        virtual void TearDown() 
        {
            Bricli_ClearBuffer(&_cli);
        }
    };

    TEST_F(AuthTest, AnonymousScope)
    {
		std::string testCommand("anonymous\n");
        BricliErrors_t error = BricliUnknown;

        // Receive the command.
        error = Bricli_ReceiveArray(&_cli, testCommand.length(), (char *)testCommand.c_str());
		EXPECT_EQ(error, BricliOk);

		 // Parse it, expecting the anonymous handler to be called.
        error = (BricliErrors_t)Bricli_Parse(&_cli);
        EXPECT_EQ(error, BricliOk);
		EXPECT_EQ(NoAuth_Handler_fake.call_count, 1);

		// Attempt to call the user command
		testCommand = "user\n";
		Bricli_ReceiveArray(&_cli, testCommand.length(), (char *)testCommand.c_str());
		error = (BricliErrors_t)Bricli_Parse(&_cli);
		EXPECT_EQ(error, BricliUnauthorized);
		EXPECT_EQ(Admin_Handler_fake.call_count, 0);
	}
	
	TEST_F(AuthTest, UserScope)
    {
		std::string testCommand("login local pass\n");
        BricliErrors_t error = BricliUnknown;

		// Login at user level
		Bricli_ReceiveArray(&_cli, testCommand.length(), (char *)testCommand.c_str());
		error = (BricliErrors_t)Bricli_Parse(&_cli);
        EXPECT_EQ(error, BricliOk);

		// Call the user command
		testCommand = "user\n";
		Bricli_ReceiveArray(&_cli, testCommand.length(), (char *)testCommand.c_str());
		Bricli_Parse(&_cli);
		EXPECT_EQ(User_Handler_fake.call_count, 1);

		// Attempt to call admin command
		testCommand = "admin\n";
		Bricli_ReceiveArray(&_cli, testCommand.length(), (char *)testCommand.c_str());
		error = (BricliErrors_t)Bricli_Parse(&_cli);
		EXPECT_EQ(error, BricliUnauthorized);
		EXPECT_EQ(Admin_Handler_fake.call_count, 0);

		// Attempt to call custom command
		testCommand = "custom\n";
		Bricli_ReceiveArray(&_cli, testCommand.length(), (char *)testCommand.c_str());
		error = (BricliErrors_t)Bricli_Parse(&_cli);
		EXPECT_EQ(error, BricliUnauthorized);
		EXPECT_EQ(Custom_Handler_fake.call_count, 0);
	}

	TEST_F(AuthTest, AdminScope)
    {
		std::string testCommand("login admin admin\n");
        BricliErrors_t error = BricliUnknown;

        // Login at admin level
		Bricli_ReceiveArray(&_cli, testCommand.length(), (char *)testCommand.c_str());
		error = (BricliErrors_t)Bricli_Parse(&_cli);
        EXPECT_EQ(error, BricliOk);

		// Call the user command
		testCommand = "user\n";
		Bricli_ReceiveArray(&_cli, testCommand.length(), (char *)testCommand.c_str());
		Bricli_Parse(&_cli);
		EXPECT_EQ(User_Handler_fake.call_count, 1);

		// Call admin command
		testCommand = "admin\n";
		Bricli_ReceiveArray(&_cli, testCommand.length(), (char *)testCommand.c_str());
		Bricli_Parse(&_cli);
		EXPECT_EQ(Admin_Handler_fake.call_count, 1);

		// Attempt to call custom command
		testCommand = "custom\n";
		Bricli_ReceiveArray(&_cli, testCommand.length(), (char *)testCommand.c_str());
		error = (BricliErrors_t)Bricli_Parse(&_cli);
		EXPECT_EQ(error, BricliUnauthorized);
		EXPECT_EQ(Custom_Handler_fake.call_count, 0);
	}

	TEST_F(AuthTest, CustomScope)
    {
		std::string testCommand("login custom abcdef\n");
        BricliErrors_t error = BricliUnknown;

        // Login at custom level
		Bricli_ReceiveArray(&_cli, testCommand.length(), (char *)testCommand.c_str());
		error = (BricliErrors_t)Bricli_Parse(&_cli);
        EXPECT_EQ(error, BricliOk);

		// Call the custom command
		testCommand = "custom\n";
		Bricli_ReceiveArray(&_cli, testCommand.length(), (char *)testCommand.c_str());
		Bricli_Parse(&_cli);
		EXPECT_EQ(Custom_Handler_fake.call_count, 1);

		// Attempt to call user command
		testCommand = "user\n";
		Bricli_ReceiveArray(&_cli, testCommand.length(), (char *)testCommand.c_str());
		error = (BricliErrors_t)Bricli_Parse(&_cli);
		EXPECT_EQ(error, BricliUnauthorized);
		EXPECT_EQ(User_Handler_fake.call_count, 0);

		// Attempt to call admin command
		testCommand = "admin\n";
		Bricli_ReceiveArray(&_cli, testCommand.length(), (char *)testCommand.c_str());
		error = (BricliErrors_t)Bricli_Parse(&_cli);
		EXPECT_EQ(error, BricliUnauthorized);
		EXPECT_EQ(Admin_Handler_fake.call_count, 0);
	}

	TEST_F(AuthTest, Logout)
	{
		std::string testCommand("login local pass\n");
        BricliErrors_t error = BricliUnknown;

		// Login at user level
		Bricli_ReceiveArray(&_cli, testCommand.length(), (char *)testCommand.c_str());
		error = (BricliErrors_t)Bricli_Parse(&_cli);
        EXPECT_EQ(error, BricliOk);

		// Call the user command
		testCommand = "user\n";
		Bricli_ReceiveArray(&_cli, testCommand.length(), (char *)testCommand.c_str());
		Bricli_Parse(&_cli);
		EXPECT_EQ(User_Handler_fake.call_count, 1);

		// logout
		testCommand = "logout\n";
		Bricli_ReceiveArray(&_cli, testCommand.length(), (char *)testCommand.c_str());
		Bricli_Parse(&_cli);

		// Attempt to call user command
		testCommand = "user\n";
		Bricli_ReceiveArray(&_cli, testCommand.length(), (char *)testCommand.c_str());
		error = (BricliErrors_t)Bricli_Parse(&_cli);
		EXPECT_EQ(error, BricliUnauthorized);
		EXPECT_EQ(User_Handler_fake.call_count, 1);
	}
}