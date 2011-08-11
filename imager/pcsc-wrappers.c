/******************************************************************************
 *
 * NAME:          pcsc-wrappers.c
 *
 * AUTHORS:       Fabio Casadei
 *		  fabio.casadei@mercurio-ws.com
 *
 *		  Antonio Savoldi
 *		  antonio.savoldi@ing.unibs.it
 *
 * FILE TYPE:     C source file
 *
 * DESCRIPTION:   this file contains the definiton of those functions that
 *                act as wrappers of the pcsc-lite library functions
 *
 *****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "utils.h"
#include "gsm-constants.h"
#include "pcsc-wrappers.h"
#include "debug.h"

//
// FUNCTION NAME: SIM_Establish_Context
//
// DESCRIPTION: this is a wrapper for pcsclite SCardEstablishContext function.
//              Its job is to call that function and manage error messages
//              eventually returned.
//
// INPUT PARAMETERS:
//   DWORD dwScope         a pcsclite defined constant that states which scope
//                         the newly created context will have.
//
// OUTPUT PARAMETERS:  SCARDCONTEXT
//   If nothing goes bad, returns a handler for the new connection, otherwise
//   exits.
//
// NOTES:
//   - For the pcsclite manual is not complete on values retuned by its API
//     functions, further cases may be included in the switch(rv) in the
//     future.
//
SCARDCONTEXT SIM_Establish_Context(DWORD dwScope) {

  SCARDCONTEXT hContext;
  LONG rv;

  rv = SCardEstablishContext(dwScope, NULL, NULL, &hContext);

  switch (rv) {
  case SCARD_E_INVALID_VALUE: {
    perror(strcat("SIM_Establish_Context:",ERR_INVALID_SCOPE));
    exit(EXIT_FAILURE);
  }
  case SCARD_S_SUCCESS: {
    printf("Context has been successfully established.\n");
    return(hContext);
  }
  default: {
    perror(strcat("SIM_Establish_Context:",ERR_WRONG_RETURN_VALUE));
    exit(EXIT_FAILURE);
  }
  }

}


//
// FUNCTION NAME: SIM_Release_Context
//
// DESCRIPTION: this is a wrapper for pcsclite SCardReleaseContext function.
//              Its job is to call that function and manage error messages
//              eventually returned.
//
// INPUT PARAMETERS:
//   SCARDCONTEXT hContext     connection context to be closed.
//
// OUTPUT PARAMETERS:
//   None.
//
void SIM_Release_Context(SCARDCONTEXT hContext) {
  LONG rv;

  rv = SCardReleaseContext(hContext);
  switch(rv) {
  case SCARD_E_INVALID_HANDLE: {
    perror(strcat("SIM_Release_Context:",ERR_INVALID_CONTEXT_HANDLER));
    exit(EXIT_FAILURE);
  }
  case SCARD_S_SUCCESS: {
    printf("Context has been successfully released.\n");
    break;
  }
  default:  {
    perror(strcat("SIM_Release_Context:",ERR_WRONG_RETURN_VALUE));
    exit(EXIT_FAILURE);
  }
  }
}



//
// FUNCTION NAME: Detect_Readers
//
// DESCRIPTION: this is a wrapper for pcsclite SCardListReaders function.
//              Its job is to call that function and manage error messages
//              eventually returned.
//
// LAST MODIFIED: 12 Giu 2004
//
// INPUT PARAMETERS:
//   SCARDCONTEXT hContext    connection context to the PC/SC resource manager.
//
// OUTPUT PARAMETERS:  LPSTR
//   If nothing goes bad, returns a list of active readers, otherwise exits.
//
// NOTES:
//   -  Check about the second call of SCardListReaders: parameter has to be
//      passed by VALUE or by REFERENCE?
LPSTR Detect_Readers(SCARDCONTEXT hContext) {
  LPSTR mszGroups;
  LPSTR mszReaders;
  DWORD dwReaders;
  LONG rv;

  rv = SCardListReaders(hContext,NULL,NULL,&dwReaders);
  mszReaders = (LPSTR)malloc(sizeof(char)*dwReaders);

  //Fill mszReaders with the names of active readers
  rv = SCardListReaders(hContext, mszGroups, mszReaders, &dwReaders);

  switch (rv) {
  case SCARD_E_INVALID_HANDLE: {
    perror(strcat("Detect_Readers:",ERR_INVALID_SCOPE));
    exit(EXIT_FAILURE);
  }
  case SCARD_E_INSUFFICIENT_BUFFER: {
    perror(strcat("Detect_Readers:",ERR_INSUFFICIENT_BUFFER));
    exit(EXIT_FAILURE);
  }
  case SCARD_S_SUCCESS: {
    printf("Active readers detected: %s. \n",mszReaders);
    return(mszReaders);
  }
  default: {
    perror(strcat("Detect_Readers:",ERR_WRONG_RETURN_VALUE));
    exit(EXIT_FAILURE);
  }
  }
}



//
// FUNCTION NAME: SIM_Connect
//
// DESCRIPTION: this is a wrapper for pcsclite SCardConnect function.
//              Its job is to call that function and manage error messages
//              eventually returned.
//
// INPUT PARAMETERS:
//   SCARDCONTEXT  hContext    connection context to the PC/SC resource manager
//   LPCSTR        szReader    reader name to connect to
//
// OUTPUT PARAMETERS:  LPSCARDHANDLE
//   If nothing goes bad, returns a handler for this card connection, otherwise
//   exits.
//
// NOTES:
//   - Possible extension will require additional parameters to be chosen by
//     the caller.
//
SCARDHANDLE SIM_Connect(SCARDCONTEXT hContext, LPSTR szReader) {
  SCARDHANDLE hCard;
  DWORD dwActiveProtocol;
  LONG rv;

  rv = SCardConnect(hContext, szReader, SCARD_SHARE_SHARED,
		    SCARD_PROTOCOL_T0, &hCard, &dwActiveProtocol);
  switch(rv) {
  case SCARD_E_NOT_READY: {
    perror(strcat("SIM_Connect:",ERR_PORT_NOT_READY));
    exit(EXIT_FAILURE);
  }
  case SCARD_E_INVALID_VALUE: {
    perror(strcat("SIM_Connect:",ERR_INVALID_PARAMETER_VALUE));
    exit(EXIT_FAILURE);
  }
  case SCARD_E_READER_UNAVAILABLE: {
    perror(strcat("SIM_Connect:",ERR_READER_UNAVAILABLE));
    exit(EXIT_FAILURE);
  }
  case SCARD_E_UNSUPPORTED_FEATURE: {
    perror(strcat("SIM_Connect:",ERR_PROTOCOL_UNAVAILABLE));
    exit(EXIT_FAILURE);
  }
  case SCARD_E_SHARING_VIOLATION: {
    perror(strcat("SIM_Connect:",ERR_SHARING_VIOLATION));
    exit(EXIT_FAILURE);
  }
  case SCARD_E_INVALID_HANDLE: {
    perror(strcat("SIM_Connect:",ERR_INVALID_CONTEXT_HANDLER));
    exit(EXIT_FAILURE);
  }
  case SCARD_S_SUCCESS: {
    printf("Connection with SIM card opened successfully.\n");
    return(hCard);
  }
  default: {
    perror(strcat("SIM_Connect:",ERR_WRONG_RETURN_VALUE));
    printf("The card returned: %li.\n",rv);
    printf("Suggestion: maybe card name was mistyped.\n");
    exit(EXIT_FAILURE);
  }
  }
}



//
// FUNCTION NAME: SIM_Disconnect
//
// DESCRIPTION: this is a wrapper for pcsclite SCardDisconnect function.
//              Its job is to call that function and manage error messages
//              eventually returned.
//
// INPUT PARAMETERS:
//   SCARDHANDLE  hCard    connection handler for this card
//
// OUTPUT PARAMETERS:
//   None.
//
void SIM_Disconnect(SCARDHANDLE hCard) {
  LONG rv;

  rv = SCardDisconnect(hCard, SCARD_UNPOWER_CARD);
  switch(rv) {
  case SCARD_E_INVALID_HANDLE: {
    perror(strcat("SIM_Disconnect:",ERR_INVALID_CONTEXT_HANDLER));
    exit(EXIT_FAILURE);
  }
  case SCARD_E_INVALID_VALUE: {
    perror(strcat("SIM_Disconnect:",ERR_INVALID_DIRECTIVE));
    exit(EXIT_FAILURE);
  }
  case SCARD_S_SUCCESS: {
    printf("SIM card connection successfully released.\n");
    break;
  }
  default: {
    perror(strcat("SIM_Disconnect:",ERR_WRONG_RETURN_VALUE));
    exit(EXIT_FAILURE);
  }
  }
}



//
// FUNCTION NAME: SIM_Status
//
// DESCRIPTION: this is a wrapper for pcsclite SCardStatus function.
//              Its job is to call that function and manage error messages
//              eventually returned.
//
// INPUT PARAMETERS:
//   SCARDHANDLE hCard    connection handler for this card.
//
// OUTPUT PARAMETERS: RESPONSE*
//   If nothing goes wrong, atr from the card is returned.
//
RESPONSE* SIM_Status(SCARDHANDLE hCard) {
  LONG rv;
  DWORD pcchReaderLen, dwState, dwProtocol;
  BYTE pbAtr[MAX_ATR_SIZE];
  DWORD pcbAtrLen;
  LPSTR lettori;

  int i;
  RESPONSE* resp;

  lettori = (LPSTR)malloc(sizeof(char)*50);
  pcchReaderLen = 50;

  rv = SCardStatus(hCard, lettori, &pcchReaderLen,
		   &dwState, &dwProtocol, pbAtr, &pcbAtrLen);
  switch(rv) {
  case SCARD_E_READER_UNAVAILABLE: {
    perror(strcat("SIM_Status:",ERR_READER_REMOVED));
    exit(EXIT_FAILURE);
  }
  case SCARD_E_INSUFFICIENT_BUFFER: {
    perror(strcat("SIM_Status:",ERR_INSUFFICIENT_BUFFER));
    exit(EXIT_FAILURE);
  }
  case SCARD_E_INVALID_HANDLE: {
    perror(strcat("SIM_Status:",ERR_INVALID_CONTEXT_HANDLER));
    exit(EXIT_FAILURE);
  }
  case SCARD_S_SUCCESS: {
    printf("\n\nCARD STATUS = :\n");
    switch(dwState) {
      case SCARD_ABSENT: {
	printf("\tThere is no card into the reader.");
	break;
      }
      case SCARD_PRESENT: {
	printf("\tCard present but not ready to be used.");
	break;
      }
      case SCARD_SWALLOWED: {
	printf("\tCard present and ready to be used, but not powered.");
	break;
      }
      case SCARD_POWERED: {
	printf("\tCard powered, but mode not recognized by the driver.");
	break;
      }
      case SCARD_NEGOTIABLE: {
	printf("\tCard reset and waiting PTS negotiation.");
	break;
      }
      case SCARD_SPECIFIC: {
	printf("\tCard reset and specific communication protocols established.");
	break;
      }
    default: {
      printf("\tCARD ANSWERED WITH AN UNKNOWN STATE: %ld",dwState);
    }
    }

    printf("\n\nCommunication protocol is:\n");
    switch(dwProtocol) {
      case SCARD_PROTOCOL_T0: {
	printf("\tT=0.\n");
	break;
      }
      case SCARD_PROTOCOL_T1: {
	printf("\tT=1.\n");
	break;
      }
    default: {
      printf("\tCARD ANSWERED WITH AN UNKNOWN PROTOCOL.\n");
    }
    }

    printf("\n\nATR = ");
    print_array(pbAtr,pcbAtrLen);
    printf("\n\nATR lenght = %ld.\n",pcbAtrLen);


    resp = create_b_list();
    for(i=0; i<pcbAtrLen; i++) {
      blist_add_element(resp,pbAtr[i]);
    }

    return(resp);
  }
  default: {
    perror(strcat("SIM_Status:",ERR_WRONG_RETURN_VALUE));
    printf("\nValue returned =  %ld\n",rv);
    exit(EXIT_FAILURE);
  }
  }
}



//
// FUNCTION NAME: SIM_Begin_Transaction
//
// DESCRIPTION: this is a wrapper for pcsclite SCardBeginTransaction function.
//              Its job is to call that function and manage error messages
//              eventually returned.
//
// INPUT PARAMETERS:
//   SCARDHANDLE  hCard    connection handler for this card.
//
// OUTPUT PARAMETERS:
//   None.
//
void SIM_Begin_Transaction(SCARDHANDLE hCard) {
  LONG rv;

  rv = SCardBeginTransaction(hCard);
  switch(rv) {
  case SCARD_E_READER_UNAVAILABLE: {
    perror(strcat("SIM_Begin_Transaction:",ERR_READER_REMOVED));
    exit(EXIT_FAILURE);
  }
  case SCARD_E_SHARING_VIOLATION: {
    perror(strcat("SIM_Begin_Transaction:",ERR_SHARING_VIOLATION));
    exit(EXIT_FAILURE);
  }
  case SCARD_E_INVALID_HANDLE: {
    perror(strcat("SIM_Begin_Transaction:",ERR_INVALID_CONTEXT_HANDLER));
    exit(EXIT_FAILURE);
  }
  case SCARD_S_SUCCESS: {
    printf("TRANSACTION SUCCESSFULLY OPENED...\n");
    break;
  }
  default: {
    perror(strcat("SIM_Begin_Transaction:",ERR_WRONG_RETURN_VALUE));
    exit(EXIT_FAILURE);
  }
  }
}



//
// FUNCTION NAME: SIM_End_Transaction
//
// DESCRIPTION: this is a wrapper for pcsclite SCardEndTransaction function.
//              Its job is to call that function and manage error messages
//              eventually returned.
//
// INPUT PARAMETERS:
//   SCARDHANDLE  hCard    connection handler for this card.
//
// OUTPUT PARAMETERS:
//   None.
//
// NOTES:
//   -  Possible extension will require additional parameters to be chosen by
//      the caller.
//
void SIM_End_Transaction(SCARDHANDLE hCard) {
  LONG rv;

  rv = SCardEndTransaction(hCard, SCARD_LEAVE_CARD);
  switch(rv) {
  case SCARD_E_READER_UNAVAILABLE: {
    perror(strcat("SIM_End_Transaction:",ERR_READER_REMOVED));
    exit(EXIT_FAILURE);
  }
  case SCARD_E_SHARING_VIOLATION: {
    perror(strcat("SIM_End_Transaction:",ERR_SHARING_VIOLATION));
    exit(EXIT_FAILURE);
  }
  case SCARD_E_INVALID_HANDLE: {
    perror(strcat("SIM_End_Transaction:",ERR_INVALID_CONTEXT_HANDLER));
    exit(EXIT_FAILURE);
  }
  case SCARD_S_SUCCESS: {
    printf("TRANSACTION SUCCESSFULLY CLOSED.\n");
    break;
  }
  default: {
    perror(strcat("SIM_End_Transaction:",ERR_WRONG_RETURN_VALUE));
    exit(EXIT_FAILURE);
  }
  }
}



//
// FUNCTION NAME: gsm_select
//
// DESCRIPTION: this is a wrapper for pcsclite SCardTransmit function, arranged
//              to transmit a SELECT command to the card.
//
// INPUT PARAMETERS:
//   SCARDHANDLE hCard                     handle for a connection to the card.
//   SCARD_IO_REQUEST* dwActiveProtocol    pointer to the pcsclite defined
//                                         structure that implement the desired
//                                         protocol.
//   int id_file                           file of the card to select.
//
// OUTPUT PARAMETERS: RESPONSE*
//   Returns a pointer to the raw response to the SELECT command from the card.
//
// NOTES:
//   - For the pcsclite manual is not complete on values retuned by its API
//     functions, further cases may be included in the switch(rv) in the
//     future.
//   - Error handling is up to this function: every time something goes wrong,
//     it exits.
//
RESPONSE* gsm_select(SCARDHANDLE hCard, SCARD_IO_REQUEST* dwActiveProtocol,
		     long int id_file) {

  int i;
  LONG rv;
  BYTE pbRecvBuffer[2]; // I expect a response composed only by the status
                        // words
  BYTE pbSendBuffer[7] = {GSM_CLASS, SELECT, 0x00, 0x00, 0x02,
                          (BYTE)((id_file & 0xFF00)>>8), // this is the msb of
			                                 // the address
			  (BYTE)(id_file & 0x00FF)};     // this is the lsb
  DWORD dwSendLength = (int)sizeof(pbSendBuffer);
  DWORD pcbRecvLength = (int)sizeof(pbRecvBuffer);
  SCARD_IO_REQUEST pioRecvPci;
  RESPONSE* response = create_b_list();

  if ((id_file < 0x0000) || (id_file > 0xFFFF)) {
    perror(strcat("gsm_select: ","id_file passed was out of range. It must remain into the interval 0x0000 - 0xFFFF"));
    exit(EXIT_FAILURE);
  }

  rv = SCardTransmit(hCard, dwActiveProtocol, pbSendBuffer, dwSendLength,
		     &pioRecvPci, pbRecvBuffer, &pcbRecvLength);
  switch(rv) {
  case SCARD_E_NOT_TRANSACTED: {
    perror(strcat("SCardTransmit:", ERR_APDU_EXCHANGE_FAILED));
    exit(EXIT_FAILURE);
  }
  case SCARD_E_INVALID_VALUE: {
    perror(strcat("SCardTransmit", ERR_INVALID_PARAMETER_VALUE));
    exit(EXIT_FAILURE);
  }
  case SCARD_E_READER_UNAVAILABLE: {
    perror(strcat("SCardTransmit", ERR_READER_UNAVAILABLE));
    exit(EXIT_FAILURE);
  }
  case SCARD_E_PROTO_MISMATCH: {
    perror(strcat("SCardTransmit", ERR_PROTOCOL_DIFFERENT));
    exit(EXIT_FAILURE);
  }
  case SCARD_W_RESET_CARD: {
    perror(strcat("SCardTransmit", ERR_CARD_RESET));
    exit(EXIT_FAILURE);
  }
  case SCARD_W_REMOVED_CARD: {
    perror(strcat("SCardTransmit", ERR_CARD_REMOVED));
    exit(EXIT_FAILURE);
  }
  case SCARD_E_INVALID_HANDLE: {
    perror(strcat("SCardTransmit", ERR_INVALID_CONTEXT_HANDLER));
    exit(EXIT_FAILURE);
  }
  case SCARD_S_SUCCESS: { //DEBUG
    //printf("APDU SELECT successfully transmitted.\n");
    //printf("Summary of input data:\n");
    //printf("SENT APDU SELECT: ");
    //print_array(pbSendBuffer,dwSendLength);
    //printf("\nLength of sent APDU SELECT: %i\n", dwSendLength);
    //printf("RECEIVED RESPONSE: ");
    //print_array(pbRecvBuffer,pcbRecvLength);
    //printf("\n");
    //printf("Length of received response: %i.\n", pcbRecvLength);
    break;

  }
  default: {
    perror(strcat("SCardTransmit", ERR_WRONG_RETURN_VALUE));
    exit(EXIT_FAILURE);
  }
  }

  for (i=0; i<pcbRecvLength; i++) {
   blist_add_element(response, pbRecvBuffer[i]);
  }

  if((*response).b_list_length != pcbRecvLength) {
    perror(strcat("gsm_select: ", PEB_WRONG_LIST_LENGTH));
    exit(EXIT_FAILURE);
  }

  return(response);
}


//
// FUNCTION NAME: gsm_get_response
//
// DESCRIPTION: this is a wrapper for pcsclite SCardTransmit function, arranged
//              to transmit a GET RESPONSE command to the card.
//
// INPUT PARAMETERS:
//   SCARDHANDLE hCard                     handle for a connection to the card.
//   SCARD_IO_REQUEST* dwActiveProtocol    pointer to the pcsclite defined
//                                         structure that implement the desired
//                                         protocol.
//   BYTE resp_lgth                        length of the response as learned
//                                         from a previous successfully SELECT.
//
// OUTPUT PARAMETERS:  RESPONSE*
//   Returns a pointer to the raw response to GET RESPONSE from the card.
//
// NOTES:
//   - For the pcsclite manual is not complete on values retuned by its API
//     functions, further cases may be included in the switch(rv) in the
//     future.
//   - Error handling is up to this function: every time something goes wrong,
//     it exits.
//
RESPONSE* gsm_get_response(SCARDHANDLE hCard,
			   SCARD_IO_REQUEST* dwActiveProtocol,
			   BYTE resp_lgth) {

  int i;
  LONG rv;
  BYTE pbSendBuffer[5] = {GSM_CLASS, GET_RESPONSE, 0x00, 0x00, resp_lgth};
  BYTE pbRecvBuffer[resp_lgth+2]; //Buffer must contain SW1 and SW2 also
  DWORD dwSendLength = sizeof(pbSendBuffer);
  DWORD pcbRecvLength = sizeof(pbRecvBuffer);
  SCARD_IO_REQUEST pioRecvPci;
  RESPONSE* response = create_b_list();

  inizializza_array(pbRecvBuffer,(int)sizeof(pbRecvBuffer));

  rv = SCardTransmit(hCard, dwActiveProtocol, pbSendBuffer, dwSendLength,
		     &pioRecvPci, pbRecvBuffer, &pcbRecvLength);

  switch(rv) {
  case SCARD_E_NOT_TRANSACTED: {
    perror(strcat("SCardTransmit", ERR_APDU_EXCHANGE_FAILED));
    /*printf("Summary of input data:\n");
    printf("SENT APDU GET RESPONSE: ");
    print_array(pbSendBuffer,(int)sizeof(pbSendBuffer));
    printf("\nLength of sent APDU GET RESPONSE: %ld\n", dwSendLength);
    printf("RECEIVED RESPONSE: ");
    print_array(pbRecvBuffer,pcbRecvLength);
    printf("\n");
    printf("Length of received response: %ld.\n", pcbRecvLength);*/
    DBG("Summary of input data:\n");
    DBG("SENT APDU GET RESPONSE: ");
    DBG(pbSendBuffer,(int)sizeof(pbSendBuffer));
    DBG("\nLength of sent APDU GET RESPONSE: %ld\n", dwSendLength);
    DBG("RECEIVED RESPONSE: ");
    #ifdef DEBUG
    print_array(pbRecvBuffer,pcbRecvLength);
    #endif
    DBG("\n");
    DBG("Length of received response: %ld.\n", pcbRecvLength);
    exit(EXIT_FAILURE);
  }
  case SCARD_E_INVALID_VALUE: {
    perror(strcat("SCardTransmit", ERR_INVALID_PARAMETER_VALUE));
    exit(EXIT_FAILURE);
  }
  case SCARD_E_READER_UNAVAILABLE: {
    perror(strcat("SCardTransmit", ERR_READER_UNAVAILABLE));
    exit(EXIT_FAILURE);
  }
  case SCARD_E_PROTO_MISMATCH: {
    perror(strcat("SCardTransmit", ERR_PROTOCOL_DIFFERENT));
    exit(EXIT_FAILURE);
  }
  case SCARD_W_RESET_CARD: {
    perror(strcat("SCardTransmit", ERR_CARD_RESET));
    exit(EXIT_FAILURE);
  }
  case SCARD_W_REMOVED_CARD: {
    perror(strcat("SCardTransmit", ERR_CARD_REMOVED));
    exit(EXIT_FAILURE);
  }
  case SCARD_E_INVALID_HANDLE: {
    perror(strcat("SCardTransmit", ERR_INVALID_CONTEXT_HANDLER));
    exit(EXIT_FAILURE);
  }
  case SCARD_S_SUCCESS: {
    /*
      // DEBUG MESSAGES
    printf("APDU GET RESPONSE successfully transmitted.\n");
    printf("Summary of input data:\n");
    printf("SENT APDU GET RESPONSE: ");
    print_array(pbSendBuffer,dwSendLength);
    printf("\nLength of sent APDU GET RESPONSE: %i\n", dwSendLength);
    printf("RECEIVED RESPONSE: ");
    print_array(pbRecvBuffer,pcbRecvLength);
    printf("\n");
    printf("Length of received response: %i.\n", pcbRecvLength);
    */
    break;
  }
  default: {
    perror(strcat("SCardTransmit", ERR_WRONG_RETURN_VALUE));
    //printf("The value returned is: %ld",rv);
    DBG("The value returned is: %ld",rv);
    exit(EXIT_FAILURE);
  }
  }

  for (i=0; i<pcbRecvLength; i++) {
    blist_add_element(response, pbRecvBuffer[i]);
  }

  return(response);
}



