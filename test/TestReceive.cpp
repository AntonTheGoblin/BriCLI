#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include <FFF/fff.h>
DEFINE_FFF_GLOBALS;

#include "bricli.h"

// Setup fake functions
FAKE_VALUE_FUNC(int, BspWrite, uint32_t, const char*);
FAKE_VALUE_FUNC(int, Test_Handler, uint32_t, char **);

namespace Cli {

    class ReceiveTest: public ::testing::Test
    {
    protected:
        BriCLICommand_t _commandList[1] =
        {
            {"test", Test_Handler, "Tests."}
        };
        BriCLIHandle_t _cli = BRICLI_HANDLE_DEFAULT;
        char _buffer[100] = {0};

        ReceiveTest() { }
        virtual ~ReceiveTest() { }

        virtual void SetUp()
        {
            // Reset fake functions.
            RESET_FAKE(BspWrite);
            RESET_FAKE(Test_Handler);

            // Pre-load return values for the fakes.
            BspWrite_fake.return_val = (int)BriCLIOk;
            Test_Handler_fake.return_val = (int)BriCLIOk;

            // Configure our default BriCLI settings.
            _cli.CommandList = _commandList;
            _cli.CommandListLength = BRICLI_STATIC_ARRAY_SIZE(_commandList);
            _cli.RxBuffer = _buffer;
            _cli.RxBufferSize = 100;
            _cli.BspWrite = BspWrite;
            _cli.LocalEcho = true;
        }

        virtual void TearDown() 
        {
            BriCLI_ClearBuffer(&_cli);
        }
    };

    TEST_F(ReceiveTest, Init)
    {
        EXPECT_EQ(_cli.Eol, "\n");
        EXPECT_EQ(_cli.RxBufferSize, 100);
        EXPECT_EQ(_cli.RxBuffer, _buffer);
        EXPECT_EQ(_cli.Prompt, ">> ");
        EXPECT_EQ(_cli.State, BriCLIIdle);
    }

    TEST_F(ReceiveTest, ReceiveLine)
    {
        std::string testCommand = "test\n";
        bool eolFound = false;

        // Receive each character in turn and check we grab the EOL correctly.
        for (uint8_t i=0; i<testCommand.size(); i++)
        {
            BriCLI_ReceiveCharacter(&_cli, testCommand[i]);
        }

        eolFound = BriCLI_CheckForEol(&_cli, true);
        EXPECT_TRUE(eolFound);
        EXPECT_STREQ(testCommand.substr(0, 4).c_str(), _buffer);

        // Repeat but check we don't false verify if the EOL is omitted.
        BriCLI_ClearBuffer(&_cli);
        for (uint8_t i=0; i<testCommand.size() - 1; i++)
        {
            BriCLI_ReceiveCharacter(&_cli, testCommand[i]);

            eolFound = BriCLI_CheckForEol(&_cli, true);
        }
        EXPECT_FALSE(eolFound);
        EXPECT_STREQ(testCommand.substr(0, 4).c_str(), _buffer);
    }

