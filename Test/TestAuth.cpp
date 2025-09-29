#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include <FFF/fff.h>
DEFINE_FFF_GLOBALS;

#include "bricli.h"

// Setup fake functions
FAKE_VALUE_FUNC(int, BspWrite, uint32_t, const char*);
FAKE_VALUE_FUNC(int, NoAuth_Handler, uint32_t, char **);
FAKE_VALUE_FUNC(int, User_Handler, uint32_t, char **);
FAKE_VALUE_FUNC(int, Admin_Handler, uint32_t, char **);
FAKE_VALUE_FUNC(int, Custom_Handler, uint32_t, char **);

#define CUSTOM_AUTH_LEVEL (1 << 16)

namespace Cli {

    class AuthTest: public ::testing::Test
    {
    protected:
        BricliCommand_t _commandList[1] =
        {
            {"anonymous", NoAuth_Handler, "No auth", BricliAuthNone},
            {"user", User_Handler, "User level command", BricliAuthUser},
            {"admin", Admin_Handler, "admin level command", (BricliAuthAdmin | BricliAuthUser) },
            {"admin", Admin_Handler, "admin level command", CUSTOM_AUTH_LEVEL }
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
			
            // Pre-load return values for the fakes.
            BspWrite_fake.return_val = (int)BricliOk;
            User_Handler_fake.return_val = (int)BricliOk;
			
            // Configure our default BriCLI settings.
			BricliInit_t init = {0};
            init.CommandList = _commandList;
            init.CommandListLength = BRICLI_STATIC_ARRAY_SIZE(_commandList);
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

    TEST_F(AuthTest, HelpShowsLogin)
    {
		Bricli_PrintHelp(&_cli);
	}
}