//
// FUNCTION NAME: gsm_verify_chv
//
// DESCRIPTION: this is a wrapper for pcsclite SCardTransmit function, arranged
//              to transmit a VERIFY CHV command to the card so that the access
//              status CHV1 can be reached.
//
// INPUT PARAMETERS:
//   SCARDHANDLE       hCard               handle for a connection to the card.
//   SCARD_IO_REQUEST* dwActiveProtocol    pointer to the pcsclite defined
//                                         structure that implement the desired
//                                         protocol.
//   BYTE_LIST         chv1                value of chv1 to be verified.
//
// OUTPUT PARAMETERS: RESPONSE*
//   Returns the response from the SIM card.
//
// NOTES:
//   - This function verifies exclusively CHV1. If CHV2 verification will be
//     needed in the future, then the function could be easily changed.
//   - This function doesn't work because CHV1 value has to be coded as
//     specified in CCITT T.50 (ISO 646 - 1983)
//
RESPONSE* gsm_verify_chv(SCARDHANDLE hCard, SCARD_IO_REQUEST* dwActiveProtocol,
			 BYTE_LIST* chv1) {

  int i;
  LONG rv;
  RESPONSE* response = create_b_list();

  DWORD dwSendLength, pcbRecvLength;
  SCARD_IO_REQUEST pioRecvPci;

  //We're about to verify CHV1
  BYTE pbSendBuffer[13] = {GSM_CLASS, VERIFY_CHV, 0x00, 0x01, 0x08,
			   blist_get_element(chv1,0),
			   blist_get_element(chv1,1),
			   blist_get_element(chv1,2),
			   blist_get_element(chv1,3),
			   blist_get_element(chv1,4),
			   blist_get_element(chv1,5),
			   blist_get_element(chv1,6),
			   blist_get_element(chv1,7)};
  BYTE pbRecvBuffer[12]; //SW1 e SW2 included

  dwSendLength = sizeof(pbSendBuffer);
  pcbRecvLength = sizeof(pbRecvBuffer);

  //printf("\nDEBUG: pcbRecvLength = %ld\n", pcbRecvLength);
  DBG("\nDEBUG: pcbRecvLength = %ld\n", pcbRecvLength);

  inizializza_array(pbRecvBuffer,(int)sizeof(pbRecvBuffer));

  rv = SCardTransmit(hCard, SCARD_PCI_T0, pbSendBuffer, dwSendLength,
		     &pioRecvPci, pbRecvBuffer, &pcbRecvLength);

  DBG("\nDEBUG: pcbRecvLength = %ld\n", pcbRecvLength);
  DBG("\nDEBUG: rv = %ld\n", rv);
  //printf("\nDEBUG: pcbRecvLength = %ld\n", pcbRecvLength);
  //printf("\nDEBUG: rv = %ld\n", rv);

  switch(rv) {
  case SCARD_E_NOT_TRANSACTED: {
    perror(strcat("gsm_verify_chv: ", ERR_APDU_EXCHANGE_FAILED));
    /*printf("Summary of input data:\n");
    printf("SENT APDU GET RESPONSE: ");
    print_array(pbSendBuffer,(int)sizeof(pbSendBuffer));
    printf("\nLength of sent APDU GET RESPONSE: %ld\n", dwSendLength);
    printf("RECEIVED RESPONSE: ");
    print_array(pbRecvBuffer,pcbRecvLength);
    printf("\n");
    printf("Length of received response: %ld.\n", pcbRecvLength);*/

    DBG("Summary of input data:\n");
    DBG("SENT APDU GET RESPONSE: ");
    #ifdef DEBUG
    print_array(pbSendBuffer,(int)sizeof(pbSendBuffer));
    #endif
    DBG("\nLength of sent APDU GET RESPONSE: %ld\n", dwSendLength);
    DBG("RECEIVED RESPONSE: ");
    #ifdef DEBUG
    print_array(pbRecvBuffer,pcbRecvLength);
    #endif
    DBG("\n");
    DBG("Length of received response: %ld.\n", pcbRecvLength);
    exit(EXIT_FAILURE);
  }
  case SCARD_E_INVALID_VALUE: {
    perror(strcat("gsm_verify_chv: ", ERR_INVALID_PARAMETER_VALUE));
    exit(EXIT_FAILURE);
  }
  case SCARD_E_READER_UNAVAILABLE: {
    perror(strcat("gsm_verify_chv: ", ERR_READER_UNAVAILABLE));
    exit(EXIT_FAILURE);
  }
  case SCARD_E_PROTO_MISMATCH: {
    perror(strcat("gsm_verify_chv: ", ERR_PROTOCOL_DIFFERENT));
    exit(EXIT_FAILURE);
  }
  case SCARD_W_RESET_CARD: {
    perror(strcat("gsm_verify_chv: ", ERR_CARD_RESET));
    exit(EXIT_FAILURE);
  }
  case SCARD_W_REMOVED_CARD: {
    perror(strcat("gsm_verify_chv: ", ERR_CARD_REMOVED));
    exit(EXIT_FAILURE);
  }
  case SCARD_E_INVALID_HANDLE: {
    perror(strcat("gsm_verify_chv: ", ERR_INVALID_CONTEXT_HANDLER));
    exit(EXIT_FAILURE);
  }
  case SCARD_S_SUCCESS: {
    /*printf("APDU VERIFY CHV successfully transmitted.\n");
    printf("Summary of input data:\n");
    printf("SENT APDU VERIFY CHV: ");
    print_array(pbSendBuffer,dwSendLength);
    printf("\nLength of sent APDU VERIFY CHV: %ld\n", dwSendLength);
    printf("RECEIVED RESPONSE: ");
    print_array(pbRecvBuffer,pcbRecvLength);
    printf("\n");
    printf("Length of received response: %ld.\n", pcbRecvLength);*/

    DBG("APDU VERIFY CHV successfully transmitted.\n");
    DBG("Summary of input data:\n");
    DBG("SENT APDU VERIFY CHV: ");
    #ifdef DEBUG
    print_array(pbSendBuffer,dwSendLength);
    #endif
    DBG("\nLength of sent APDU VERIFY CHV: %ld\n", dwSendLength);
    DBG("RECEIVED RESPONSE: ");
    #ifdef DEBUG
    print_array(pbRecvBuffer,pcbRecvLength);
    #endif
    DBG("\n");
    DBG("Length of received response: %ld.\n", pcbRecvLength);
    getchar();
    getchar();
    break;
  }
  default: {
    perror(strcat("gsm_verify_chv: ", ERR_WRONG_RETURN_VALUE));
    //printf("The value returned is: %ld",rv);
    DBG("The value returned is: %ld",rv);
    exit(EXIT_FAILURE);
  }
  }

  for (i=0; i<pcbRecvLength; i++) {
    blist_add_element(response, pbRecvBuffer[i]);
  }

  //printf("\nDEBUG: EXIT gsm_verify_chv.\n");
  DBG("\nDEBUG: EXIT gsm_verify_chv.\n");

  return(response);
}