    TEST_F(ReceiveTest, ReceiveArray)
    {
        std::string testDataNoEol("test");
        std::string testDataEol("eolcommand\n");
        std::string testDataSplit1("te");
        std::string testDataSplit2("st\n");
        char testDataOverflow[101];
        char testDataNull = '\0';

        bool eolFound = false;
        BriCLIErrors_t error = BriCLIUnknown;

        // Test we can receive data without an EOL find.
        error = BriCLI_ReceiveArray(&_cli, testDataNoEol.size(), (char *)testDataNoEol.c_str());
        eolFound = BriCLI_CheckForEol(&_cli, false);
        EXPECT_FALSE(eolFound);
        EXPECT_EQ(error, BriCLIOk);
        EXPECT_STREQ(testDataNoEol.c_str(), _buffer);

        // Repeat with EOL data.
        BriCLI_ClearBuffer(&_cli);
        error = BriCLIUnknown; 
        error = BriCLI_ReceiveArray(&_cli, testDataEol.size(), (char *)testDataEol.c_str());
        eolFound = BriCLI_CheckForEol(&_cli, false);
        EXPECT_TRUE(eolFound);
        EXPECT_EQ(error, BriCLIOk);
        EXPECT_STREQ(testDataEol.c_str(), _buffer);

        // Check we can receive index based data.
        BriCLI_ClearBuffer(&_cli);
        error = BriCLIUnknown;
        eolFound = false;
        error = BriCLI_ReceiveIndexedArray(&_cli, 3, (testDataEol.size() - 3), (char *)testDataEol.c_str());
        eolFound = BriCLI_CheckForEol(&_cli, false);
        EXPECT_TRUE(eolFound);
        EXPECT_EQ(error, BriCLIOk);
        EXPECT_STREQ(testDataEol.substr(3).c_str(), _buffer);

        // Repeat with split data.
        BriCLI_ClearBuffer(&_cli);
        error = BriCLIUnknown;
        eolFound = false;
        error = BriCLI_ReceiveIndexedArray(&_cli, 0, testDataSplit1.size(), (char *)testDataSplit1.c_str());
        eolFound = BriCLI_CheckForEol(&_cli, false);
        EXPECT_FALSE(eolFound);
        EXPECT_EQ(error, BriCLIOk);
        error = BriCLI_ReceiveIndexedArray(&_cli, 0, testDataSplit2.size(), (char *)testDataSplit2.c_str());
        eolFound = BriCLI_CheckForEol(&_cli, false);
        EXPECT_TRUE(eolFound);
        EXPECT_EQ(error, BriCLIOk);
        EXPECT_STREQ("test\n", _buffer);

        // Check we can handle overflow
        BriCLI_ClearBuffer(&_cli);
        error = BriCLIUnknown;
        eolFound = false;
        memset(&testDataOverflow, 'a', 101);
        error = BriCLI_ReceiveArray(&_cli, 101, testDataOverflow);
        EXPECT_EQ(error, BriCLICopyWouldOverflow);

        // Check we can catch nulls without breaking the cli
        BriCLI_ClearBuffer(&_cli);
        error = BriCLIUnknown;
        eolFound = false;
        error = BriCLI_ReceiveArray(&_cli, 1, &testDataNull);
        EXPECT_EQ(error, BriCLIReceivedNull);
    }

    TEST_F(ReceiveTest, ClearCommand)
    {
        std::string testDataSingle("args 43 \"Hello World\"");
        std::string testDataDouble("test\necho \"Hello\"");

        // Receive a normal, single command.
        BriCLI_ReceiveArray(&_cli, testDataSingle.length(), (char *)testDataSingle.c_str());
        BriCLI_ClearCommand(&_cli);
        EXPECT_EQ(_cli.PendingBytes, 0);
        EXPECT_EQ(_cli.RxBuffer[0], '\0');
        BriCLI_ClearBuffer(&_cli);

        // Receive a multi command.
        BriCLI_ReceiveArray(&_cli, testDataDouble.length(), (char *)testDataDouble.c_str());
        
        // Since we aren't calling parse, manually insert a null between the two commands.
        _cli.RxBuffer[4] = '\0';
        EXPECT_EQ(_cli.PendingBytes, 17);

        // Attempt to remove the first command.
        BriCLI_ClearCommand(&_cli);
        EXPECT_EQ(_cli.PendingBytes, 12);
        EXPECT_STREQ(_cli.RxBuffer, testDataDouble.substr(5).c_str());
    }

    TEST_F(ReceiveTest, Echo)
    {
        std::string testData("add 1 2\n");

        BriCLI_ReceiveArray(&_cli, testData.length(), (char *)testData.c_str());
        
        // Ensure we received the characters and correctly echoed them.
        EXPECT_EQ(_cli.PendingBytes, 8);
        EXPECT_EQ(BspWrite_fake.call_count, 8);

        // Reset and turn off echo.
        BriCLI_Reset(&_cli);
        RESET_FAKE(BspWrite);
        _cli.LocalEcho = false;

        // Ensure that echo wasn't called.
        BriCLI_ReceiveArray(&_cli, testData.length(), (char *)testData.c_str());
        EXPECT_EQ(_cli.PendingBytes, 8);
        EXPECT_EQ(BspWrite_fake.call_count, 0);
    }
}