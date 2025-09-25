#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include <FFF/fff.h>
DEFINE_FFF_GLOBALS;

// Ensure command errors are turned on.
#define BRICLI_SHOW_COMMAND_ERRORS 1
#define BRICLI_SHOW_HELP_ON_ERROR 1
#define BRICLI_USE_REENTRANT 0

#include "bricli.h"

// Setup fake functions
FAKE_VALUE_FUNC(int, BspWrite, uint32_t, const char*);
FAKE_VALUE_FUNC(int, Test_Handler, uint32_t, char **);
FAKE_VALUE_FUNC(int, Argument_Handler, uint32_t, char **);
FAKE_VOID_FUNC(Test_StateChanged, BriCLIStates_t, BriCLIStates_t);

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
        BriCLICommand_t _commandList[2] =
        {
            {"test", Test_Handler, "Tests."},
            {"args", Argument_Handler, "Test Arguments"}
        };
        BriCLIHandle_t _cli = BRICLI_HANDLE_DEFAULT;
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

            // Pre-load return values for the fakes.
            BspWrite_fake.return_val = (int)BriCLIOk;
            Test_Handler_fake.return_val = (int)BriCLIOk;
            Argument_Handler_fake.return_val = (int)BriCLIOk;

            // Configure our default BriCLI settings.
            _cli.CommandList = _commandList;
            _cli.CommandListLength = BRICLI_STATIC_ARRAY_SIZE(_commandList);
            _cli.RxBuffer = _buffer;
            _cli.RxBufferSize = 100;
            _cli.BspWrite = BspWrite;
            _cli.OnStateChanged = Test_StateChanged;
        }

        virtual void TearDown() 
        {
            BriCLI_ClearBuffer(&_cli);
        }
    };

    TEST_F(HandlerTest, ReceiveHandler)
    {
        std::string testCommand("test\n");
        BriCLIErrors_t error = BriCLIUnknown;

        // Receive the command.
        error = BriCLI_ReceiveArray(&_cli, testCommand.length(), (char *)testCommand.c_str());
        EXPECT_EQ(error, BriCLIOk);

        // Parse it, expecting the test handler to be called.
        error = (BriCLIErrors_t)BriCLI_Parse(&_cli);
        EXPECT_EQ(error, BriCLIOk);
        EXPECT_EQ(Test_Handler_fake.call_count, 1);
        EXPECT_EQ(_cli.State, BriCLIIdle);

        // Make sure our states were correctly changed as a result of the Parse call.
        EXPECT_EQ(Test_StateChanged_fake.call_count, 4);

        // Idle -> Parsing
        EXPECT_EQ(Test_StateChanged_fake.arg0_history[0], BriCLIIdle);
        EXPECT_EQ(Test_StateChanged_fake.arg1_history[0], BriCLIParsing);

        // Parsing -> Handler
        EXPECT_EQ(Test_StateChanged_fake.arg0_history[1], BriCLIParsing);
        EXPECT_EQ(Test_StateChanged_fake.arg1_history[1], BriCLIHandlerRunning);

        // Handler -> Finished
        EXPECT_EQ(Test_StateChanged_fake.arg0_history[2], BriCLIHandlerRunning);
        EXPECT_EQ(Test_StateChanged_fake.arg1_history[2], BriCLIFinished);

        // Finished -> Idle
        EXPECT_EQ(Test_StateChanged_fake.arg0_history[3], BriCLIFinished);
        EXPECT_EQ(Test_StateChanged_fake.arg1_history[3], BriCLIIdle);

        // Receive a partial command and make sure the handler isn't called.
        BriCLI_ReceiveArray(&_cli, (testCommand.length() - 1), (char *)testCommand.c_str());
        error = (BriCLIErrors_t)BriCLI_Parse(&_cli);
        EXPECT_EQ(error, BriCLIOk);
        EXPECT_EQ(Test_Handler_fake.call_count, 1);
        EXPECT_EQ(Test_StateChanged_fake.call_count, 4);
    }

    TEST_F(HandlerTest, Arguments)
    {
        std::string testCommandTwoArgs("args \"Hello World\" 43\n");
        std::string testCommandOneArgs("args 43\n");
        std::string testCommandNoArgs("args\n");
        BriCLIErrors_t error = BriCLIUnknown;

        // Receive the command.
        error = BriCLI_ReceiveArray(&_cli, testCommandTwoArgs.length(), (char *)testCommandTwoArgs.c_str());
        EXPECT_EQ(error, BriCLIOk);

        // Parse and check the handler passed the arguments through ok.
        error = (BriCLIErrors_t)BriCLI_Parse(&_cli);
        EXPECT_EQ(Argument_Handler_fake.call_count, 1);
        EXPECT_EQ(2, Argument_Handler_fake.arg0_val);
        EXPECT_EQ(error, BriCLIOk);

        // Repeat with 1 and 0 arguments.
        BriCLI_ReceiveArray(&_cli, testCommandOneArgs.length(), (char *)testCommandOneArgs.c_str());
        BriCLI_Parse(&_cli);
        EXPECT_EQ(Argument_Handler_fake.call_count, 2);
        EXPECT_EQ(1, Argument_Handler_fake.arg0_val);

        BriCLI_ReceiveArray(&_cli, testCommandNoArgs.length(), (char *)testCommandNoArgs.c_str());
        BriCLI_Parse(&_cli);
        EXPECT_EQ(Argument_Handler_fake.call_count, 3);
        EXPECT_EQ(0, Argument_Handler_fake.arg0_val);
    }

    TEST_F(HandlerTest, MultipleCommands)
    {
        std::string twoCommands("test\ntest\n");
        std::string threeCommands("test\nargs 124\ntest");
        std::string twoArgsCommands("args 1 \"Hello\"\nargs 2 \"World\"\n");
        BriCLIErrors_t error = BriCLIUnknown;

        // Receive the command.
        error = BriCLI_ReceiveArray(&_cli, twoCommands.length(), (char *)twoCommands.c_str());
        EXPECT_EQ(error, BriCLIOk);

        // Parse and check both commands were handled.
        error = (BriCLIErrors_t)BriCLI_Parse(&_cli);
        EXPECT_EQ(Test_Handler_fake.call_count, 2);
        EXPECT_EQ(_cli.PendingBytes, 0);
        EXPECT_EQ(error, BriCLIOk);

        // Receive the triple command
        error = BriCLI_ReceiveArray(&_cli, threeCommands.length(), (char *)threeCommands.c_str());
        EXPECT_EQ(error, BriCLIOk);

        error = (BriCLIErrors_t)BriCLI_Parse(&_cli);
        EXPECT_EQ(Test_Handler_fake.call_count, 4);
        EXPECT_EQ(Argument_Handler_fake.call_count, 1);
        EXPECT_EQ(_cli.PendingBytes, 0);
        EXPECT_EQ(error, BriCLIOk);

        // Receive the dual arguments command
        error = BriCLI_ReceiveArray(&_cli, twoArgsCommands.length(), (char *)twoArgsCommands.c_str());
        EXPECT_EQ(error, BriCLIOk);

        error = (BriCLIErrors_t)BriCLI_Parse(&_cli);
        EXPECT_EQ(Argument_Handler_fake.call_count, 3);
        EXPECT_EQ(_cli.PendingBytes, 0);
        EXPECT_EQ(error, BriCLIOk);
    }

    TEST_F(HandlerTest, CommandNotFound)
    {
        std::string testCommand("abcdef\n");
        BriCLIErrors_t error = BriCLIUnknown;

        // Receive the command.
        BriCLI_ReceiveArray(&_cli, testCommand.length(), (char *)testCommand.c_str());
        error = (BriCLIErrors_t)BriCLI_Parse(&_cli);
        EXPECT_EQ(error, BriCLIBadCommand);

        // Should have 8 calls: Error, 4 help messages with 2 automatic eols, Prompt
        EXPECT_EQ(BspWrite_fake.call_count, 8);
        // Make sure our system commands are called.
        EXPECT_STREQ(BspWrite_fake.arg1_history[1], "help - Displays this help message");
        EXPECT_STREQ(BspWrite_fake.arg1_history[3], "clear - Clears the terminal");
    }

    TEST_F(HandlerTest, ErrorPropagation)
    {
        std::string errorCommand("test\n");
        int testErrorCode = -3;
        std::string errorString("Command returned error: -3\n");
        BriCLIErrors_t error = BriCLIUnknown;

        // Receive the command.
        error = BriCLI_ReceiveArray(&_cli, errorCommand.length(), (char *)errorCommand.c_str());
        EXPECT_EQ(error, BriCLIOk);

        // Configure the fake to return an error.
        Test_Handler_fake.return_val = testErrorCode;

        // Test that the handler is called and the error propagated.
        error = (BriCLIErrors_t)BriCLI_Parse(&_cli);
        EXPECT_EQ(Test_Handler_fake.call_count, 1);
        EXPECT_EQ((int)error, testErrorCode);
        EXPECT_EQ(_cli.LastError, BriCLIErrorCommand);
        EXPECT_EQ(BspWrite_fake.call_count, 4);
    }
}