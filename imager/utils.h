/******************************************************************************
 *
 * NAME:          utils.h
 *
 * AUTHORS:       Fabio Casadei
 * 		  fabio.casadei@mercurio-ws.com
 *
 * 		  Antonio Savoldi
 * 		  antonio.savoldi@ing.unibs.it
 *
 * FILE TYPE:     Header file
 *
 * DESCRIPTION:   this file contains constant definitions and function
 *                prototypes for utils.c 
 *
 *****************************************************************************/
#include "winscard.h"


// PROGRAM-RELATED ERROR MESSAGES (PEB = Program Error or Bug)

#define PEB_WRONG_LIST_LENGTH "inconsistence in list length found."
#define PEB_NULL_INPUT_LIST "NULL input list."


//This structure is necessary to substitute variable byte array fields.
typedef struct BNODE BYTE_LIST_NODE;
struct BNODE {
  BYTE content;
  BYTE_LIST_NODE* next_b_element;
};

typedef struct {
  int b_list_length;
  BYTE_LIST_NODE* p_byte_list;
} BYTE_LIST;

typedef BYTE_LIST FILE_ID; // This definition takes into account future
                           // expansions of the identifier space to 3 or more
                           // bytes.
typedef BYTE_LIST RESPONSE;

typedef BYTE_LIST RECORD;

//This structure is necessary to represent linear-fixed files.
typedef struct RECNODE REC_LIST_NODE;
struct RECNODE {
  RECORD* content;
  REC_LIST_NODE* next_rec_element;
};

typedef struct {
  int rec_list_length;
  REC_LIST_NODE* p_rec_list;
} REC_LIST;

typedef REC_LIST REC_RESPONSE;

typedef BYTE* OFFSET;


//
// FUNCTION PROTOTYPES
//

void send_warning(char* warn_msg);

   //
   // ARRAY-SPECIFIC FUNCTIONS: init_array and print_array
   //

int inizializza_array(BYTE* nome_array, int lunghezza_array);

int print_array(BYTE array_name[], int array_length);


   //
   // BYTE_LIST-SPECIFIC FUNCTIONS: create_b_list, blist_add_element, 
   //                               print_b_list, blist_get_element
   //

BYTE_LIST* create_b_list();

int blist_add_element(BYTE_LIST* list, BYTE new_content);

int print_b_list(BYTE_LIST* to_print);

BYTE blist_get_element(BYTE_LIST* list, int position);

int blist_set_element(BYTE_LIST* list, int position, BYTE new_content);


   //
   // RECORD-SPECIFIC FUNCTIONS: create_rec_list, reclist_add_element, 
   //                            print_rec_list, reclist_get_element
   //

REC_LIST* create_rec_list();

int reclist_add_element(REC_LIST* list, RECORD* new_content);

int print_rec_list(REC_LIST* to_print);

RECORD* reclist_get_element(REC_LIST* list, int position);

int reclist_set_element(REC_LIST* list, int position, RECORD* new_content);
