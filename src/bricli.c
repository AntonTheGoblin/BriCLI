#include <bricli/bricli.h>

#include <string.h>

/**
 * @brief Initialises our CLI parser with the given parameters
 * 
 * @param cli A pointer to the CLI parser to initialise
 * @param init A pointer to the intilisation options
 * @return BricliError_t BriCliErrorOk if everything was successful
 */
BricliError_t Bricli_Init(Bricli_t* cli, BricliInit_t* init)
{
	BricliError_t result = BricliErrorUnknown;

	memset(cli, 0, sizeof(Bricli_t));
	
	// Validate the provided EOL string
	if (init->RxEol == NULL)
	{
		result = BricliErrorNullArgument;
		goto cleanup;
	}
	else if (strlen(init->RxEol) >= BRICLI_EOL_SIZE || strlen(init->RxEol) == 0)
	{
		result = BricliErrorBadParameter;
		goto cleanup;
	}
	else
	{
		// Set our EoL string
		strcpy(cli->RxEol, init->RxEol);
	}
	

	// Set our BSP Write function
	if (init->BspWrite == NULL)
	{
		result = BricliErrorBadParameter;
		goto cleanup;
	}
	else
	{
		cli->BspWrite = init->BspWrite;
	}

	// Success
	result = BricliErrorOk;

cleanup:
	return result;
}

BricliError_t Bricli_Parse(uint8_t *buffer, uint32_t length)
{
	(void)buffer;
	(void)length;
	return BricliErrorUnknown;
}