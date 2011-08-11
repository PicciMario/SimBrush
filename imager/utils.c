/******************************************************************************
 *
 * NAME:          utils.c
 *
 * AUTHORS:       Fabio Casadei
 * 		  fabio.casadei@mercurio-ws.com
 *
 * 		  Antonio Savoldi
 * 		  antonio.savoldi@ing.unibs.it
 *
 * FILE TYPE:     C source file
 *
 * DESCRIPTION:   this file contains utility functions that are used widely
 *                by other modules.
 *
 *****************************************************************************/
#include <string.h>
#include <stdio.h>
#include "stdlib.h"
#include "winscard.h"
#include "utils.h"
#include "debug.h"


void send_warning(char* warn_msg) {

  printf("\nWARNING: %s.\n", warn_msg);

}



/****************************************************************
 *                                                              *
 *     ARRAY-SPECIFIC FUNCTIONS: init_array and print_array     *
 *                                                              *
 ****************************************************************/


int inizializza_array(BYTE* nome_array, int lunghezza_array) {

  int i;

  for (i=0; i<lunghezza_array; i++) {
    	nome_array[i] = 0xFF;
  }
}


//
// FUNCTION NAME: print_array
//
// DESCRIPTION: prints on the standard output the elements of an array of
//              bytes.
//
// INPUT PARAMETERS:
//   BYTE array_name[]          the array to print.
//   int array_length           lenght of the array to print.
//
// OUTPUT PARAMETERS: int
//   0 if the computation finishes without errors.
//   1 otherwise.
//
// NOTES:
//   - Controls over parameters to be implemented.
//   - Bound checking is responsibility of the calling function.
//
int print_array(BYTE array_name[], int array_length) {

  int i;

  //printf("\nDEBUG: array_name length: %i\n", lunghezza_array);

  for (i=0; i<array_length; i++) {
    	printf("%.2X ", array_name[i]);
  }

  return(0);
}



/***************************************************************************
 *                                                                         *
 *     BYTE_LIST-SPECIFIC FUNCTIONS: create_b_list, blist_add_element,     *
 *                                   print_b_list, blist_get_element       *
 *                                                                         *
 ***************************************************************************/

//
// FUNCTION NAME: create_b_list
//
// DESCRIPTION: creates a new list of bytes.
//
// INPUT PARAMETERS:
//   none.
//
// OUTPUT PARAMETERS: BYTE_LIST*
//   The newly created list.
//
// NOTES:
//   none.
//
BYTE_LIST* create_b_list() {

  BYTE_LIST* b_node = malloc(sizeof(BYTE_LIST));

  (*b_node).b_list_length = 0;
  (*b_node).p_byte_list = NULL;

  return(b_node);
}


//
// FUNCTION NAME: blist_add_element
//
// DESCRIPTION: add the specified byte in a new element at the end of
//              the specified pointer list of bytes.
//
// INPUT PARAMETERS:
//   BYTE_LIST* list          the list to expand.
//   BYTE new_content         the new byte to add.
//
// OUTPUT PARAMETERS: int
//   0 if the computation finishes without errors.
//   1 otherwise.
//
// NOTES:
//   - Controls over parameters to be tested.
//
int blist_add_element(BYTE_LIST* list, BYTE new_content) {

  BYTE_LIST_NODE* p_aux = malloc(sizeof(BYTE_LIST_NODE));
  BYTE_LIST_NODE* p_index;
  int control = 0;

  (*p_aux).content = new_content;
  (*p_aux).next_b_element = NULL;

  if (list == NULL) {
    	perror(strcat("blist_add_element: ", PEB_NULL_INPUT_LIST));
    	exit(EXIT_FAILURE);
  } else if ((*list).p_byte_list == NULL) {
    	(*list).p_byte_list = p_aux;
    	(*list).b_list_length = 1;
    	return(0);
  } else {
    	control = 1;
    	for (p_index = (*list).p_byte_list; (*p_index).next_b_element!=NULL ;
	 	p_index = (*p_index).next_b_element) {
    		control++;
    	}
  }

  //Control on list length
  if ((*list).b_list_length != control) {
    	perror(strcat("blist_add_element: ", PEB_WRONG_LIST_LENGTH));
    	exit(EXIT_FAILURE);
  }

  //At this point, p_index points to the last element of current list.
  (*p_index).next_b_element = p_aux;
  (*list).b_list_length = ++control;

  return(0);
}


