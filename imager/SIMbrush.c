/******************************************************************************
 *
 * NAME:          SIMbrush.c
 *
 * AUTHORS:       Fabio Casadei
 * 		  fabio.casadei@mercurio-ws.com
 *
 * 		  Antonio Savoldi
 * 		  antonio.savoldi@ing.unibs.it
 *
 * FILE TYPE:     C source
 *
 * DESCRIPTION:   this is the file where function main resides.
 *
 *****************************************************************************/
 /*
 usage: ./simbrush -o fileName.xml
		   -c => Redistribution
		   -w => Warranty
		   -h => Help file
		   -v => Verbose mode (default mode ON)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "winscard.h"
#include "utils.h"
#include "gsm-constants.h"
#include "pcsc-wrappers.h"
#include "dirtree.h"
#include "xml-results.h"
#include "getopt.h"
#include "debug.h"
#include "md5.h"

#define active_protocol SCARD_PCI_T0 // T0 is the protocol used by SIM/USIM cards with PC/SC reader compliant



int  dumping_mode;      // '0' => FULL ACQUISITION OF SIM CARD
                        // '1' => PARTIAL ACQUISITION OF SIM CARD
int print_auth_code_status(SCARDHANDLE hCard, SCARD_IO_REQUEST* dwActiveProtocol);
int chv1_verification_ok(SCARDHANDLE hCard, SCARD_IO_REQUEST* dwActiveProtocol);

int main (int argc, char **argv) {

  // This variable is used only in the demo code
  RESPONSE* card_resp = malloc(sizeof(BYTE_LIST));

  int app_var;
  int chv1_verified = 0;
  int dont_verify_chv1 = 0;
  time_t t1, t2;
  char *outFileName, ch;

//##
//##   VARIABLES DEFINITION
//##

  SIM_CARD* seized_card; //It will contain seized SIM card informations

  RESPONSE* atr = malloc(sizeof(RESPONSE)); //It will contain the ATR
                                            //from the card.

  //Connection context
  SCARDCONTEXT hContext;

  //List of available readers
  LPSTR mszReaders;

  //Connection handler
  SCARDHANDLE hCard;

  //File that will store the xml formatted results
  FILE* xml_output_file;

  //Debugging
  //dbg_enable(DBG_ON);



//##
//##   1ST SECTION: PARSING THE COMMAND LINE PARAMETERS
//##

///////////////////
// Argument parsing
// parse command line args
if(argc < 2){
  printf(" -> Missing parameters \n");
  printf(" -> Usage: \n");
  printf(" ./SIMbrush -[option] fileName.xml \n");
  printf(" ./SIMbrush -c: Redistribution of the software \n");
  printf(" ./SIMbrush -w: Warranty \n");
  printf(" ./SIMbrush -o fileName.xml: save the image in the file 'fileName.xml' \n");
  printf(" ./SIMbrush -h: Displays the help \n");
  return 1;
}

while( (ch = getopt(argc, argv, "-cwoh" )) >=0 )
{
    //printf("DEBUG: %c\n", ch);
    switch(ch)
    {
    case 'c':
        printf("\n -> Redistribution\n");
        printf("    Please visit http://www.gnu.org/licenses/gpl.html sections 2 through 7\n");
        return 1;

    case 'w':
        printf("\n -> Warranty\n");
        printf("    Please visit http://www.gnu.org/licenses/gpl.html sections 15 and 16\n");
        return 1;

    case 'o':
       outFileName = argv[2];
       if(outFileName == NULL){
         printf("\n fileName is missing... \n");
         return 1;
       } else {
         printf("fileName = %s \n",outFileName);
       }
       goto OK;

    //case 'v':
    //   printf("\n -> Verbose mode activated: all the logs are displayed; Turned On when dumping the Full Card");
    //   fLog = TRUE;
    //   break;

    case 'h':
    default:
       printf(" -> Usage: \n");
       printf(" ./SIMbrush -[option] fileName.xml \n");
       printf(" ./SIMbrush -c: Redistribution of the software \n");
       printf(" ./SIMbrush -w: Warranty \n");
       printf(" ./SIMbrush -o fileName.xml: save the image in the file 'fileName.xml' \n");
       printf(" ./SIMbrush -h: Displays the help \n");
       return 1;

    }
}

//Exit from the switch statement as soon as the fileName is defined
OK:

//##
//##   2ND SECTION: SIM CARD READER(S) SETUP AND CONNECTION(S) SETUP
//##
  
  system("clear");
  printf("SimBrush collection module v0.0.1\n");	
  hContext = SIM_Establish_Context(SCARD_SCOPE_SYSTEM);
  mszReaders = Detect_Readers(hContext);

  //This call functions only when there is only one active reader. To
  //activate the scan on every reader attached to the system, it is
  //necessary to parse mszReaders.
  hCard = SIM_Connect(hContext,mszReaders);
  printf("\nPress a key to see card status...\n");
  getchar();
  atr = SIM_Status(hCard);
  SIM_Begin_Transaction(hCard);

//##
//##   3RD SECTION: PART A - AUTHENTICATION
//##

  printf("\n\nPress a key to access the authentication session...\n\n");
  getchar();

  if(print_auth_code_status(hCard, active_protocol)) {
      while(!chv1_verified && !dont_verify_chv1) {
          printf("\n");
      	  printf("\n############################################################\n");
      	  printf("CHV1 ON THE CARD IS ENABLED. WHAT DO YOU WANT TO DO ?\n");
      	  printf("\tType 1 to go trough CHV1 verification\n");
      	  printf("\tType 2 to go ahead without CHV1 verification (only content of files with access condition equal to ALW will be extracted)\n");
      	  printf("\tType 3 to exit");
      	  printf("\n############################################################\n");
      	  scanf("%i",&app_var);
      	  switch(app_var) {
      	      case 1: {
	          chv1_verified = chv1_verification_ok(hCard, active_protocol);
		  system("clear");
		  printf("CHV1 succesfully verified\n");
		  break;
	      }
      	      case 2: {
	    	  dont_verify_chv1 = 1;
		  exit(EXIT_FAILURE);     	      
	      }
      	      case 3: {
	    	  exit(EXIT_SUCCESS);
	    	  break;
      	      }
      	      default: {
		  printf("\nWrong number...\n");
		  exit(EXIT_SUCCESS);
	     }	
      	  } // switch()
      } // while()
  } // if()


//##
//##   3RD SECTION: PART B - FILES AND DIRECTORY TREE EXTRACTION
//##

  printf("\n\nPress a key to extract card directory tree...\n\n");
  getchar();

  printf("\n\n");
  printf("*******************************************************************************\n");
  printf("*                                                                             *\n");
  printf("* EXTRACTION OF FILES AND DIRECTORIES REACHABLE FROM MASTER FILE UNDER WAY... *\n");
  printf("*                                                                             *\n");
  printf("*******************************************************************************\n");
  printf("\n\n");

  printf("\nDEBUG: entering create_directory_tree...\n");

  printf("\n\nSelect Acquisition mode for SIMbrush tool... \n\n");
  printf("\t'0' for FULL acquisition: n.b. Be carefull!!! Full acquisition mode allows to get a full, complete image of the SIM card. The process\n \
          is time consuming and depends on the number of DF (dedicated files) within the file system (e.g. 7 DFs implies that the collection time\n \
          is about 7xK = 140 minutes, where K=20 minutes is the average time to brush a full DF folder).\n\n");
  printf("\t'1' for PARTIAL acquisition: the collection time is less than two minutes. Every well-known file is accessed directly.\n\n");
  scanf("%d", &dumping_mode);

  // dumping_mode = 0 ==> FULL ACQUISITION
  // dumping_mode = 1 ==> PARTIAL ACQUISITION
  
  printf("\nDumping in progress...wait...\n");	

  (void) time(&t1); // Starting time
  seized_card = create_SIM_card(hCard, active_protocol);
  (*seized_card).raw_atr = atr;
  (void) time(&t2); // Ending time

  printf("\nExtraction time is %d minute, that is %d second \n",
        (int)((t2-t1)/60), (int)(t2-t1) );

//##
//##   4TH SECTION: XML FORMATTING AND OUTPUT OF THE RESULTS
//##
  //The file should be opened at the beginning of the extraction process
  xml_output_file = fopen(outFileName, "w");
  if(xml_output_file == NULL) {
    	printf("DEBUG: cannot open xml results file.");
    	exit(1);
  }

  create_xml_results(xml_output_file, seized_card);
  fclose(xml_output_file);

//## Hash file calculation

  MDFile(outFileName);

//##
//##   5TH SECTION: CARD(S) DISCONNECTION
//##

  SIM_End_Transaction(hCard);
  SIM_Disconnect(hCard);
  SIM_Release_Context(hContext);

}// int main()



//
// FUNCTION NAME: print_auth_codes_status
//
// DESCRIPTION: this function print on the screen the status of the
//              authentication codes of the sim card.
//
// INPUT PARAMETERS:
//   None.
//
// OUTPUT PARAMETERS: int
//   Returns 1 if CHV1 is enabled, 0 otherwise.
//
int print_auth_code_status(SCARDHANDLE hCard,
			   SCARD_IO_REQUEST* dwActiveProtocol) {

  int app;
  RESPONSE* resp;

  //Get card CHV abilitation status
  resp = gsm_select(hCard, dwActiveProtocol,0x3F00);
  if(blist_get_element(resp,-2) == SW1_CORRECT_WITH_RESPONSE) {
    	resp = gsm_get_response(hCard, dwActiveProtocol, blist_get_element(resp,-1));
    	printf("\n\n");
    	printf("*****************************************\n");
    	printf("*                                       *\n");
    	printf("*   AUTHENTICATION STATUS OF THE CARD   *\n");
    	printf("*                                       *\n");
    	printf("******************************************\n");
    	printf("\n");

    	printf("NUMBER OF CHVs, UNBLOCK CHVs AND ADMINISTRATIVE CODES\t=\t%i\n",
		   blist_get_element(resp,16));

    	printf("CHV1 STATUS (PIN1)\t\t=\t");
    	app = 128; //binary value of this mask = 10000000
    	if(blist_get_element(resp,18) & app)
		printf("INITIALIZED,\t");
    	else
		printf("DEACTIVATED,\t");
    	app = 15; //binary value of this mask = 00001111
    	printf("TRIES LEFT = %i\n", blist_get_element(resp,18) & app);

    	printf("UNBLOCK CHV1 STATUS (PUK1)\t=\t");
    	app = 128; //binary value of this mask = 10000000
    	if(blist_get_element(resp,19) & app)
		printf("INITIALIZED,\t");
    	else
		printf("DEACTIVATED,\t");
    	app = 15; //binary value of this mask = 00001111
    	printf("TRIES LEFT = %i\n", blist_get_element(resp,19) & app);

    	printf("CHV2 STATUS (PIN2)\t\t=\t");
    	app = 128; //binary value of this mask = 10000000
    	if(blist_get_element(resp,20) & app)
		printf("INITIALIZED,\t");
    	else
		printf("DEACTIVATED,\t");
    	app = 15; //binary value of this mask = 00001111
    	printf("TRIES LEFT = %i\n", blist_get_element(resp,20) & app);

    	printf("UNBLOCK CHV2 STATUS (PUK2)\t=\t");
    	app = 128; //binary value of this mask = 10000000
    	if(blist_get_element(resp,21) & app)
		printf("INITIALIZED,\t");
    	else
		printf("DEACTIVATED,\t");
    	app = 15; //binary value of this mask = 00001111
    	printf("TRIES LEFT = %i\n", blist_get_element(resp,21) & app);

    	printf("CHV1 abilitation state is:\t");
    	app = 128; //binary value of this mask = 10000000
    	if(blist_get_element(resp,13) & app) {
      		printf("DISABLED\n");
      		return(0);
    	}
    	else {
     		printf("ENABLED");
      		return(1);
    	}
  }
  else {
    	perror("IMPOSSIBLE TO SELECT MF: card damaged, not GSM, badly inserted into the reader or not supported.");
    	exit(EXIT_FAILURE);
  }

}





//
// FUNCTION NAME: chv1_verification_ok
//
// DESCRIPTION: this function get CHV1 from the user and send it to the card
//              for verification.
//
// INPUT PARAMETERS:
//   None.
//
// OUTPUT PARAMETERS: RESPONSE*
//   Returns 1 if verification is successfull, 0 otherwise.
//
// NOTES:
//   - This function verifies exclusively CHV1. If CHV2 verification will be
//     needed in the future, then the function could be easily changed.
//
int chv1_verification_ok(SCARDHANDLE hCard, SCARD_IO_REQUEST* dwActiveProtocol) {

  int i;
  char app_char, *pin;
  BYTE app;

  BYTE_LIST* chv1 = create_b_list();
  RESPONSE* sim_response;

  printf("\n");
  printf("INSERT THE PIN1 CODE (ex. 1234)\n");
  printf("WARNING: DON'T GO AHEAD IF YOU DON'T REALLY KNOW THE CODE!\n");
  printf("THE CARD WILL BE BLOCKED AFTER 3 TRIES!!!\n");

  //PIN CODING:
  //The coding for the PIN1 is determined by hexadecimal representation of
  //pin's number with '0xff' padding byte; for example the pin
  //'1234' in decimal notation will be converted in '31323334ffffffff'
  
  for(i=0; i<8; i++) {
  	blist_add_element(chv1, 0xFF);
  }

  pin = (char *) malloc(10 * sizeof (char)); // PIN code can be 4 or 6 ?
  printf("Insert CHV1 code (PIN1): ");
  scanf("%s", pin);

  for(i=0;i<strlen(pin);i++){
	app = pin[i]; 			// PIN is converted into hexadecimal representation (i.e. '0'->'0x30')
  	blist_set_element(chv1,i,app);  
  }

  getchar();

  printf("\nThe codified CHV1 is: ");
  for(i=0; i<8; i++) {
    	printf("%.2X",blist_get_element(chv1,i));
  }
  app_char = 'X';
  
  printf("\nAre you sure you want to continue (y/n)?\n");
  scanf("%c", &app_char);
  
  if (app_char == 'Y' || app_char == 'y') {
  	sim_response = gsm_verify_chv(hCard, dwActiveProtocol, chv1);
  	if((blist_get_element(sim_response,-2) == 0x90) && (blist_get_element(sim_response,-1) == 0x00)) {
  		printf("CHV1 SUCCESSFULLY VERIFIED.\n");
		return 1; //return 1 if the CHV1 is verified
  	}
  	else {
  		printf("CHV1 VERIFICATION FAILED.\n");
  		print_auth_code_status(hCard,dwActiveProtocol);
		exit(EXIT_FAILURE);
		//return 0; //return 0 if the CHV1 is not verified
  	}
  } else if (app_char != 'Y' || app_char != 'y'){
       // the user is not sure about the code => exit from the program
       	printf("Exiting program...\n");
       	exit(EXIT_FAILURE);
  } 

}
