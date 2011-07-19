/******************************************************************************
 *
 * NAME:          pcsc-wrappers.h
 *
 * AUTHORS:       Fabio Casadei
 *                fabio.casadei@mercurio-ws.com
 *
 *		  Antonio Savoldi
 *		  antonio.savoldi@ing.unibs.it  
 *
 * FILE TYPE:     Header file
 *
 * DESCRIPTION:   this file contains constant definitions and function
 *                prototypes for pcsc-wrappers.c 
 *
 *****************************************************************************/

//#include <unistd.h>
#include "winscard.h"

//
// CONSTANT DEFINITIONS
//

// PCSC-RELATED ERROR MESSAGES DEFINITION

#define ERR_INVALID_SCOPE           "invalid scope passed as parameter"
#define ERR_INVALID_CONTEXT_HANDLER "invalid context handler passed as parameter"
#define ERR_INSUFFICIENT_BUFFER     "insufficient buffer passed as parameter" 
#define ERR_READER_NOT_ACCESSIBLE   "reader is not accessible"

#define ERR_PORT_NOT_READY          "it is not possible to allocate specified port"
#define ERR_INVALID_PARAMETER_VALUE "sharing mode, protocol requested or reader name not valid"
#define ERR_READER_UNAVAILABLE      "it is not possible to power up the reader or the card"
#define ERR_PROTOCOL_UNAVAILABLE    "protocol not supported"
#define ERR_SHARING_VIOLATION       "someone else owns exclusive rights on this card"

#define ERR_INVALID_CONNECTION_HANDLER "invalid connection handler passed as parameter"
#define ERR_INVALID_DIRECTIVE          "an invalid connection release directive has been requested"

#define ERR_READER_REMOVED "reader has been probably removed"

#define ERR_APDU_EXCHANGE_FAILED "APDU exchange failed"
#define ERR_PROTOCOL_DIFFERENT   "connection protocol passed as parameter is different from that specified"
#define ERR_CARD_RESET           "the smartcard has been reset by another process"
#define ERR_CARD_REMOVED         "the smartcard has been removed from the reader"
 
#define ERR_WRONG_RETURN_VALUE "a value not contemplated has been returned"

#define ERR_NOT_MF_ROOT "the file selected after ATR is not MF."



//
// FUNCTION PROTOTYPES
//

SCARDCONTEXT SIM_Establish_Context(DWORD dwScope);

void SIM_Release_Context(SCARDCONTEXT hContext);

LPSTR Detect_Readers(SCARDCONTEXT hContext);

SCARDHANDLE SIM_Connect(SCARDCONTEXT hContext, LPSTR szReader);

RESPONSE* SIM_Status(SCARDHANDLE hCard);

void SIM_Begin_Transaction(SCARDHANDLE hCard);

void SIM_End_Transaction(SCARDHANDLE hCard);

RESPONSE* gsm_select(SCARDHANDLE hCard, SCARD_IO_REQUEST* dwActiveProtocol,
		     long int id_file);

RESPONSE* gsm_get_response(SCARDHANDLE hCard,
			   SCARD_IO_REQUEST* dwActiveProtocol,
			   BYTE resp_lgth);

RESPONSE* gsm_read_binary_file(SCARDHANDLE hCard,
			       SCARD_IO_REQUEST* dwActiveProtocol,
			       BYTE block_dim);

RESPONSE* gsm_verify_chv(SCARDHANDLE hCard, SCARD_IO_REQUEST* dwActiveProtocol,
			 BYTE_LIST* chv1);

RESPONSE* gsm_read_transparent_file (SCARDHANDLE hCard,
				     SCARD_IO_REQUEST* dwActiveProtocol,
				     BYTE block_dim);


RESPONSE* gsm_read_record (SCARDHANDLE hCard,
			   SCARD_IO_REQUEST* dwActiveProtocol,
			   BYTE rec_number,
			   BYTE mode,
			   BYTE rec_dim);

REC_RESPONSE* gsm_read_linear_fixed_file (SCARDHANDLE hCard,
		  		          SCARD_IO_REQUEST* dwActiveProtocol,
				          int file_size,
				          BYTE record_dim);

REC_RESPONSE* gsm_read_cyclic_file (SCARDHANDLE hCard,
		  		    SCARD_IO_REQUEST* dwActiveProtocol,
			            int file_size,
			            BYTE record_dim);