//
// FUNCTION NAME: print_b_list
//
// DESCRIPTION: prints in a row all the bytes forming the list given as
//              parameter.
//
// INPUT PARAMETERS:
//   BYTE_LIST* to_print      the list to print.
//
// OUTPUT PARAMETERS: int
//   0 if the computation finishes without errors.
//   1 otherwise.
//
// NOTES:
//   - Given the correctness of b_list_length, we use it as a control over the
//     unexpected end of the list.
//
int print_b_list(BYTE_LIST* to_print) {

  int i = 0;
  BYTE_LIST_NODE* p_aux;

  if (to_print == NULL) {
    	perror(strcat("print_b_list: ", PEB_NULL_INPUT_LIST));
    	exit(EXIT_FAILURE);
  } else if ((*to_print).p_byte_list == NULL) {
    	printf("print_b_list: this is an empty byte list!");
  } else {
    	for (p_aux=(*to_print).p_byte_list; p_aux!=NULL; p_aux=(*p_aux).next_b_element) {
      		printf("%.2X ", (*p_aux).content);
      		i++;
    	}
  }

  if (i != (*to_print).b_list_length) {
    	perror(strcat("print_b_list: ", PEB_WRONG_LIST_LENGTH));
    	exit(EXIT_FAILURE);
  }

  return(0);
}


//
// FUNCTION NAME: blist_get_element
//
// DESCRIPTION: gets an element out of a byte list.
//
// INPUT PARAMETERS:
//   BYTE_LIST* list       the list from where to extract an element
//   int position          indicates which element to extract: it works the
//                         same way as the index of an array (0 is the first
//                         element). However, this behaviour is extended: to
//                         directly reach elements at the end of the list it is
//                         possible to use a negative number (-1 is the last
//                         element, -2 the last but one and so on).
//
// OUTPUT PARAMETERS: BYTE
//   The byte requested.
//
// NOTES:
//   none.
//
BYTE blist_get_element(BYTE_LIST* list, int position) {

  int index = position + 1; // We use the length of the partially examined list
                            // as a index
  int i;
  BYTE_LIST_NODE* p_aux;

  if (position < 0) {
    	index += (*list).b_list_length; // In case of backward indexing, index is a
                                        // negative number: to obtain the correct
                                        // index, i sum it to the length of the
                                        // list
  }

  p_aux = (*list).p_byte_list;
  if (p_aux == NULL) {
    	perror(strcat("blist_get_element", PEB_NULL_INPUT_LIST));
    	exit(EXIT_FAILURE);
  }

  for (i=1; i!=index; i++) {
    	if (p_aux == NULL) {
      		perror(strcat("blist_get_element", PEB_WRONG_LIST_LENGTH));
      		exit(EXIT_FAILURE);
    	}
    	p_aux = (*p_aux).next_b_element;
  }

  // When i = index, p_aux points to the element containing the requested byte
  return((*p_aux).content);
}


//
// FUNCTION NAME: blist_set_element
//
// DESCRIPTION: sets an element of a byte list.
//
// INPUT PARAMETERS:
//   BYTE_LIST* list       the list from where to extract an element
//   int position          in indicates which element to extract: it works the
//                         same way as the index of an array (0 is the first
//                         element). However, this behaviour is extended: to
//                         directly reach elements at the end of the list it is
//                         possible to use a negative number (-1 is the last
//                         element, -2 the last but one and so on).
//   new_content           that's the new content of the spedified element.
//
// OUTPUT PARAMETERS: int
//   0 if the computation finishes without errors.
//   1 otherwise.
//
// NOTES:
//   none.
//
int blist_set_element(BYTE_LIST* list, int position, BYTE new_content) {

  int index = position + 1; // We use the length of the partially examined list
                            // as a index
  int i;
  BYTE_LIST_NODE* p_aux;

  if (position < 0) {
    	index += (*list).b_list_length; // In case of backward indexing, index is a
                                        // negative number: to obtain the correct
                                        // index, i sum it to the length of the
                                        // list
  }

  p_aux = (*list).p_byte_list;
  if (p_aux == NULL) {
    	perror(strcat("blist_get_element", PEB_NULL_INPUT_LIST));
    	exit(EXIT_FAILURE);
  }

  for (i=1; i!=index; i++) {
    	if (p_aux == NULL) {
      		perror(strcat("blist_get_element", PEB_WRONG_LIST_LENGTH));
      		exit(EXIT_FAILURE);
    	}
    	p_aux = (*p_aux).next_b_element;
  }

  // When i = index, p_aux points to the element containing the requested byte
  (*p_aux).content = new_content;
  return(0);
}