//
// FUNCTION NAME: gsm_transparent_file_readability
//
// DESCRIPTION: this is a wrapper for pcsclite SCardTransmit function, arranged
//              to transmit a READ BINARY command to the card so that it is
//              possible know if the selected binary file's body can be read
//              with current authorization.
//
// INPUT PARAMETERS:
//   SCARDHANDLE hCard                     handle for a connection to the card.
//   SCARD_IO_REQUEST* dwActiveProtocol    pointer to the pcsclite defined
//                                         structure that implement the desired
//                                         protocol.
//   BYTE block_dim                        length in byte of the block to read.
//
// OUTPUT PARAMETERS:  int
//   Returns a number that codes the status of readability of the file: 0 means
//   that the body is readable with current authorization, other values specify
//   different types of protection errors.
//
// NOTES:
//   None.
//
int gsm_transparent_file_readability (SCARDHANDLE hCard,
				      SCARD_IO_REQUEST* dwActiveProtocol,
				      BYTE block_dim) {

}

//
// FUNCTION NAME: gsm_read_transparent_file
//
// DESCRIPTION: this is a wrapper for pcsclite SCardTransmit function, arranged
//              to transmit a READ BINARY command to the card so that the
//              entire contents of the currently selected binary file can be
//              read.
//
// INPUT PARAMETERS:
//   SCARDHANDLE hCard                     handle for a connection to the card.
//   SCARD_IO_REQUEST* dwActiveProtocol    pointer to the pcsclite defined
//                                         structure that implement the desired
//                                         protocol.
//   BYTE block_dim                        length in byte of the block to read.
//
// OUTPUT PARAMETERS:  RESPONSE*
//   Returns a pointer to the raw content of the specified file.
//
// NOTES:
//   - For the pcsclite manual is not complete on values retuned by its API
//     functions, further cases may be included in the switch(rv) in the
//     future.
//   - Changes need to be implemented to manage transparent files bigger
//     than 256 bytes.
//
RESPONSE* gsm_read_transparent_file (SCARDHANDLE hCard,
				     SCARD_IO_REQUEST* dwActiveProtocol,
				     BYTE block_dim) {

  int i;
  LONG rv;
  RESPONSE* response = create_b_list();

  DWORD dwSendLength, pcbRecvLength;
  SCARD_IO_REQUEST pioRecvPci;

  //Offset is hardcoded to 0x0000 because we are only interested in the
  //reading of the entire file
  BYTE pbSendBuffer[5] = {GSM_CLASS, READ_BINARY, 0, 0, block_dim};
  BYTE pbRecvBuffer[block_dim+2]; //SW1 e SW2 included

  dwSendLength = sizeof(pbSendBuffer);
  pcbRecvLength = sizeof(pbRecvBuffer);

  //printf("\nDEBUG: pcbRecvLength = %ld\n", pcbRecvLength);
  DBG("\nDEBUG: pcbRecvLength = %ld\n", pcbRecvLength);

  inizializza_array(pbRecvBuffer,(int)sizeof(pbRecvBuffer));

  rv = SCardTransmit(hCard, SCARD_PCI_T0, pbSendBuffer, dwSendLength,
		     &pioRecvPci, pbRecvBuffer, &pcbRecvLength);
  switch(rv) {
  case SCARD_E_NOT_TRANSACTED: {
    perror(strcat("gsm_read_transparent_file: ", ERR_APDU_EXCHANGE_FAILED));
    /*printf("Summary of input data:\n");
    printf("SENT APDU GET RESPONSE: ");
    print_array(pbSendBuffer,(int)sizeof(pbSendBuffer));
    printf("\nLength of sent APDU GET RESPONSE: %ld\n", dwSendLength);
    printf("RECEIVED RESPONSE: ");
    print_array(pbRecvBuffer,pcbRecvLength);
    printf("\n");
    printf("Length of received response: %ld.\n", pcbRecvLength);*/

    DBG("Summary of input data:\n");
    DBG("SENT APDU GET RESPONSE: ");
    #ifdef DEBUG
    print_array(pbSendBuffer,(int)sizeof(pbSendBuffer));
    #endif
    DBG("\nLength of sent APDU GET RESPONSE: %ld\n", dwSendLength);
    DBG("RECEIVED RESPONSE: ");
    #ifdef DEBUG
    print_array(pbRecvBuffer,pcbRecvLength);
    #endif
    DBG("\n");
    DBG("Length of received response: %ld.\n", pcbRecvLength);
    exit(EXIT_FAILURE);
  }
  case SCARD_E_INVALID_VALUE: {
    perror(strcat("gsm_read_transparent_file: ", ERR_INVALID_PARAMETER_VALUE));
    exit(EXIT_FAILURE);
  }
  case SCARD_E_READER_UNAVAILABLE: {
    perror(strcat("gsm_read_transparent_file: ", ERR_READER_UNAVAILABLE));
    exit(EXIT_FAILURE);
  }
  case SCARD_E_PROTO_MISMATCH: {
    perror(strcat("gsm_read_transparent_file: ", ERR_PROTOCOL_DIFFERENT));
    exit(EXIT_FAILURE);
  }
  case SCARD_W_RESET_CARD: {
    perror(strcat("gsm_read_transparent_file: ", ERR_CARD_RESET));
    exit(EXIT_FAILURE);
  }
  case SCARD_W_REMOVED_CARD: {
    perror(strcat("gsm_read_transparent_file: ", ERR_CARD_REMOVED));
    exit(EXIT_FAILURE);
  }
  case SCARD_E_INVALID_HANDLE: {
    perror(strcat("gsm_read_transparent_file: ", ERR_INVALID_CONTEXT_HANDLER));
    exit(EXIT_FAILURE);
  }
  case SCARD_S_SUCCESS: {
    /*printf("APDU GET RESPONSE successfully transmitted.\n");
    printf("Summary of input data:\n");
    printf("SENT APDU GET RESPONSE: ");
    print_array(pbSendBuffer,dwSendLength);
    printf("\nLength of sent APDU GET RESPONSE: %ld\n", dwSendLength);
    printf("RECEIVED RESPONSE: ");
    print_array(pbRecvBuffer,pcbRecvLength);
    printf("\n");
    printf("Length of received response: %ld.\n", pcbRecvLength);*/

    DBG("APDU GET RESPONSE successfully transmitted.\n");
    DBG("Summary of input data:\n");
    DBG("SENT APDU GET RESPONSE: ");
    #ifdef DEBUG
    print_array(pbSendBuffer,dwSendLength);
    #endif
    DBG("\nLength of sent APDU GET RESPONSE: %ld\n", dwSendLength);
    DBG("RECEIVED RESPONSE: ");
    #ifdef DEBUG
    print_array(pbRecvBuffer,pcbRecvLength);
    #endif
    DBG("\n");
    DBG("Length of received response: %ld.\n", pcbRecvLength);
    break;
  }
  default: {
    perror(strcat("gsm_read_transparent_file: ", ERR_WRONG_RETURN_VALUE));
    //printf("The value returned is: %ld",rv);
    DBG("The value returned is: %ld",rv);
    exit(EXIT_FAILURE);
  }
  }

  for (i=0; i<pcbRecvLength; i++) {
    blist_add_element(response, pbRecvBuffer[i]);
  }

  return(response);
}



