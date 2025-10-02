#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include <FFF/fff.h>
DEFINE_FFF_GLOBALS;

#include <bricli/bricli.h>

// Setup fake functions
FAKE_VALUE_FUNC(int, BspWrite, uint32_t, const char*);
FAKE_VALUE_FUNC(int, Test_Handler, uint32_t, char **);

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

    class SendTest: public ::testing::Test
    {
    protected:
        BricliCommand_t _commandList[2] =
        {
            { "test", Test_Handler, "Tests.", BricliScopeAll },
			BRICLI_COMMAND_LIST_TERMINATOR
        };
        BricliHandle_t _cli;
        char _buffer[100] = {0};

        SendTest() { }
        virtual ~SendTest() { }

        virtual void SetUp()
        {
            // Reset fake functions.
            RESET_FAKE(BspWrite);
            RESET_FAKE(Test_Handler);
            FFF_RESET_HISTORY();

			// Reset string history
			memset(&_stringHistory[0][0], 0, (50 * 80));
			_stringHistoryCount = 0;

            // Pre-load return values for the fakes.
            BspWrite_fake.return_val = (int)BricliOk;
			BspWrite_fake.custom_fake = CustomBspWriteFake;
			Test_Handler_fake.return_val = (int)BricliOk;

            // Configure our default BriCLI settings.
            BricliInit_t init = {0};
            init.CommandList = _commandList;
            init.RxBuffer = _buffer;
            init.RxBufferSize = 100;
            init.BspWrite = BspWrite;

			Bricli_Init(&_cli, &init);
        }

        virtual void TearDown() 
        {
            Bricli_ClearBuffer(&_cli);
        }
    };

    TEST_F(SendTest, Write)
    {
        std::string testCommand("Some Response Data");

        // Test each of the write functions.
        Bricli_Write(&_cli, testCommand.length(), (char *)testCommand.c_str());
        EXPECT_EQ(testCommand.length(), BspWrite_fake.arg0_history[0]);
        EXPECT_STREQ(testCommand.c_str(), BspWrite_fake.arg1_history[0]);
        
        Bricli_WriteColoured(&_cli, testCommand.length(), (char *)testCommand.c_str(), BricliTextRed);
        EXPECT_EQ(testCommand.length(), BspWrite_fake.arg0_history[2]);
        EXPECT_STREQ(testCommand.c_str(), BspWrite_fake.arg1_history[2]);
        
        Bricli_WriteString(&_cli, (char *)testCommand.c_str());
        EXPECT_EQ(testCommand.length(), BspWrite_fake.arg0_history[4]);
        EXPECT_STREQ(testCommand.c_str(), BspWrite_fake.arg1_history[4]);
        
        Bricli_WriteStringColoured(&_cli, (char *)testCommand.c_str(), BricliTextRed);
        EXPECT_EQ(testCommand.length(), BspWrite_fake.arg0_history[6]);
        EXPECT_STREQ(testCommand.c_str(), BspWrite_fake.arg1_history[6]);

        // PrintF's uses an internal temporary buffer so we can't track the actual command output.
        Bricli_PrintF(&_cli, "%s", testCommand.c_str());
        EXPECT_EQ(testCommand.length(), BspWrite_fake.arg0_history[8]);

        BRICLI_PRINTF_COLOURED(&_cli, BricliTextYellow, "%s", testCommand.c_str());
        EXPECT_EQ(testCommand.length(), BspWrite_fake.arg0_history[10]);

        // Make sure total calls match.
        EXPECT_EQ(BspWrite_fake.call_count, 12);
    }

    TEST_F(SendTest, WriteLine)
    {
        std::string testCommand("Some Response Data");

        // Test each of the write functions.

        // 0: command, 1: eol
        Bricli_WriteLine(&_cli, testCommand.length(), (char *)testCommand.c_str());
        EXPECT_EQ(testCommand.length(), BspWrite_fake.arg0_history[0]);
        EXPECT_STREQ(testCommand.c_str(), BspWrite_fake.arg1_history[0]);
        EXPECT_STREQ(_cli.Eol, BspWrite_fake.arg1_history[1]);
        
        // 2: colour, 3: command, 4: eol, 5: colour reset
        Bricli_WriteColouredLine(&_cli, testCommand.length(), (char *)testCommand.c_str(), BricliTextRed);
        EXPECT_EQ(testCommand.length(), BspWrite_fake.arg0_history[3]);
        EXPECT_STREQ(testCommand.c_str(), BspWrite_fake.arg1_history[3]);
        EXPECT_STREQ(_cli.Eol, BspWrite_fake.arg1_history[4]);

        // Change the Eol to make sure \r\n works
        _cli.Eol = (char *)"\r\n";

        // 6: command, 7: eol
        Bricli_WriteStringLine(&_cli, (char *)testCommand.c_str());
        EXPECT_EQ(testCommand.length(), BspWrite_fake.arg0_history[6]);
        EXPECT_STREQ(testCommand.c_str(), BspWrite_fake.arg1_history[6]);
        EXPECT_STREQ(_cli.Eol, BspWrite_fake.arg1_history[7]);
        
        // 8: colour, 9: command, 10: eol, 11: colour reset
        Bricli_WriteStringColouredLine(&_cli, (char *)testCommand.c_str(), BricliTextRed);
        EXPECT_EQ(testCommand.length(), BspWrite_fake.arg0_history[9]);
        EXPECT_STREQ(testCommand.c_str(), BspWrite_fake.arg1_history[9]);
        EXPECT_STREQ(_cli.Eol, BspWrite_fake.arg1_history[10]);

        // Make sure total calls match.
        EXPECT_EQ(BspWrite_fake.call_count, 12);

        for (size_t i = 0; i < BspWrite_fake.call_count; i++)
        {
            std::cout << "[" << i << "]: " << BspWrite_fake.arg1_history[i] << "\n";
        }
        
    }

    TEST_F(SendTest, Prompt)
    {
        Bricli_SendPrompt(&_cli);

        EXPECT_EQ(BspWrite_fake.call_count, 1);
        EXPECT_EQ(BspWrite_fake.arg0_val, strlen(_cli.Prompt));
        EXPECT_STREQ(BspWrite_fake.arg1_val, _cli.Prompt);
    }

    TEST_F(SendTest, Help)
    {
        uint32_t NumberOfCommands = (4 + 1); // 4 system command calls with automatic Eols plus however many custom commands.

        // Print the help message and make sure BspWrite is called.
        EXPECT_EQ(0, Bricli_PrintHelp(&_cli));
        EXPECT_EQ(_stringHistoryCount, NumberOfCommands);

        // Make sure our system commands are called.
        EXPECT_STREQ(&_stringHistory[0][0], "help - Displays this help message\n");
        EXPECT_STREQ(&_stringHistory[1][0], "clear - Clears the terminal\n");
        EXPECT_STREQ(&_stringHistory[2][0], "login - Login to the terminal\n");
    }

    TEST_F(SendTest, SendEol)
    {
        std::string testCommand("Hello World");
        
        // Test EoL passthrough
        _cli.SendEol = NULL;
        Bricli_WriteStringLine(&_cli, testCommand.c_str());

        // Expect 2 calls, one for the command and one for the EoL.
        EXPECT_EQ(BspWrite_fake.call_count, 2);
        EXPECT_STREQ(BspWrite_fake.arg1_history[0], "Hello World");
        EXPECT_STREQ(BspWrite_fake.arg1_history[1], _cli.Eol);

        // Test SendEol
        _cli.SendEol = (char*)"\r";
        Bricli_WriteStringLine(&_cli, testCommand.c_str());
        EXPECT_EQ(BspWrite_fake.call_count, 4);
        EXPECT_STREQ(BspWrite_fake.arg1_history[2], "Hello World");
        EXPECT_STREQ(BspWrite_fake.arg1_history[3], _cli.SendEol);
    }
}