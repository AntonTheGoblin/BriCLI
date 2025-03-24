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

	memset(cli->Buffer, 0, BRICLI_BUFFER_SIZE);
	
	// Validate the provided EOL string
	if (init->Eol == NULL)
	{
		result = BricliErrorNullArgument;
		goto cleanup;
	}
	else if (strlen(init->Eol) >= BRICLI_EOL_SIZE || strlen(init->Eol) == 0)
	{
		result = BricliErrorInavlidArgument;
		goto cleanup;
	}
	
	// Set our EoL string
	strcpy(cli->Eol, init->Eol);

	// Success
	result = BricliErrorOk;

cleanup:
	return result;
}