//
// FUNCTION NAME: gsm_read_linear_fixed_file
//
// DESCRIPTION: this is a wrapper for pcsclite SCardTransmit function, arranged
//              to transmit READ RECORD commands to the card so that the entire
//              contents of the currently selected linear fixed file can be
//              read.
//
// INPUT PARAMETERS:
//   SCARDHANDLE hCard                     handle for a connection to the card.
//   SCARD_IO_REQUEST* dwActiveProtocol    pointer to the pcsclite defined
//                                         structure that implement the desired
//                                         protocol.
//   int file_size                         file size in bytes. The number of
//                                         records present in the file equals
//                                         the file size divided by the length
//                                         of a single record.
//   BYTE record_dim                       length in byte of every record
//                                         stores into the file.
//
// OUTPUT PARAMETERS:  REC_RESPONSE*
//   Returns a pointer to the raw content of the specified file.
//
// NOTES:
//   - For the pcsclite manual is not complete on values retuned by its API
//     functions, further cases may be included in the switch(rv) in the
//     future.
//   - Changes need to be implemented to manage transparent files bigger
//     than 256 bytes.
//
REC_RESPONSE* gsm_read_linear_fixed_file (SCARDHANDLE hCard,
				      SCARD_IO_REQUEST* dwActiveProtocol,
				      int file_size,
				      BYTE record_dim) {

  int i;
  REC_RESPONSE* response = create_rec_list();
  int block_num = file_size % record_dim; //In this statement block_num is used
                                          //as a support variabile to trace
                                          //erroneous situations

  if(block_num != 0){
    //printf("DEBUG: gsm_read_linear_fixed_file: file_size/record_size mismatch.");
    DBG("DEBUG: gsm_read_linear_fixed_file: file_size/record_size mismatch.");
  }
  else{
    block_num = file_size/record_dim;
  }
  // Now that we now that the file is composed of block_num records, we have to
  // call gsm_read_record block_num times to get the whole content of the file.

  // Records are extracted from number 1 to last record.
  for(i=1; i<=block_num; i++) {
    // Records are added to the list one at a time. Here the GSM primitive
    // READ RECORD is invoked with mode=0x04 that is with an absolute reference
    // to the record number
    reclist_add_element(response,
			gsm_read_record(hCard, dwActiveProtocol,
                                        i, 0x04, record_dim));
  }

  return(response);
}


