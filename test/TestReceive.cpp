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
        BricliCommand_t _commandList[2] =
        {
            {"test", Test_Handler, "Tests", BricliScopeAll},
			BRICLI_COMMAND_LIST_TERMINATOR
        };
        BricliHandle_t _cli;
        char _buffer[100] = {0};

        ReceiveTest() { }
        virtual ~ReceiveTest() { }

        virtual void SetUp()
        {
            // Reset fake functions.
            RESET_FAKE(BspWrite);
            RESET_FAKE(Test_Handler);

            // Pre-load return values for the fakes.
            BspWrite_fake.return_val = (int)BricliOk;
            Test_Handler_fake.return_val = (int)BricliOk;

            // Configure our default BriCLI settings.
            BricliInit_t init = {0};
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

    TEST_F(ReceiveTest, ReceiveLine)
    {
        std::string testCommand = "test\n";
        bool eolFound = false;

        // Receive each character in turn and check we grab the EOL correctly.
        for (uint8_t i=0; i<testCommand.size(); i++)
        {
            Bricli_ReceiveCharacter(&_cli, testCommand[i]);
        }

        eolFound = Bricli_CheckForEol(&_cli, true);
        EXPECT_TRUE(eolFound);
        EXPECT_STREQ(testCommand.substr(0, 4).c_str(), _buffer);

        // Repeat but check we don't false verify if the EOL is omitted.
        Bricli_ClearBuffer(&_cli);
        for (uint8_t i=0; i<testCommand.size() - 1; i++)
        {
            Bricli_ReceiveCharacter(&_cli, testCommand[i]);

            eolFound = Bricli_CheckForEol(&_cli, true);
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
        BricliErrors_t error = BricliUnknown;

        // Test we can receive data without an EOL find.
        error = Bricli_ReceiveArray(&_cli, testDataNoEol.size(), (char *)testDataNoEol.c_str());
        eolFound = Bricli_CheckForEol(&_cli, false);
        EXPECT_FALSE(eolFound);
        EXPECT_EQ(error, BricliOk);
        EXPECT_STREQ(testDataNoEol.c_str(), _buffer);

        // Repeat with EOL data.
        Bricli_ClearBuffer(&_cli);
        error = BricliUnknown; 
        error = Bricli_ReceiveArray(&_cli, testDataEol.size(), (char *)testDataEol.c_str());
        eolFound = Bricli_CheckForEol(&_cli, false);
        EXPECT_TRUE(eolFound);
        EXPECT_EQ(error, BricliOk);
        EXPECT_STREQ(testDataEol.c_str(), _buffer);

        // Check we can receive index based data.
        Bricli_ClearBuffer(&_cli);
        error = BricliUnknown;
        eolFound = false;
        error = Bricli_ReceiveIndexedArray(&_cli, 3, (testDataEol.size() - 3), (char *)testDataEol.c_str());
        eolFound = Bricli_CheckForEol(&_cli, false);
        EXPECT_TRUE(eolFound);
        EXPECT_EQ(error, BricliOk);
        EXPECT_STREQ(testDataEol.substr(3).c_str(), _buffer);

        // Repeat with split data.
        Bricli_ClearBuffer(&_cli);
        error = BricliUnknown;
        eolFound = false;
        error = Bricli_ReceiveIndexedArray(&_cli, 0, testDataSplit1.size(), (char *)testDataSplit1.c_str());
        eolFound = Bricli_CheckForEol(&_cli, false);
        EXPECT_FALSE(eolFound);
        EXPECT_EQ(error, BricliOk);
        error = Bricli_ReceiveIndexedArray(&_cli, 0, testDataSplit2.size(), (char *)testDataSplit2.c_str());
        eolFound = Bricli_CheckForEol(&_cli, false);
        EXPECT_TRUE(eolFound);
        EXPECT_EQ(error, BricliOk);
        EXPECT_STREQ("test\n", _buffer);

        // Check we can handle overflow
        Bricli_ClearBuffer(&_cli);
        error = BricliUnknown;
        eolFound = false;
        memset(&testDataOverflow, 'a', 101);
        error = Bricli_ReceiveArray(&_cli, 101, testDataOverflow);
        EXPECT_EQ(error, BricliCopyWouldOverflow);

        // Check we can catch nulls without breaking the cli
        Bricli_ClearBuffer(&_cli);
        error = BricliUnknown;
        eolFound = false;
        error = Bricli_ReceiveArray(&_cli, 1, &testDataNull);
        EXPECT_EQ(error, BricliReceivedNull);
    }

    TEST_F(ReceiveTest, ClearCommand)
    {
        std::string testDataSingle("args 43 \"Hello World\"");
        std::string testDataDouble("test\necho \"Hello\"");

        // Receive a normal, single command.
        Bricli_ReceiveArray(&_cli, testDataSingle.length(), (char *)testDataSingle.c_str());
        Bricli_ClearCommand(&_cli);
        EXPECT_EQ(_cli.PendingBytes, 0);
        EXPECT_EQ(_cli.RxBuffer[0], '\0');
        Bricli_ClearBuffer(&_cli);

        // Receive a multi command.
        Bricli_ReceiveArray(&_cli, testDataDouble.length(), (char *)testDataDouble.c_str());
        
        // Since we aren't calling parse, manually insert a null between the two commands.
        _cli.RxBuffer[4] = '\0';
        EXPECT_EQ(_cli.PendingBytes, 17);

        // Attempt to remove the first command.
        Bricli_ClearCommand(&_cli);
        EXPECT_EQ(_cli.PendingBytes, 12);
        EXPECT_STREQ(_cli.RxBuffer, testDataDouble.substr(5).c_str());
    }

    TEST_F(ReceiveTest, Echo)
    {
        std::string testData("add 1 2\n");

        Bricli_ReceiveArray(&_cli, testData.length(), (char *)testData.c_str());
        
        // Ensure we received the characters and correctly echoed them.
        EXPECT_EQ(_cli.PendingBytes, 8);
        EXPECT_EQ(BspWrite_fake.call_count, 8);

        // Reset and turn off echo.
        Bricli_Reset(&_cli);
        RESET_FAKE(BspWrite);
        _cli.LocalEcho = false;

        // Ensure that echo wasn't called.
        Bricli_ReceiveArray(&_cli, testData.length(), (char *)testData.c_str());
        EXPECT_EQ(_cli.PendingBytes, 8);
        EXPECT_EQ(BspWrite_fake.call_count, 0);
    }
}