/*****************************************************************************
 *                                                                           *
 *   RECORD_LIST-SPECIFIC FUNCTIONS: create_rec_list, reclist_add_element,   *
 *                                   print_rec_list, reclist_get_element     *
 *                                                                           *
 *                                                                           *
 *   NOTE: in this section, the term 'record' has the meaning, specific to   *
 *         GSM, of 'structural element of a linear fixed file', instead of   *
 *         the general meaning of 'struct'.                                  *
 *                                                                           *
 *****************************************************************************/

//
// FUNCTION NAME: create_rec_list
//
// DESCRIPTION: creates a new list of records.
//
// INPUT PARAMETERS:
//   none.
//
// OUTPUT PARAMETERS: REC_LIST*
//   The newly created list.
//
// NOTES:
//   none.
//
REC_LIST* create_rec_list() {

  REC_LIST* rec_node = malloc(sizeof(REC_LIST));

  (*rec_node).rec_list_length = 0;
  (*rec_node).p_rec_list = NULL;

  return(rec_node);
}


//
// FUNCTION NAME: reclist_add_element
//
// DESCRIPTION: add the specified record in a new element at the end of
//              the specified pointer list of records.
//
// INPUT PARAMETERS:
//   REC_LIST* list          the list to expand.
//   REC new_content         the new record to add.
//
// OUTPUT PARAMETERS: int
//   0 if the computation finishes without errors.
//   1 otherwise.
//
// NOTES:
//   - Controls over parameters to be tested.
//
int reclist_add_element(REC_LIST* list, RECORD* new_content) {

  REC_LIST_NODE* p_aux = malloc(sizeof(REC_LIST_NODE));
  REC_LIST_NODE* p_index;
  int control = 0;

  (*p_aux).content = new_content;
  (*p_aux).next_rec_element = NULL;

  if (list == NULL) {
    	perror(strcat("reclist_add_element: ", PEB_NULL_INPUT_LIST));
    	exit(EXIT_FAILURE);
  } else if ((*list).p_rec_list == NULL) {
    	(*list).p_rec_list = p_aux;
    	(*list).rec_list_length = 1;
    	return(0);
  } else {
    	control = 1;
    	for (p_index = (*list).p_rec_list; (*p_index).next_rec_element!=NULL ;
	 	p_index = (*p_index).next_rec_element) {
    		control++;
    	}
  }

  //Control on list length
  if ((*list).rec_list_length != control) {
    	perror(strcat("reclist_add_element: ", PEB_WRONG_LIST_LENGTH));
    	exit(EXIT_FAILURE);
  }

  //At this point, p_index points to the last element of current list.

  (*p_index).next_rec_element = p_aux;
  (*list).rec_list_length = ++control;

  return(0);
}