//
// FUNCTION NAME: gsm_read_cyclic_file
//
// DESCRIPTION: this is a wrapper for pcsclite SCardTransmit function, arranged
//              to transmit READ RECORD commands to the card so that the entire
//              contents of the currently selected cyclic file can be read.
//
// INPUT PARAMETERS:
//   SCARDHANDLE hCard                     handle for a connection to the card.
//   SCARD_IO_REQUEST* dwActiveProtocol    pointer to the pcsclite defined
//                                         structure that implement the desired
//                                         protocol.
//   int file_size                         file size in bytes. The number of
//                                         records present in the file equals
//                                         the file size divided by the length
//                                         of a single record.
//   BYTE record_dim                       length in byte of every record
//                                         stores into the file.
//
// OUTPUT PARAMETERS:  REC_RESPONSE*
//   Returns a pointer to the raw content of the specified file.
//
// NOTES:
//   - For the pcsclite manual is not complete on values retuned by its API
//     functions, further cases may be included in the switch(rv) in the
//     future.
//   - The output is arranged such that the first record of the list is the
//     newest (most recently written) one and the last record of the list is
//     the oldest (least recently written) one.
//
REC_RESPONSE* gsm_read_cyclic_file (SCARDHANDLE hCard,
				    SCARD_IO_REQUEST* dwActiveProtocol,
				    int file_size,
				    BYTE record_dim) {

  int i;
  REC_RESPONSE* response = create_rec_list();
  int block_num = file_size % record_dim; //In this statement block_num is used
                                          //as a support variabile to trace
                                          //erroneous situations

  if(block_num != 0){
    //printf("DEBUG: gsm_read_linear_fixed_file: file_size/record_size mismatch.");
    DBG("DEBUG: gsm_read_linear_fixed_file: file_size/record_size mismatch.");
  }
  else{
    block_num = file_size/record_dim;
  }
  // Now that we now that the file is composed of block_num records, we have to
  // call gsm_read_record block_num times to get the whole content of the file.

  // Records are extracted from last written one to least recently written one.
  for(i=0; i<block_num; i++) {
    // Records are added to the list one at a time. Here the GSM primitive
    // READ RECORD is invoked with mode=0x03 that is "previous mode": this way
    // the primitive returns last record written before the current one.
    // The third parameter of gsm_read_record is useless here; anyway
    // standards state that, in this case, his value must be forced to zero.
    reclist_add_element(response,
			gsm_read_record(hCard, dwActiveProtocol,
                                        0x00, 0x03, record_dim));
  }

  return(response);
}



