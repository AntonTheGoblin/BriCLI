#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include <FFF/fff.h>
DEFINE_FFF_GLOBALS;

// Ensure command errors are turned on.
#define BRICLI_SHOW_COMMAND_ERRORS 1
#define BRICLI_SHOW_HELP_ON_ERROR 1
#define BRICLI_USE_REENTRANT 0

#include <bricli/bricli.h>

// Setup fake functions
FAKE_VALUE_FUNC(int, BspWrite, uint32_t, const char*);
FAKE_VALUE_FUNC(int, Test_Handler, uint32_t, char **);
FAKE_VALUE_FUNC(int, Argument_Handler, uint32_t, char **);
FAKE_VOID_FUNC(Test_StateChanged, BricliStates_t, BricliStates_t);

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

    // Test function used for debugging handler passing.
    int ArgumentTest_Handler(uint32_t numberOfArgs, char **args)
    {
        for (uint32_t i = 0; i < numberOfArgs; i++)
        {
            EXPECT_TRUE(NULL != args[i]);
            std::cout << "Argument [" << i << "] " << args[i] << std::endl;
        }
        return 0;
    }

    class HandlerTest: public ::testing::Test
    {
    protected:
        BricliCommand_t _commandList[3] =
        {
            {"test", Test_Handler, "Tests", BricliScopeAll},
            {"args", Argument_Handler, "Test Arguments", BricliScopeAll},
			BRICLI_COMMAND_LIST_TERMINATOR
        };
        BricliHandle_t _cli;
        char _buffer[100] = {0};

        HandlerTest() { }
        virtual ~HandlerTest() { }

        virtual void SetUp()
        {
            // Reset fake functions.
            RESET_FAKE(BspWrite);
            RESET_FAKE(Test_Handler);
            RESET_FAKE(Argument_Handler);
            RESET_FAKE(Test_StateChanged);

			// Reset string history
			memset(&_stringHistory[0][0], 0, (50 * 80));
			_stringHistoryCount = 0;

			// Pre-load return values for the fakes.
            BspWrite_fake.return_val = (int)BricliOk;
			BspWrite_fake.custom_fake = CustomBspWriteFake;
            Test_Handler_fake.return_val = (int)BricliOk;
            Argument_Handler_fake.return_val = (int)BricliOk;

            // Configure our default BriCLI settings.
			BricliInit_t init = {0};
			init.CommandList = _commandList;
            init.RxBuffer = _buffer;
            init.RxBufferSize = 100;
            init.BspWrite = BspWrite;
            init.OnStateChanged = Test_StateChanged;

			Bricli_Init(&_cli, &init);
		}

        virtual void TearDown() 
        {
            Bricli_ClearBuffer(&_cli);
        }
    };

	TEST_F(HandlerTest, InitErrors)
	{
		BricliHandle_t initCli;
		BricliInit_t initSettings = {0};

		// NULL pointers
		EXPECT_EQ(BricliBadParameter, Bricli_Init(NULL, &initSettings));
		EXPECT_EQ(BricliBadParameter, Bricli_Init(&initCli, NULL));

		// Bad RX Buffer
		EXPECT_EQ(BricliBadParameter, Bricli_Init(&initCli, &initSettings));
		initSettings.RxBuffer = _buffer;
		EXPECT_EQ(BricliBadParameter, Bricli_Init(&initCli, &initSettings));
		initSettings.RxBufferSize = 100;

		// Bad Command List
		EXPECT_EQ(BricliBadParameter, Bricli_Init(&initCli, &initSettings));
		initSettings.CommandList = _commandList;
		
		// Bad BspWrite
		EXPECT_EQ(BricliBadParameter, Bricli_Init(&initCli, &initSettings));
		initSettings.BspWrite = BspWrite;

		// Valid setup
		EXPECT_EQ(BricliOk, Bricli_Init(&initCli, &initSettings));
	}

	TEST_F(HandlerTest, InitDefaults)
	{
		BricliHandle_t initCli;
		BricliInit_t initSettings = {0};
		
		initSettings.RxBuffer = _buffer;
		initSettings.RxBufferSize = 100;
		initSettings.CommandList = _commandList;
		initSettings.BspWrite = BspWrite;

		// Valid setup
		EXPECT_EQ(BricliOk, Bricli_Init(&initCli, &initSettings));
		EXPECT_STREQ(BRICLI_DEFAULT_EOL, initCli.Eol);
		EXPECT_STREQ(BRICLI_DEFAULT_PROMPT, initCli.Prompt);
	}

	TEST_F(HandlerTest, InitCustom)
	{
		BricliHandle_t initCli;
		BricliInit_t initSettings = {0};
		
		initSettings.RxBuffer = _buffer;
		initSettings.RxBufferSize = 100;
		initSettings.CommandList = _commandList;
		initSettings.BspWrite = BspWrite;

		initSettings.Eol = (char *)"\r\n";
		initSettings.Prompt = (char *)"$ ";
		initSettings.OnStateChanged = (Bricli_StateChanged)0x02;

		// Valid setup
		EXPECT_EQ(BricliOk, Bricli_Init(&initCli, &initSettings));
		EXPECT_STREQ("\r\n", initCli.Eol);
		EXPECT_STREQ("$ ", initCli.Prompt);
		EXPECT_EQ((Bricli_StateChanged)0x02, initCli.OnStateChanged);
	}

    TEST_F(HandlerTest, ReceiveHandler)
    {
        std::string testCommand("test\n");
        BricliErrors_t error = BricliUnknown;

        // Receive the command.
        error = Bricli_ReceiveArray(&_cli, testCommand.length(), (char *)testCommand.c_str());
        EXPECT_EQ(error, BricliOk);

        // Parse it, expecting the test handler to be called.
        error = (BricliErrors_t)Bricli_Parse(&_cli);
        EXPECT_EQ(error, BricliOk);
        EXPECT_EQ(Test_Handler_fake.call_count, 1);
        EXPECT_EQ(_cli.State, BricliStateIdle);

        // Make sure our states were correctly changed as a result of the Parse call.
        EXPECT_EQ(Test_StateChanged_fake.call_count, 4);

        // Idle -> Parsing
        EXPECT_EQ(Test_StateChanged_fake.arg0_history[0], BricliStateIdle);
        EXPECT_EQ(Test_StateChanged_fake.arg1_history[0], BricliStateParsing);

        // Parsing -> Handler
        EXPECT_EQ(Test_StateChanged_fake.arg0_history[1], BricliStateParsing);
        EXPECT_EQ(Test_StateChanged_fake.arg1_history[1], BricliStateHandlerRunning);

        // Handler -> Finished
        EXPECT_EQ(Test_StateChanged_fake.arg0_history[2], BricliStateHandlerRunning);
        EXPECT_EQ(Test_StateChanged_fake.arg1_history[2], BricliStateFinished);

        // Finished -> Idle
        EXPECT_EQ(Test_StateChanged_fake.arg0_history[3], BricliStateFinished);
        EXPECT_EQ(Test_StateChanged_fake.arg1_history[3], BricliStateIdle);

        // Receive a partial command and make sure the handler isn't called.
        Bricli_ReceiveArray(&_cli, (testCommand.length() - 1), (char *)testCommand.c_str());
        error = (BricliErrors_t)Bricli_Parse(&_cli);
        EXPECT_EQ(error, BricliOk);
        EXPECT_EQ(Test_Handler_fake.call_count, 1);
        EXPECT_EQ(Test_StateChanged_fake.call_count, 4);
    }

    TEST_F(HandlerTest, Arguments)
    {
        std::string testCommandTwoArgs("args \"Hello World\" 43\n");
        std::string testCommandOneArgs("args 52\n");
        std::string testCommandNoArgs("args\n");
        std::string testCommandSingleStringArgs("args \"Word\"\n");
        BricliErrors_t error = BricliUnknown;

        // Receive the command.
        error = Bricli_ReceiveArray(&_cli, testCommandTwoArgs.length(), (char *)testCommandTwoArgs.c_str());
        EXPECT_EQ(error, BricliOk);

        // Parse and check the handler passed the arguments through ok.
        error = (BricliErrors_t)Bricli_Parse(&_cli);
        EXPECT_EQ(Argument_Handler_fake.call_count, 1);
        EXPECT_EQ(2, Argument_Handler_fake.arg0_val);
        EXPECT_EQ(error, BricliOk);

        // Repeat with 1 and 0 arguments.
        Bricli_ReceiveArray(&_cli, testCommandOneArgs.length(), (char *)testCommandOneArgs.c_str());
        Bricli_Parse(&_cli);
        EXPECT_EQ(Argument_Handler_fake.call_count, 2);
        EXPECT_EQ(1, Argument_Handler_fake.arg0_val);

        Bricli_ReceiveArray(&_cli, testCommandNoArgs.length(), (char *)testCommandNoArgs.c_str());
        Bricli_Parse(&_cli);
        EXPECT_EQ(Argument_Handler_fake.call_count, 3);
        EXPECT_EQ(0, Argument_Handler_fake.arg0_val);

		// Repeat with single word string
		Bricli_ReceiveArray(&_cli, testCommandSingleStringArgs.length(), (char *)testCommandSingleStringArgs.c_str());
        Bricli_Parse(&_cli);
        EXPECT_EQ(Argument_Handler_fake.call_count, 4);
        EXPECT_EQ(1, Argument_Handler_fake.arg0_val);
    }

	TEST_F(HandlerTest, EscapedArguments)
	{
		std::string testCommand("args \"This is a \\\"substring\\\"\"\n");
		BricliErrors_t error = BricliUnknown;

		// Receive the command.
        error = Bricli_ReceiveArray(&_cli, testCommand.length(), (char *)testCommand.c_str());
        EXPECT_EQ(error, BricliOk);

		// Parse and check the handler passed the arguments through ok.
        error = (BricliErrors_t)Bricli_Parse(&_cli);
        EXPECT_EQ(Argument_Handler_fake.call_count, 1);
        EXPECT_EQ(1, Argument_Handler_fake.arg0_val);
        EXPECT_EQ(error, BricliOk);

		// Check the response
		// EXPECT_STREQ("This is a \"substring\"", Argument_Handler_fake.arg1_val[0]);
	}

    TEST_F(HandlerTest, MultipleCommands)
	{
        std::string twoCommands("test\ntest\n");
        std::string threeCommands("test\nargs 124\ntest");
        std::string twoArgsCommands("args 1 \"Hello\"\nargs 2 \"World\"\n");
        BricliErrors_t error = BricliUnknown;

        // Receive the command.
        printf("Test 1\n");
		error = Bricli_ReceiveArray(&_cli, twoCommands.length(), (char *)twoCommands.c_str());
        EXPECT_EQ(error, BricliOk);

		// Parse and check both commands were handled.
		error = (BricliErrors_t)Bricli_Parse(&_cli);
        EXPECT_EQ(Test_Handler_fake.call_count, 2);
        EXPECT_EQ(_cli.PendingBytes, 0);
        EXPECT_EQ(error, BricliOk);
		printf("DONE\n");

        // Receive the triple command
		printf("Test 2\n");
        error = Bricli_ReceiveArray(&_cli, threeCommands.length(), (char *)threeCommands.c_str());
        EXPECT_EQ(error, BricliOk);

        error = (BricliErrors_t)Bricli_Parse(&_cli);
        EXPECT_EQ(Test_Handler_fake.call_count, 4);
        EXPECT_EQ(Argument_Handler_fake.call_count, 1);
        EXPECT_EQ(_cli.PendingBytes, 0);
        EXPECT_EQ(error, BricliOk);
		printf("DONE\n");

        // Receive the dual arguments command
		printf("Test 3\n");
        error = Bricli_ReceiveArray(&_cli, twoArgsCommands.length(), (char *)twoArgsCommands.c_str());
        EXPECT_EQ(error, BricliOk);

        error = (BricliErrors_t)Bricli_Parse(&_cli);
        EXPECT_EQ(Argument_Handler_fake.call_count, 3);
        EXPECT_EQ(_cli.PendingBytes, 0);
        EXPECT_EQ(error, BricliOk);
		printf("DONE\n");
    }

    TEST_F(HandlerTest, CommandNotFound)
    {
        std::string testCommand("abcdef\n");
        BricliErrors_t error = BricliUnknown;

        // Receive the command.
        Bricli_ReceiveArray(&_cli, testCommand.length(), (char *)testCommand.c_str());
        error = (BricliErrors_t)Bricli_Parse(&_cli);
        EXPECT_EQ(error, BricliBadCommand);

        // Should have 10 calls: 1 unknown command, 4 system commands, 2 user commands, 1 prompt
        EXPECT_EQ(_stringHistoryCount, 8);

        // Make sure the output is valid.
        EXPECT_STREQ(&_stringHistory[0][0], "Unknown Command abcdef\n");
        EXPECT_STREQ(&_stringHistory[1][0], "help - Displays this help message\n");
        EXPECT_STREQ(&_stringHistory[2][0], "clear - Clears the terminal\n");
        EXPECT_STREQ(&_stringHistory[3][0], "login - Login to the terminal\n");
        EXPECT_STREQ(&_stringHistory[4][0], "logout - Logout from the terminal\n");
    }

    TEST_F(HandlerTest, ErrorPropagation)
    {
        std::string errorCommand("test\n");
        int testErrorCode = -3;
        std::string errorString("Command returned error: -3\n");
        BricliErrors_t error = BricliUnknown;

        // Receive the command.
        error = Bricli_ReceiveArray(&_cli, errorCommand.length(), (char *)errorCommand.c_str());
        EXPECT_EQ(error, BricliOk);

        // Configure the fake to return an error.
        Test_Handler_fake.return_val = testErrorCode;

        // Test that the handler is called and the error propagated.
        error = (BricliErrors_t)Bricli_Parse(&_cli);
        EXPECT_EQ(Test_Handler_fake.call_count, 1);
        EXPECT_EQ((int)error, testErrorCode);
        EXPECT_EQ(_cli.LastError, BricliErrorCommand);
        EXPECT_EQ(BspWrite_fake.call_count, 4);
    }
}