//
// FUNCTION NAME: print_rec_list
//
// DESCRIPTION: prints in a series of row all the records forming the list
//              given as parameters.
//
// INPUT PARAMETERS:
//   REC_LIST* to_print      the list to print.
//
// OUTPUT PARAMETERS: int
//   0 if the computation finishes without errors.
//   1 otherwise.
//
// NOTES:
//   - Given the correctness of rec_list_length, we use it as a control over
//     the unexpected end of the list.
//
int print_rec_list(REC_LIST* to_print) {

  int i = 0;
  REC_LIST_NODE* p_aux;

  if (to_print == NULL) {
    	perror(strcat("print_rec_list: ", PEB_NULL_INPUT_LIST));
    	exit(EXIT_FAILURE);
  } else if ((*to_print).p_rec_list == NULL) {
    	printf("print_rec_list: this is an empty byte list!");
  } else {
    	for (p_aux=(*to_print).p_rec_list; p_aux!=NULL; p_aux=(*p_aux).next_rec_element) {
      		print_b_list((*p_aux).content);
      		printf("\n\n");
      		i++;
    	}
  }

  if (i != (*to_print).rec_list_length) {
    	perror(strcat("print_rec_list: ", PEB_WRONG_LIST_LENGTH));
    	exit(EXIT_FAILURE);
  }

  return(0);
}



//
// FUNCTION NAME: reclist_get_element
//
// DESCRIPTION: gets an element out of a record list.
//
// INPUT PARAMETERS:
//   REC_LIST* list       the list from where to extract an element
//   int position         indicates which element to extract: it works the
//                        same way as the index of an array (0 is the first
//                        element). However, this behaviour is extended: to
//                        directly reach elements at the end of the list it is
//                        possible to use a negative number (-1 is the last
//                        element, -2 the last but one and so on).
//
// OUTPUT PARAMETERS: RECORD*
//   A pointer to the record requested.
//
// NOTES:
//   none.
//
RECORD* reclist_get_element(REC_LIST* list, int position) {

  int index = position + 1; // We use the length of the partially examined list
                            // as a index
  int i;
  REC_LIST_NODE* p_aux;

  if (position < 0) {
    	index += (*list).rec_list_length; // In case of backward indexing, index
                                          // is a negative number: to obtain the
                                          // correct index, i sum it to the length
                                          // of the list
  }

  p_aux = (*list).p_rec_list;
  if (p_aux == NULL) {
    	perror(strcat("reclist_get_element", PEB_NULL_INPUT_LIST));
    	exit(EXIT_FAILURE);
  }

  for (i=1; i!=index; i++) {
    	if (p_aux == NULL) {
      		perror(strcat("reclist_get_element", PEB_WRONG_LIST_LENGTH));
      		exit(EXIT_FAILURE);
    	}
    	p_aux = (*p_aux).next_rec_element;
  }

  // When i = index, p_aux points to the element containing the requested
  // record
  return((*p_aux).content);
}


//
// FUNCTION NAME: reclist_set_element
//
// DESCRIPTION: sets an element of a record list.
//
// INPUT PARAMETERS:
//   REC_LIST* list       the list from where to extract an element
//   int position         in indicates which element to extract: it works the
//                        same way as the index of an array (0 is the first
//                        element). However, this behaviour is extended: to
//                        directly reach elements at the end of the list it is
//                        possible to use a negative number (-1 is the last
//                        element, -2 the last but one and so on).
//   new_content          that's the new content of the spedified element.
//
// OUTPUT PARAMETERS: int
//   0 if the computation finishes without errors.
//   1 otherwise.
//
// NOTES:
//   none.
//
int reclist_set_element(REC_LIST* list, int position, RECORD* new_content) {

  int index = position + 1; // We use the length of the partially examined list
                            // as a index
  int i;
  REC_LIST_NODE* p_aux;

  if (position < 0) {
    	index += (*list).rec_list_length; // In case of backward indexing, index
                                          // is a negative number: to obtain the
                                          // correct index, i sum it to the length
                                          // of the list
  }

  p_aux = (*list).p_rec_list;
  if (p_aux == NULL) {
    	perror(strcat("reclist_get_element", PEB_NULL_INPUT_LIST));
    	exit(EXIT_FAILURE);
  }

  for (i=1; i!=index; i++) {
    	if (p_aux == NULL) {
      		perror(strcat("reclist_get_element", PEB_WRONG_LIST_LENGTH));
      		exit(EXIT_FAILURE);
    	}
    	p_aux = (*p_aux).next_rec_element;
  }

  // When i = index, p_aux points to the element containing the requested
  // record
  (*p_aux).content = new_content;

  return(0);
}