//
// FUNCTION NAME: gsm_read_record
//
// DESCRIPTION: this is a wrapper for pcsclite SCardTransmit function, arranged
//              to transmit READ RECORD commands to the card so that the
//              specified record of a linear-fixed file can be read.
//
// INPUT PARAMETERS:
//   SCARDHANDLE hCard                     handle for a connection to the card.
//   SCARD_IO_REQUEST* dwActiveProtocol    pointer to the pcsclite defined
//                                         structure that implement the desired
//                                         protocol.
//
//   BYTE rec_number                       progressive number of the record to
//                                         be read.
//   BYTE mode                             mode of operation of the READ RECORD
//                                         primitive
//   BYTE rec_dim                          length in byte of the block to read.
//
// OUTPUT PARAMETERS:  RESPONSE*
//   Returns a pointer to the raw content of the specified record.
//
// NOTES:
//   - For the pcsclite manual is not complete on values retuned by its API
//     functions, further cases may be included in the switch(rv) in the
//     future.
//
RESPONSE* gsm_read_record (SCARDHANDLE hCard,
			   SCARD_IO_REQUEST* dwActiveProtocol,
			   BYTE rec_number,
			   BYTE mode,
			   BYTE rec_dim) {

  int i;
  LONG rv;
  RESPONSE* response = create_b_list();

  DWORD dwSendLength, pcbRecvLength;
  SCARD_IO_REQUEST pioRecvPci;

  BYTE pbSendBuffer[5] = {GSM_CLASS, READ_RECORD, rec_number, mode, rec_dim};
  BYTE pbRecvBuffer[rec_dim+2]; //SW1 e SW2 included

  dwSendLength = sizeof(pbSendBuffer);
  pcbRecvLength = sizeof(pbRecvBuffer);

  inizializza_array(pbRecvBuffer,(int)sizeof(pbRecvBuffer));

  rv = SCardTransmit(hCard, SCARD_PCI_T0, pbSendBuffer, dwSendLength,
		     &pioRecvPci, pbRecvBuffer, &pcbRecvLength);
  switch(rv) {
  case SCARD_E_NOT_TRANSACTED: {
    perror(strcat("gsm_read_record: ", ERR_APDU_EXCHANGE_FAILED));
    /*printf("Summary of input data:\n");
    printf("SENT APDU GET RESPONSE: ");
    print_array(pbSendBuffer,(int)sizeof(pbSendBuffer));
    printf("\nLength of sent APDU GET RESPONSE: %ld\n", dwSendLength);
    printf("RECEIVED RESPONSE: ");
    print_array(pbRecvBuffer,pcbRecvLength);
    printf("\n");
    printf("Length of received response: %ld.\n", pcbRecvLength);*/

    DBG("Summary of input data:\n");
    DBG("SENT APDU GET RESPONSE: ");
    #ifdef DEBUG
    print_array(pbSendBuffer,(int)sizeof(pbSendBuffer));
    #endif
    DBG("\nLength of sent APDU GET RESPONSE: %ld\n", dwSendLength);
    DBG("RECEIVED RESPONSE: ");
    #ifdef DEBUG
    print_array(pbRecvBuffer,pcbRecvLength);
    #endif
    DBG("\n");
    DBG("Length of received response: %ld.\n", pcbRecvLength);
    exit(EXIT_FAILURE);
  }
  case SCARD_E_INVALID_VALUE: {
    perror(strcat("gsm_read_record: ", ERR_INVALID_PARAMETER_VALUE));
    exit(EXIT_FAILURE);
  }
  case SCARD_E_READER_UNAVAILABLE: {
    perror(strcat("gsm_read_record: ", ERR_READER_UNAVAILABLE));
    exit(EXIT_FAILURE);
  }
  case SCARD_E_PROTO_MISMATCH: {
    perror(strcat("gsm_read_record: ", ERR_PROTOCOL_DIFFERENT));
    exit(EXIT_FAILURE);
  }
  case SCARD_W_RESET_CARD: {
    perror(strcat("gsm_read_record: ", ERR_CARD_RESET));
    exit(EXIT_FAILURE);
  }
  case SCARD_W_REMOVED_CARD: {
    perror(strcat("gsm_read_record: ", ERR_CARD_REMOVED));
    exit(EXIT_FAILURE);
  }
  case SCARD_E_INVALID_HANDLE: {
    perror(strcat("gsm_read_record: ", ERR_INVALID_CONTEXT_HANDLER));
    exit(EXIT_FAILURE);
  }
  case SCARD_S_SUCCESS: {
    /*
      //DEBUG MESSAGES
    printf("APDU GET RESPONSE successfully transmitted.\n");
    printf("Summary of input data:\n");
    printf("SENT APDU GET RESPONSE: ");
    print_array(pbSendBuffer,dwSendLength);
    printf("\nLength of sent APDU GET RESPONSE: %i\n", dwSendLength);
    printf("RECEIVED RESPONSE: ");
    print_array(pbRecvBuffer,pcbRecvLength);
    printf("\n");
    printf("Length of received response: %i.\n", pcbRecvLength);
    */
    break;
  }
  default: {
    perror(strcat("gsm_read_record: ", ERR_WRONG_RETURN_VALUE));
    //printf("The value returned is: %ld",rv);
    DBG("The value returned is: %ld",rv);
    exit(EXIT_FAILURE);
  }
  }

  for (i=0; i<pcbRecvLength; i++) {
    blist_add_element(response, pbRecvBuffer[i]);
  }

  return(response);
}

