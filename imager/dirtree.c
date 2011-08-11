/******************************************************************************
 *
 * NAME:          dirtree.c
 *
 * AUTHORS:       Fabio Casadei
 *                fabio.casadei@mercurio-ws.com
 *
 *                Antonio Savoldi
 *                antonio.savoldi@ing.unibs.it
 *
 *
 * FILE TYPE:     C source
 *
 * DESCRIPTION:   this file contains function definitions for the directory
 *                tree extraction functionality.
 *
 *****************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "winscard.h"
#include "utils.h"
#include "dirtree.h"
#include "gsm-constants.h"
#include "pcsc-wrappers.h"
#include "debug.h"



extern int dumping_mode;

/***************************************
 *                                     *
 *   GSM SPECIFIC COMMANDS FUNCTIONS   *
 *                                     *
 ***************************************/


//
// FUNCTION NAME: gsm_status_after_command
//
// DESCRIPTION: tells with which status the card has responded to the previous
//              command by extracting staus words SW1 and SW2 from a raw
//              response and interpreting them.
//
//
// INPUT PARAMETERS:
//   RESPONSE* resp           raw response to interpret.
//
// OUTPUT PARAMETERS:  int
//   A constant corresponding to the status described by the status words
//   extracted from input parameter.
//
// NOTES:
//   - This implementation has to be changed to reflect changes in types
//     definition.
//   - Every combination of SW1 and SW2 and their meaning has been extracted
//     from GSM standard.
//
int get_status_after_command (RESPONSE* resp) {

  BYTE sw1 = blist_get_element(resp,-2);
  BYTE sw2 = blist_get_element(resp,-1);

  //printf("\nENTERED get_status_after_command.\n");

  switch(sw1) {
  	case SW1_CORRECT: {
      		if (sw2 == 0x00)
			return(SW_NORMAL_ENDING);
      		else {
			// What is better: exit or continue???
			send_warning(strcat("get_status_after_command: ", WRN_WRONG_STATUS_RETURNED));
			return(SW_WRONG_STATUS_RETURNED);
      		}
    	}
    	case SW1_CORRECT_WITH_PROACTIVE_COMMAND: {
      		return(SW_NORMAL_ENDING_WITH_PROACTIVE_COMMAND);
    	}
    	case SW1_CORRECT_SIM_DATA_DOWNLOAD_ERROR: {
      		return(SW_NORMAL_ENDING_WITH_DOWNLOAD_ERROR);
    	}
    	case SW1_CORRECT_WITH_RESPONSE: {
      		return(SW_NORMAL_ENDING_WITH_RESPONSE);
    	}
    	case SW1_SAT_BUSY: {
      		if (sw2 == 0x00)
			return(SW_SAT_BUSY);
      		else {
			// What is better: exit or continue???
			send_warning(strcat("get_status_after_command: ", WRN_WRONG_STATUS_RETURNED));
			return(SW_WRONG_STATUS_RETURNED);
      		}
    	}
    	case SW1_MEMORY_NOTE: {
      		if (sw2 <= 0x0F)
			return(SW_NORMAL_ENDING_INTERNAL_UPDATE_RETRY);
      		else if (sw2 == 0x40)
			return (SW_MEMORY_PROBLEM);
      		else {
			// What is better: exit or continue???
			send_warning(strcat("get_status_after_command: ", WRN_WRONG_STATUS_RETURNED));
			return(SW_WRONG_STATUS_RETURNED);
      		}
    	}
    	case SW1_REFERENCES: {
      		switch(sw2) {
      			case 0x00:
				return(SW_NO_EF_SELECTED);
      			case 0x02:
				return(SW_INVALID_ADDRESS);
      			case 0x04:
				return(SW_FILE_ID_PATTERN_NOT_FOUND);
      			case 0x08:
				return(SW_FILE_INCONSISTENT_WITH_COMMAND);
      			default: {
				// What is better: exit or continue???
				send_warning(strcat("get_status_after_command: ", WRN_WRONG_STATUS_RETURNED));
				return(SW_WRONG_STATUS_RETURNED);
      			}
      		}
    	}
    	case SW1_SECURITY: {
      		switch(sw2) {
      			case 0x02:
				return(SW_NO_CHV_INITIALIZED);
      			case 0x04:
				return(SW_WRONG_CHV_OR_ACCESS_NOT_GRANTED);
      			case 0x08:
				return(SW_CONTRADICTION_WITH_CHV_STATUS);
      			case 0x10:
				return(SW_CONTRADICTION_WITH_INVALIDATION_STATUS);
      			case 0x40:
				return(SW_CHV_OR_UNBLOCKCHV_BLOCKED);
      			case 0x50:
				return(SW_MAX_INCREASE_VALUE_REACHED);
      			default: {
				// What is better: exit or continue???
				send_warning(strcat("get_status_after_command: ", WRN_WRONG_STATUS_RETURNED));
				return(SW_WRONG_STATUS_RETURNED);
      			}
      		}
    	}
    	case SW1_ERROR_P3: {
      		return(SW_INCORRECT_PARAMETER_P3);
    	}
    	case SW1_ERROR_P1_P2: {
      		return(SW_INCORRECT_PARAMETER_P1_P2);
    	}
    	case SW1_ERROR_INSTRUCTION_CODE: {
      		return(SW_UNKNOWN_INSTRUCTION_CODE);
    	}
    	case SW1_ERROR_INSTRUCTION_CLASS: {
      		return(SW_UNKNOWN_INSTRUCTION_CLASS);
    	}
    	case SW1_ERROR_TECHNICAL: {
      		return(SW_TECHNICAL_PROBLEM);
    	}
  	default: {
    		send_warning(strcat("get_status_after_command: ", WRN_WRONG_STATUS_RETURNED));
    		return(SW_WRONG_STATUS_RETURNED);
  	}

  }// switch(sw1)

}



/*************************************
 *                                   *
 *   SIM CARD MANAGEMENT FUNCTIONS   *
 *                                   *
 *************************************/

// Here will be defined those functions related to the SIM_CARD structure, such as
// extract_root and create_SIM_card.
//
// This dummy section serves only to remind which functions will go in the file
// relative to this section when this big file will be split.



/*******************************************
 *                                         *
 *   DIRECTORY TREE MANAGEMENT FUNCTIONS   *
 *                                         *
 *******************************************/


   //
   // DIRECTORY_TREE-SPECIFIC FUNCTIONS: create_tree, create_EF_header,
   //                                    create_DF_header,
   //                                    create_node_contents, and
   //                                    print_tree_node
   //


//
// FUNCTION NAME: create_EF_header
//
// DESCRIPTION: creates that part of node data related to EF files,
//              initializing it according to the response given as parameter.
//
// INPUT PARAMETERS:
//   RESPONSE* resp           raw response.
//
// OUTPUT PARAMETERS:  EF_HEADER*
//   Returns a pointer to the newly created and initialized EF_header.
//
// NOTES:
//   - This function hasn't been fully tested yet.
//
EF_HEADER* create_EF_header(RESPONSE* resp) {

  EF_HEADER* ef_header = malloc(sizeof(EF_HEADER));
  BYTE_LIST* blist_app = create_b_list();
  int i;

  blist_add_element(blist_app,blist_get_element(resp,2));
  blist_add_element(blist_app,blist_get_element(resp,3));

  (*ef_header).ef_size = *blist_app;

  (*ef_header).ef_cyclic_increase_allowed = blist_get_element(resp,7);
  (*ef_header).ef_readseek_update_perm = blist_get_element(resp,8);
  (*ef_header).ef_increase_rfu_perm = blist_get_element(resp,9);
  (*ef_header).ef_rehabilitate_invalidate_perm = blist_get_element(resp,10);
  (*ef_header).ef_status = blist_get_element(resp,11);
  (*ef_header).ef_structure = blist_get_element(resp,13);
  (*ef_header).ef_record_length = blist_get_element(resp,14);

  blist_app = create_b_list();
  for(i=15; i<(*resp).b_list_length; i++) {
  	blist_add_element(blist_app,blist_get_element(resp,i));
  }
  (*ef_header).ef_rfu = *blist_app;

  return(ef_header);
  free(ef_header);
}


//
// FUNCTION NAME: create_DF_header
//
// DESCRIPTION: creates that part of node data related to DF files,
//              initializing it according to the response given as parameter.
//
// INPUT PARAMETERS:
//   RESPONSE* resp           raw response.
//
// OUTPUT PARAMETERS:  DF_HEADER*
//   Returns a pointer to the newly created and initialized DF_header.
//
// NOTES:
//   - This function hasn't implemented yet.
//
DF_HEADER* create_DF_header(RESPONSE* resp) {

  DF_HEADER* df_header = malloc(sizeof(DF_HEADER));
  BYTE_LIST* blist_app = create_b_list();
  int i;

  blist_add_element(blist_app,blist_get_element(resp,2));
  blist_add_element(blist_app,blist_get_element(resp,3));
  (*df_header).df_free_space = *blist_app;

  blist_app = create_b_list();
  for(i=7; i<12; i++) {
    	blist_add_element(blist_app,blist_get_element(resp,i));
  }
  (*df_header).df_rfu = *blist_app;

  (*df_header).df_characteristics = blist_get_element(resp,13);
  (*df_header).df_DF_sons = blist_get_element(resp,14);
  (*df_header).df_EF_sons = blist_get_element(resp,15);
  (*df_header).chv_quantity = blist_get_element(resp,16);
  (*df_header).df_rfu_18 = blist_get_element(resp,17);
  (*df_header).chv1_status = blist_get_element(resp,18);
  (*df_header).unblock_chv1_status = blist_get_element(resp,19);
  (*df_header).chv2_status = blist_get_element(resp,20);
  (*df_header).unblock_chv2_status = blist_get_element(resp,21);
  (*df_header).df_rfu_23 = blist_get_element(resp,22);

  blist_app = create_b_list();
  for(i=23; i<24; i++) {
    	blist_add_element(blist_app,blist_get_element(resp,i));
  }
  (*df_header).df_adm_management = *blist_app;

  return(df_header);
  free(df_header);
}



//
// FUNCTION NAME: create_n_list
//
// DESCRIPTION: creates a new pointer list of tree nodes.
//
// INPUT PARAMETERS:
//   none.
//
// OUTPUT PARAMETERS: TREE_NODE_LIST*
//   A pointer to the memory location where the newly created and initialized
//   structure resides.
//
// NOTES:
//   none.
//
TREE_NODE_LIST* create_n_list() {

  TREE_NODE_LIST* node = malloc(sizeof(TREE_NODE_LIST));

  (*node).n_list_length = 0;
  (*node).p_node_list = NULL;

  return(node);
}



//
// FUNCTION NAME: create_node_contents
//
// DESCRIPTION: creates a new node of the directory tree starting from data
//              provided by the smart card in response of a SELECT - GET
//              RESPONSE pair. Every field of the node will be initialized
//              according to the response given as parameter.
//
//
// INPUT PARAMETERS:
//   RESPONSE* resp                    raw response.
//   BYTE_LIST* transp_EF_contents     if the new node will represent an EF
//                                     with a transparent structure, then this
//                                     parameter points to the body of the EF.
//                                     If not, it can be null and is ignored.
//   REC_LIST* linfix_cyc_EF_contents  if the new node will represent an EF
//                                     with a linear-fixed or cyclic structure,
//                                     then this parameter points to the list
//                                     of records that is the body of such EF.
//                                     If not, it can be null and is ignored.
//
// OUTPUT PARAMETERS:  TREE_NODE_TYPE*
//   Returns the new node with proper contents added.
//
// NOTES:
//   None.
//
TREE_NODE_TYPE* create_node_contents(RESPONSE* resp,
				     BYTE_LIST* transp_EF_contents,
				     REC_LIST* linfix_cyc_EF_contents) {

  TREE_NODE_TYPE* new_node = malloc(sizeof(TREE_NODE_TYPE));
  int i;
  BYTE_LIST* p_bl_aux;
  REC_LIST* p_rl_aux;

  (*new_node).readable_body = 2;  // This field is set up so that it is unknown
                                  // if the body of the associated file is
                                  // readable or not, in the case of an EF.
                                  // The field has to be set when an attempt
                                  // to read the body of the associated file
                                  // is done

  //print_array((*resp).content, 23);

  (*new_node).file_type = blist_get_element(resp,6);
  (*new_node).rfu[0] = blist_get_element(resp,0);
  (*new_node).rfu[1] = blist_get_element(resp,1);

  p_bl_aux = create_b_list();
  (*new_node).file_id.b_list_length = (*p_bl_aux).b_list_length;
  (*new_node).file_id.p_byte_list = (*p_bl_aux).p_byte_list;
  for (i=0; i<ADDRSIZE; i++) {
    	blist_add_element(&((*new_node).file_id), blist_get_element(resp,i+4));
  }

  // Put here a control over the length of the resulting list

  //printf("\nDEBUG: file_id durante creazione = ");
  DBG("\nDEBUG: file_id durante creazione = ");
  #ifdef DEBUG
  print_b_list(&((*new_node).file_id));
  #endif
  //printf("\n");
  DBG("\n");


  (*new_node).header_size = 0x0E + blist_get_element(resp,12);

  //printf("DEBUG: header size= %i", (*new_node).header_size);
  DBG("DEBUG: header size= %i", (*new_node).header_size);

  p_bl_aux = create_b_list(); //We must recreate p_bl_aux to avoid overwriting
                              //of p_raw_header data over file_id data.
  (*new_node).p_raw_header.b_list_length = (*p_bl_aux).b_list_length;
  (*new_node).p_raw_header.p_byte_list = (*p_bl_aux).p_byte_list;
  for (i=0; i<(*resp).b_list_length; i++) {
    	blist_add_element(&((*new_node).p_raw_header), blist_get_element(resp,i));
  }

  // Initialization of node pointers
  (*new_node).next_brother = NULL;
  (*new_node).firstborn = NULL;

  // Initialization of the fields whose value depend on file type

  if ((*new_node).file_type == FTYP_EF) {
    	//printf("\nCATCHED EF.\n");
    	DBG("\nCATCHED EF.\n");
    	(*new_node).p_ef_header = create_EF_header(resp);
    	(*new_node).p_df_header = NULL;
    	(*new_node).sons_num = 0;
    	(*new_node).p_sons = NULL;
    	(*new_node).p_brothers = create_n_list();
    	if(blist_get_element(resp,13) == FSTR_TRANSP) {
      		if(transp_EF_contents == NULL) {
			//printf("\n\nDEBUG: create_node_contents: a transparent EF has been detected but no content has been provided.\n");
			DBG("\n\nDEBUG: create_node_contents: a transparent EF has been detected but no content has been provided.\n");
			//printf("\n\nThis error could invalidate the entire process of forensic extraction: correct it.\n");
			DBG("\n\nThis error could invalidate the entire process of forensic extraction: correct it.\n");
			exit(1);
      		}
      		// Insertion of the contents of the transparent EF
      		p_bl_aux = create_b_list(); //See comment above
      		(*new_node).p_raw_content_tEF.b_list_length = (*p_bl_aux).b_list_length;
      		(*new_node).p_raw_content_tEF.p_byte_list = (*p_bl_aux).p_byte_list;
      		//printf("%i",(transp_EF_contents == NULL));
      		//printf("%s",linfix_cyc_EF_contents);
      		DBG("%i",(transp_EF_contents == NULL));
      		DBG("%s",linfix_cyc_EF_contents);
      		for (i=0; i<(*transp_EF_contents).b_list_length; i++) {
			blist_add_element(&((*new_node).p_raw_content_tEF), blist_get_element(transp_EF_contents,i));
      		}
    	} else if (blist_get_element(resp,13) == FSTR_LINFIX || blist_get_element(resp,13) == FSTR_CYCLIC) {
      		// Insertion of the contents of the linear-fixed or cyclic EF.
      		if(linfix_cyc_EF_contents == NULL) {
			//printf("\n\nDEBUG: create_node_contents: a linear-fixed or cyclic EF has been detected but no content has been provided.\n");
			//printf("\n\nThis error could invalidate the entire process of forensic extraction: correct it.\n");
			DBG("\n\nDEBUG: create_node_contents: a linear-fixed or cyclic EF has been detected but no content has been provided.\n");
			DBG("\n\nThis error could invalidate the entire process of forensic extraction: correct it.\n");
			exit(1);
      		}
      		p_rl_aux = create_rec_list(); //See comment about p_bl_aux
      		(*new_node).p_raw_content_lfcEF.rec_list_length = (*p_rl_aux).rec_list_length;
      		(*new_node).p_raw_content_lfcEF.p_rec_list = (*p_rl_aux).p_rec_list;
      		for (i=0; i<(*linfix_cyc_EF_contents).rec_list_length; i++) {
			reclist_add_element(&((*new_node).p_raw_content_lfcEF),
					    reclist_get_element(linfix_cyc_EF_contents,
					    i));
      		}
    	} else {
      		//printf("\n\nDEBUG: create_node_contents: file structure not recognized.");
      		//printf("\n\nThis error could invalidate the entire process of forensic extraction: correct it.\n");
      		DBG("\n\nDEBUG: create_node_contents: file structure not recognized.");
      		DBG("\n\nThis error could invalidate the entire process of forensic extraction: correct it.\n");
      		exit(1);
    	}
    	return(new_node);
  } else if ((*new_node).file_type == FTYP_MF || (*new_node).file_type == FTYP_DF) {
    	//printf("CATCHED DF or MF");
    	DBG("CATCHED DF or MF");
    	(*new_node).p_ef_header = NULL;
    	(*new_node).p_df_header = create_DF_header(resp);
    	(*new_node).sons_num = blist_get_element(resp,14) + blist_get_element(resp,15);
    	(*new_node).p_sons = create_n_list();

    	if((*new_node).file_type == FTYP_MF) {
      		(*new_node).p_brothers = NULL;
    	} else {
      		(*new_node).p_brothers = create_n_list();
     	}
    return(new_node);
  } else {
    	perror(strcat("create_node_contents: ","UNKNOWN FILE TYPE"));
    	exit(EXIT_FAILURE);
  }

}


//
// FUNCTION NAME: print_tree_node
//
// DESCRIPTION: prints on the standard output the contents of specified node,
//              giving an interpretation of the raw response from the smart
//              card.
//
// INPUT PARAMETERS:
//   TREE_NODE_TYPE* node         the node whose content is to be printed.
//
// OUTPUT PARAMETERS:  int
//   0 if the computation finishes without errors.
//   1 otherwise.
//
// NOTES:
//   - The actual choice is to avoid the definition of the type TREE. This
//     could, however, change in the future.
//   - This function is work in progress: all the operations to print out every
//     content of the node haven't implemented yet.
//
int print_tree_node(TREE_NODE_TYPE* node) {

  int EF_struct;

  //printf("\n==============================================================\n");
  //printf("\tNODE HEADER:\n");
  //printf("--------------------------------------------------------------\n");

  DBG("\n==============================================================\n");
  DBG("\tNODE HEADER:\n");
  DBG("--------------------------------------------------------------\n");

  //printf("readable_body = \t%i\n", (*node).readable_body);
  //printf("file_type = \t\t");
  DBG("readable_body = \t%i\n", (*node).readable_body);
  DBG("file_type = \t\t");

  if ((*node).file_type == FTYP_EF) {
    	//printf("FTYP_EF\n");
    	DBG("FTYP_EF\n");
  } else if ((*node).file_type == FTYP_DF) {
    	//printf("FTYP_DF\n");
    	DBG("FTYP_DF\n");
  } else if ((*node).file_type == FTYP_MF) {
    	//printf("FTYP_MF\n");
    	DBG("FTYP_MF\n");
  } else {
    	//printf("UNKNOWN\n");
    	DBG("UNKNOWN\n");
  }

  #ifdef DEBUG
  printf("rfu bytes 0-1 = \t"); // note that this field never points to NULL
  print_array((*node).rfu, 2); // because its dimension was statically
  #endif                       // declared during the definition of
                               // TREE_NODE_TYPE structure.
  //printf("\n");
  //printf("file_id = \t\t");
  DBG("\n");
  DBG("file_id = \t\t");

  if ((*node).file_id.b_list_length == 0) {
    	//printf("NOT ASSIGNED\n");
    	DBG("NOT ASSIGNED\n");
  } else {
    	#ifdef DEBUG
	print_b_list(&((*node).file_id));
	#endif
    	//printf("\n");
    	DBG("\n");
  }


  //printf("header_size = \t\t%i\n", (*node).header_size);
  DBG("header_size = \t\t%i\n", (*node).header_size);

  if ((*node).p_raw_header.b_list_length == 0) {
    	//printf("p_raw_header is empty.\n");
    	DBG("p_raw_header is empty.\n");
  } else {
    	//printf("(*p_raw_header).b_list_length = \t%i\n", (*node).p_raw_header.b_list_length);
    	//printf("(*p_raw_header).p_byte_list = \t");
    	DBG("(*p_raw_header).b_list_length = \t%i\n", (*node).p_raw_header.b_list_length);
    	DBG("(*p_raw_header).p_byte_list = \t");
    	if ((*node).p_raw_header.p_byte_list == NULL) {
      		perror("print_tree_node: inconsitence found in p_raw_header.");
      		exit(EXIT_FAILURE);
    	} else {
      		#ifdef DEBUG
      		print_b_list(&((*node).p_raw_header));
      		#endif
      		//printf("\n");
      		DBG("\n");
    	}
  }

  //printf("\n--------------------------------------------------------------\n");
  //printf("\tNODE CONTENTS:\n");
  //printf("--------------------------------------------------------------\n");
  DBG("\n--------------------------------------------------------------\n");
  DBG("\tNODE CONTENTS:ANTONIO\n");
  DBG("--------------------------------------------------------------\n");

  if ((*node).file_type == FTYP_EF) {
    	EF_struct = blist_get_element(&(*node).p_raw_header,13);
    	if(EF_struct == FSTR_TRANSP) {
      		// Printing of the body of this transparent EF
      		if ((*node).p_raw_content_tEF.b_list_length == 0) {
			//printf("p_raw_content_tEF is empty.\n");
			DBG("p_raw_content_tEF is empty.\n");
      		}
      		else {
			//printf("(*p_raw_content_tEF).b_list_length = \t%i\n",
	       	//		(*node).p_raw_content_tEF.b_list_length);
            DBG("(*p_raw_content_tEF).b_list_length = \t%i\n",
	       			(*node).p_raw_content_tEF.b_list_length);
			//printf("(*p_raw_content_tEF).p_byte_list = \t");
			DBG("(*p_raw_content_tEF).p_byte_list = \t");
			if ((*node).p_raw_content_tEF.p_byte_list == NULL) {
	  			perror("print_tree_node: inconsitence found in p_raw_content_tEF.");
	  			exit(EXIT_FAILURE);
			} else {
	  			#ifdef DEBUG
	  			print_b_list(&((*node).p_raw_content_tEF));
	  			#endif
	  			//printf("\n");
	  			DBG("\n");
			}
      		}
    	}
    	else if (EF_struct == FSTR_LINFIX || EF_struct == FSTR_CYCLIC) {
      		// Printing of the body of this linear-fixed or cyclic EF
      		if ((*node).p_raw_content_lfcEF.rec_list_length == 0) {
			//printf("p_raw_content_lfcEF is empty.\n");
			DBG("p_raw_content_lfcEF is empty.\n");
      		}
      		else {
			//printf("(*p_raw_content_lfcEF).rec_list_length = \t%i\n",
	       	//		(*node).p_raw_content_lfcEF.rec_list_length);
			//printf("(*p_raw_content_lfcEF).p_rec_list = \t");
			DBG("(*p_raw_content_lfcEF).rec_list_length = \t%i\n",
	       			(*node).p_raw_content_lfcEF.rec_list_length);
			DBG("(*p_raw_content_lfcEF).p_rec_list = \t");
			if ((*node).p_raw_content_lfcEF.p_rec_list == NULL) {
	  			perror("print_tree_node: inconsitence found in p_raw_content_lfcEF.");
	  			exit(EXIT_FAILURE);
			} else {
	  			#ifdef DEBUG
	  			print_rec_list(&((*node).p_raw_content_lfcEF));
	  			#endif
	  			//printf("\n");
	  			DBG("\n");
			}
      		}
    	}
  }
  else if ((*node).file_type == FTYP_DF) {
    	//printf("Not an elementary file: no body here.\n");
    	DBG("Not an elementary file: no body here.\n");
  }
  else if ((*node).file_type == FTYP_MF) {
    	//printf("Not an elementary file: no body here.\n");
    	DBG("Not an elementary file: no body here.\n");
  }
  else {
    	//printf("UNKNOWN\n");
    	DBG("UNKNOWN\n");
  }

  //printf("==================================================================");
  DBG("==================================================================");

  return(0);

}


//
// FUNCTION NAME: same_file_id
//
// DESCRIPTION: Compares two nodes to see if they refer to same file.
//
// INPUT PARAMETERS:
//   TREE_NODE_TYPE* first        first node to compare.
//   TREE_NODE_TYPE* second       second node to compare.
//
// OUTPUT PARAMETERS:  int
//   0 if the nodes do not refer to the same file
//   1 otherwise
//
// NOTES:
//   - If file id is spread on more than two bytes to address a greater number
//     of files, this function has to be changed.
//
int same_file_id(TREE_NODE_TYPE* first, TREE_NODE_TYPE* second) {

  //printf("\nEntered same_file_id.\n");

  BYTE first_msb = blist_get_element(&((*first).file_id),0);
  BYTE first_lsb = blist_get_element(&((*first).file_id),1);
  BYTE second_msb = blist_get_element(&((*second).file_id),0);
  BYTE second_lsb = blist_get_element(&((*second).file_id),1);

  if((first_msb == second_msb) && (first_lsb == second_lsb)) {
    	return(1);
  }
  else {
    	return(0);
  }
}



//
// FUNCTION NAME: extract_root
//
// DESCRIPTION: initializer of the root node of the tree representing the
//              filesystem of the SIM card.
//
// INPUT PARAMETERS:
//   SCARDHANDLE hCard                     handle for a connection to the card.
//   SCARD_IO_REQUEST* dwActiveProtocol    pointer to the pcsclite defined
//                                         structure that implement the desired
//                                         protocol.
//
// OUTPUT PARAMETERS:  TREE_NODE_TYPE*
//   A pointer to the root node of the tree.
//
// NOTES:
//   - This function hasn't been tested yet.
//
TREE_NODE_TYPE* extract_root(SCARDHANDLE hCard,
			     SCARD_IO_REQUEST* dwActiveProtocol) {

  BYTE baux;
  int get_response_status; // First time we access the card we do not know
                           // which file is selected, so we have to ask this to
                           // the card, issuing a GET RESPONSE command.
  RESPONSE* resp;
  TREE_NODE_TYPE* root = malloc(sizeof(TREE_NODE_TYPE));

  //Selecting Master File
  resp = gsm_select(hCard, dwActiveProtocol, 0x3F00);
  get_response_status = get_status_after_command(resp);

  if (!(get_response_status == SW_NORMAL_ENDING_WITH_RESPONSE)) {
    	perror(strcat("extract_root: ","card responded in unexpected manner."));
    	exit(EXIT_FAILURE);
  }

  resp = gsm_get_response(hCard, dwActiveProtocol, blist_get_element(resp,1));
  get_response_status = get_status_after_command(resp);
  root = create_node_contents(resp, NULL, NULL);
  //printf("\nNEWLY CREATED ROOT NODE CONTENTS:\n");
  DBG("\nNEWLY CREATED ROOT NODE CONTENTS:\n");
  #ifdef DEBUG
  print_tree_node(root);
  #endif

  if ((*root).file_type != FTYP_MF) {
    	perror(strcat("extract_root: ", ERR_NOT_MF_ROOT));
    	exit(EXIT_FAILURE);
  }

  return(root);
}


//
// FUNCTION NAME: create_SIM_card
//
// DESCRIPTION: creates the SIM_CARD structure that describes the inserted
//              card.
//
// INPUT PARAMETERS:
//   SCARDHANDLE hCard                     handle for a connection to the card.
//   SCARD_IO_REQUEST* dwActiveProtocol    pointer to the pcsclite defined
//                                         structure that implement the desired
//                                         protocol.
//
// OUTPUT PARAMETERS: SIM_CARD*
//   A pointer to the memory location where the newly created and initialized
//   SIM_CARD structure resides.
//
// NOTES:
//   none.
//
SIM_CARD* create_SIM_card(SCARDHANDLE hCard,
			  SCARD_IO_REQUEST* dwActiveProtocol) {

  BYTE_LIST* list_of_father = create_b_list();

  SIM_CARD* new_card = malloc(sizeof(SIM_CARD));
  TREE_NODE_TYPE* father = NULL;

  // Here is the directory tree construction
  (*new_card).root = extract_root(hCard, dwActiveProtocol);
  gsm_build_up_tree(hCard, dwActiveProtocol, (*new_card).root, father, list_of_father);

  return(new_card);
}


   //
   // TREE_NODE_LIST-SPECIFIC FUNCTIONS: create_n_list, nlist_add_element and
   //                                    print_n_list
   // They are very similar to those regarding byte-lists: the difference is
   // that here the content of a list element is a pointer to a nose tree
   //  instead of a byte.
   //

//
// FUNCTION NAME: nlist_get_element
//
// DESCRIPTION: gets the contents of an element of a tree node list.
//
// INPUT PARAMETERS:
//   TREE_NODE_LIST* list  the list from where to extract an element
//   int position          in indicates which element to extract: it works the
//                         same way as the index of an array (0 is the first
//                         element). However, this behaviour is extended: to
//                         directly reach elements at the end of the list it is
//                         possible to use a negative number (-1 is the last
//                         element, -2 the last but one and so on).
//
// OUTPUT PARAMETERS: TREE_NODE_TYPE*
//   Returns a pointer to the content of the specified element.
//
// NOTES:
//   none.
//
TREE_NODE_TYPE* nlist_get_element(TREE_NODE_LIST* list, int position) {

  int index = position + 1; // We use the length of the partially examined list
                            // as a index
  int i;
  LIST_NODE_TYPE* p_aux;

  if (position < 0) {
    index += (*list).n_list_length; // In case of backward indexing, index is a
                                    // negative number: to obtain the correct
                                    // index, i sum it to the length of the
                                    // list
  }

  p_aux = (*list).p_node_list;
  if (p_aux == NULL) {
    	perror(strcat("nlist_get_element", PEB_NULL_INPUT_LIST));
    	exit(EXIT_FAILURE);
  }

  for (i=1; i!=index; i++) {
    	if (p_aux == NULL) {
      		perror(strcat("nlist_get_element", PEB_WRONG_LIST_LENGTH));
      		exit(EXIT_FAILURE);
    	}
    	p_aux = (*p_aux).next_node;
  }

  // When i = index, p_aux points to the element containing the requested byte
  return((*p_aux).p_tree_node);
}


//
// FUNCTION NAME: nlist_add_element
//
// DESCRIPTION: add the specified tree node in a new element at the front of
//              the specified pointer list of tree nodes. This kind of list
//              implements a set, so order of elements is not important.
//
// INPUT PARAMETERS:
//   LIST_NODE_TYPE* list            the list to expand.
//   TREE_NODE_TYPE* content         a pointer to the new node in the list.
//
// OUTPUT PARAMETERS: int
//   0 if the computation finishes without errors.
//   1 otherwise.
//
// NOTES:
//   - Controls over parameters to be implemented.
//
int nlist_add_element(TREE_NODE_LIST* list, TREE_NODE_TYPE* to_add) {

  LIST_NODE_TYPE* p_aux = malloc(sizeof(LIST_NODE_TYPE));
  LIST_NODE_TYPE* p_index;

  int control = 0;

  (*p_aux).p_tree_node = to_add;
  (*p_aux).next_node = NULL;

  if (list == NULL) {
    	perror(strcat("nlist_add_element: ", PEB_NULL_INPUT_LIST));
    	exit(EXIT_FAILURE);
  } else if ((*list).p_node_list == NULL) {
    	(*list).p_node_list = p_aux;
    	(*list).n_list_length = 1;
    	return(0);
  } else {
    	control = 1;
    	for (p_index = (*list).p_node_list; (*p_index).next_node!=NULL ;
	 	p_index = (*p_index).next_node) {
    		control++;
    	}
  }

  //Control over list length
  if ((*list).n_list_length != control) {
    	perror(strcat("nlist_add_element: ", PEB_WRONG_LIST_LENGTH));
    	exit(EXIT_FAILURE);
  }

  //At this point, p_index points to the last element of current list.

  (*p_index).next_node = p_aux;
  (*list).n_list_length = ++control;

  return(0);
}


//
// FUNCTION NAME: nlist_delete_element
//
// DESCRIPTION: deletes the specified tree node from the specified list.
//
// INPUT PARAMETERS:
//   TREE_NODE_LIST* list            the list to shrink.
//   TREE_NODE_TYPE* content         a pointer to the node to delete from the
//                                   list.
//
// OUTPUT PARAMETERS: int
//   0 if the node is successfully deleted from the list;
//   1 if the node is not part of the list.
//
// NOTES:
//   The input parameter representing the list is changed.
//
int nlist_delete_element(TREE_NODE_LIST* list, TREE_NODE_TYPE* to_delete) {

  LIST_NODE_TYPE* current_node = (*list).p_node_list;
  LIST_NODE_TYPE* aux = malloc(sizeof(LIST_NODE_TYPE));
  int node_num = (*list).n_list_length;
  int i,j;


  for(i=0; i<node_num; i++) {
    	if(same_file_id((*current_node).p_tree_node,to_delete)) {
      		if ((*current_node).next_node != NULL) {
			(*current_node) = (*(*current_node).next_node);
      		}
		else {
			//In this case, the node to delete is the last element of the list
			aux = current_node;
			current_node = (*list).p_node_list;
			for(j=0; j<i; j++) {
	  			current_node = (*current_node).next_node;
			}
			//At this point current_node is the last but one element.
			(*current_node).next_node = NULL;
			current_node = aux;
      		}
      		(*list).n_list_length--;
      		return(0);
  	}
    	current_node = (*current_node).next_node;
  }

  //printf("\nDEBUG: EXIT nlist_delete_element: ELEMENT TO DELETE NOT FOUND.\n");
  DBG("\nDEBUG: EXIT nlist_delete_element: ELEMENT TO DELETE NOT FOUND.\n");
  return(1);
}



//
// FUNCTION NAME: nlist_duplicate
//
// DESCRIPTION: duplicates the specified tree list.
//
// INPUT PARAMETERS:
//   TREE_NODE_LIST* list            the list to duplicate.
//
// OUTPUT PARAMETERS: TREE_NODE_LIST*
//   Returns a pointer to the newly allocated memory where the duplicate list
//   resides.
//
// NOTES:
//   This function hasn't been implemented yet.
//
TREE_NODE_LIST* nlist_duplicate(TREE_NODE_LIST* list) {

  TREE_NODE_LIST *copy = malloc(sizeof(TREE_NODE_LIST));
  LIST_NODE_TYPE* aux_from;
  LIST_NODE_TYPE* aux_to = malloc(sizeof(LIST_NODE_TYPE));
  int i;
  (*copy).n_list_length = (*list).n_list_length;

  //Buffers initialization
  (*copy).p_node_list = malloc(sizeof(LIST_NODE_TYPE));
  aux_to = (*copy).p_node_list;
  aux_from = (*list).p_node_list;

  for(i=0; i<(*list).n_list_length; i++) {
    	(*aux_to) = (*aux_from);
    	(*aux_to).next_node = malloc(sizeof(LIST_NODE_TYPE));
    	aux_to = (*aux_to).next_node;
    	aux_from = (*aux_from).next_node;
  }

  return(copy);
}



//
// FUNCTION NAME: print_n_list
//
// DESCRIPTION: prints in a row all the nodes forming the list given as
//              parameter.
//
// INPUT PARAMETERS:
//   TREE_NODE_LIST* to_print      the list to print.
//
// OUTPUT PARAMETERS: int
//   0 if the computation finishes without errors.
//   1 otherwise.
//
// NOTES:
//   - Given the correctness of n_list_length, we use it as a control over the
//     unexpected end of the list.
//
int print_n_list(TREE_NODE_LIST* to_print) {

  int i = 0;
  LIST_NODE_TYPE* p_aux;

  if (to_print == NULL) {
    	perror(strcat("print_n_list: ", PEB_NULL_INPUT_LIST));
    	exit(EXIT_FAILURE);
  }
  else if ((*to_print).p_node_list == NULL) {
    	printf("print_n_list: this is an empty list!");
  }
  else {
    	for (p_aux=(*to_print).p_node_list; p_aux!=NULL; p_aux=(*p_aux).next_node) {
      		print_tree_node((*p_aux).p_tree_node);
      		i++;
    	}
  }
  if (i != (*to_print).n_list_length) {
    	perror(strcat("print_n_list: ", PEB_WRONG_LIST_LENGTH));
    	exit(EXIT_FAILURE);
  }
  return(0);
}


/**********************
 *                    *
 *   CORE FUNCTIONS   *
 *                    *
 **********************/


//
// FUNCTION NAME: gsm_find_file
//
// DESCRIPTION: tries to issue a SELECT on the file with the specified id and
//              tells if it exists on the SIM card or not. Moreover, if the
//              file exists, saves all the related informations in a node of
//              the directory tree under construction.
//
// INPUT PARAMETERS:
//   SCARDHANDLE hCard                     handle for a connection to the card.
//   SCARD_IO_REQUEST* dwActiveProtocol    pointer to the pcsclite defined
//                                         structure that implement the desired
//                                         protocol.
//   int id                                identifier of the file we're
//   TREE_NODE_TYPE* new_node              node where to store the informations
//                                         eventually found.
//
// OUTPUT PARAMETERS:  int
//   1 if the computation finishes without errors and file exists.
//   0 if the computation finishes without errors and file doesn't exists.
//  -1 otherwise.
//
// NOTES:
//   - When basic program functionality will be implemented and tested, further
//     work will be needed to see if proactive SIMs support is meaningful or
//     not.
//   - Decide whether exit (precluding the creation of a partial tree) or not
//     (marking for example bad nodes) in presence of unexpected response
//     requests further study.
//
int gsm_find_node(SCARDHANDLE hCard, SCARD_IO_REQUEST* dwActiveProtocol,
                  long int id, TREE_NODE_TYPE* node) {

  RESPONSE* resp;
  int select_status_condition, get_response_status_condition;
  long int file_size;
  BYTE record_dim;

  resp = gsm_select(hCard, dwActiveProtocol, id);
  select_status_condition = get_status_after_command(resp);

  switch(select_status_condition) {
    	case SW_FILE_ID_PATTERN_NOT_FOUND: {
      		// This is the case that more frequently happens when a search of the
      		// complete address space is performed.
      		return(0);
    	}
    	case SW_NORMAL_ENDING_WITH_RESPONSE: {
      		// If we successfully issued a SELECT command, then the response from the
      		// card will be of two bytes, the second of whose indicates how long in
      		// bytes data prepared for GET RESPONSE is.
      		//printf("\nENTERED gsm_get_response.\n");
      		DBG("\nENTERED gsm_get_response.\n");
      		resp = gsm_get_response(hCard, dwActiveProtocol, blist_get_element(resp,1));
      		//printf("\nENTERED get_status_after_command.\n");
      		DBG("\nENTERED get_status_after_command.\n");
      		get_response_status_condition = get_status_after_command(resp);
      		break;
    	}
    	case SW_MEMORY_PROBLEM: {
      		send_warning(strcat("gsm_find_node after SELECT: ",WRN_MEM_PROBLEM));
      		return(-1);
    	}
    	case SW_TECHNICAL_PROBLEM: {
      		send_warning(strcat("gsm_find_node after SELECT: ",WRN_TECH_PROBLEM));
      		return(-1);
    	}
    	case SW_INCORRECT_PARAMETER_P3:
    	case SW_INCORRECT_PARAMETER_P1_P2:
    	case SW_UNKNOWN_INSTRUCTION_CODE:
    	case SW_UNKNOWN_INSTRUCTION_CLASS: {
      		perror(strcat("gsm_find_node after SELECT: ",
                    	      "a bug has been detected into gsm_select!"));
      		exit(EXIT_FAILURE);
    	}
    	case SW_NORMAL_ENDING:
    	case SW_WRONG_STATUS_RETURNED:
    	case SW_NORMAL_ENDING_WITH_PROACTIVE_COMMAND:
    	case SW_NORMAL_ENDING_WITH_DOWNLOAD_ERROR:
    	case SW_SAT_BUSY:
    	case SW_NORMAL_ENDING_INTERNAL_UPDATE_RETRY:
    	case SW_NO_EF_SELECTED:
    	case SW_INVALID_ADDRESS:
    	case SW_FILE_INCONSISTENT_WITH_COMMAND:
    	case SW_NO_CHV_INITIALIZED:
    	case SW_WRONG_CHV_OR_ACCESS_NOT_GRANTED:
    	case SW_CONTRADICTION_WITH_CHV_STATUS:
    	case SW_CONTRADICTION_WITH_INVALIDATION_STATUS:
    	case SW_CHV_OR_UNBLOCKCHV_BLOCKED:
    	case SW_MAX_INCREASE_VALUE_REACHED: {
      		perror(strcat("gsm_find_node after SELECT: ",ERR_WRONG_RETURN_VALUE));
      		exit(EXIT_FAILURE);
    	}
    	default: {
      		perror(strcat("gsm_find_node after SELECT: ",ERR_WRONG_RETURN_VALUE));
      		exit(EXIT_FAILURE);
    	}
  }


  //printf("\nENTERED SWITCH ABOUT get_response_status_condition.\n");
  //printf("\nDEBUG: (*resp).content= ");
  DBG("\nENTERED SWITCH ABOUT get_response_status_condition.\n");
  DBG("\nDEBUG: (*resp).content= ");
  #ifdef DEBUG
  print_b_list(resp);
  #endif
  //printf("\n");
  DBG("\n");

  switch(get_response_status_condition) {
    	case SW_NORMAL_ENDING: {
    		// Depending of which kind of node has been retrieved, the function
    		// 'create_node_contents' requires different parameters.
      		if(blist_get_element(resp,6) == FTYP_EF) {
			file_size = (256 * blist_get_element(resp,2)) + blist_get_element(resp,3);
			if(blist_get_element(resp,13) == FSTR_TRANSP) {
	  			// TREE_NODE_TYPE creation
	  			(*node) = (*create_node_contents(resp,
					   gsm_read_transparent_file(hCard, dwActiveProtocol, file_size), NULL));
	  			//printf("\nDEBUG: exit create_node_contents for TRANSPARENT FILE\n");
	  			DBG("\nDEBUG: exit create_node_contents for TRANSPARENT FILE\n");
	  			#ifdef DEBUG
	  			print_tree_node(node);
	  			#endif
	  			return(1);
			}
			else {
	  			record_dim = blist_get_element(resp,14);
	  			if (blist_get_element(resp,13) == FSTR_LINFIX) {
	    				(*node) = (*create_node_contents(resp,
						   NULL, gsm_read_linear_fixed_file(hCard, dwActiveProtocol,
										    file_size, record_dim)));
	    				//printf("\nDEBUG: exit create_node_contents for LINEAR FIXED FILE\n");
	    				DBG("\nDEBUG: exit create_node_contents for LINEAR FIXED FILE\n");
	    				#ifdef DEBUG
	    				print_tree_node(node);
	    				#endif
	    				return(1);
	  			}
	  			else {
	    				// I suppose that, in this case, the file is cyclic.
	    				// Error checking is a responsibility of 'create_node_contents'.
	    				(*node) = (*create_node_contents(resp,
						   NULL, gsm_read_cyclic_file(hCard, dwActiveProtocol,
								  	      file_size, record_dim)));
	    				//printf("\nDEBUG: exit create_node_contents for CYCLIC FILE\n");
	    				DBG("\nDEBUG: exit create_node_contents for CYCLIC FILE\n");
	    				#ifdef DEBUG
	    				print_tree_node(node);
	    				#endif
	    				return(1);
	  			}
			}
      		}
      		else {
	 		// I suppose that, in this case, the file is a DF or the MF. Error
         		// checking is a responsibility of 'create_node_contents'.
	 		(*node) = (*create_node_contents(resp, NULL, NULL));
	 		//printf("\nDEBUG: exit create_node_contents for NOT ELEMENTARY FILE\n");
	 		DBG("\nDEBUG: exit create_node_contents for NOT ELEMENTARY FILE\n");
	 		#ifdef DEBUG
	 		print_tree_node(node);
	 		#endif
	 		return(1);
      		}
    	}
    	case SW_NORMAL_ENDING_WITH_PROACTIVE_COMMAND: {
      		// In this case, we create the node with response results and we warn
      		// that proactive SIMs are not supported by this version of the software.
      		// Probably supporting those cards is useless at all.
      		//create_node_contents(resp, node);
      		send_warning(strcat("gsm_find_mode after GET RESPONSE: ",
                	            "proactive SIM non supported by this software."));
      		return(1);
    	}
    	case SW_MEMORY_PROBLEM: {
      		send_warning(strcat("gsm_find_node after GET RESPONSE: ",
                          	     WRN_MEM_PROBLEM));
      		return(-1);
    	}
    	case SW_TECHNICAL_PROBLEM: {
      		send_warning(strcat("gsm_find_node after GET RESPONSE: ",
                          	     WRN_TECH_PROBLEM));
      		return(-1);
    	}
    	case SW_INCORRECT_PARAMETER_P3:
    	case SW_INCORRECT_PARAMETER_P1_P2:
    	case SW_UNKNOWN_INSTRUCTION_CODE:
    	case SW_UNKNOWN_INSTRUCTION_CLASS: {
      		perror(strcat("gsm_find_node after GET RESPONSE: ",
                    	      "a bug has been detected into gsm_get_response!"));
      		exit(EXIT_FAILURE);
    	}
    	case SW_WRONG_STATUS_RETURNED:
    	case SW_NORMAL_ENDING_WITH_DOWNLOAD_ERROR:
    	case SW_NORMAL_ENDING_WITH_RESPONSE:
    	case SW_SAT_BUSY:
    	case SW_NORMAL_ENDING_INTERNAL_UPDATE_RETRY:
    	case SW_NO_EF_SELECTED:
    	case SW_INVALID_ADDRESS:
    	case SW_FILE_ID_PATTERN_NOT_FOUND:
    	case SW_FILE_INCONSISTENT_WITH_COMMAND:
    	case SW_NO_CHV_INITIALIZED:
    	case SW_WRONG_CHV_OR_ACCESS_NOT_GRANTED:
    	case SW_CONTRADICTION_WITH_CHV_STATUS:
    	case SW_CONTRADICTION_WITH_INVALIDATION_STATUS:
    	case SW_CHV_OR_UNBLOCKCHV_BLOCKED:
    	case SW_MAX_INCREASE_VALUE_REACHED: {
      		perror(strcat("gsm_find_node after SELECT: ", ERR_WRONG_RETURN_VALUE));
      		exit(EXIT_FAILURE);
    	}
    	default: {
      		perror(strcat("gsm_find_node after SELECT: ", ERR_WRONG_RETURN_VALUE));
      		exit(EXIT_FAILURE);
    	}
  }// switch()

}


//
// FUNCTION NAME: find_selectable_full
//
// DESCRIPTION: executes a scan of the entire file identifier space, saving the
//              informations related to the selectable files in a list. This
//              list represent the set of the files selectable from the
//              directory of the card that is current when this function is
//              called.
//
// INPUT PARAMETERS:
//   SCARDHANDLE hCard                     handle for a connection to the card.
//   SCARD_IO_REQUEST* dwActiveProtocol    pointer to the pcsclite defined
//                                         structure that implement the desired
//                                         protocol.
//   long int          starting_point      this parameter is necessary to keep
//                                         the current directory the same during
//                                         the entire cicle, despite of any
//                                         successfull selection
//
// OUTPUT PARAMETERS:  LIST_NODE_TYPE*
//   The list representing the nodes relative to all the files selectable on
//   the card at this level.
//
// NOTES:
//   - It is necessary to analize if it's a better implementation to remove an
//     input paramenter, shifting LIST_NODE_TYPE* to_fill on the return value.
//   - We suppose that the list to_fill is empty when this function is called:
//     a control to this prerequisite is needed.
//
TREE_NODE_LIST* find_selectable_full(SCARDHANDLE hCard,
				SCARD_IO_REQUEST* dwActiveProtocol,
				long int starting_point,
				long int id_father,
				BYTE_LIST* list_of_father) {

  long int file_to_test,id;
  int reset = 1, i;

  TREE_NODE_LIST* to_fill = create_n_list();
  TREE_NODE_TYPE* tnode = malloc(sizeof(TREE_NODE_TYPE));

  //printf("\nENTERED find_selectable.\n");
  //printf("\nACQUISITION MODE IS %d\n", dumping_mode);
  DBG("\nENTERED find_selectable.\n");
  DBG("\nACQUISITION MODE IS %d\n", dumping_mode);


  for (file_to_test=0x0000; file_to_test <= 0xFFFF; file_to_test++) {
  	if(reset) {
		//printf("\nFILE SELECTION OK.\n");
		DBG("\nFILE SELECTION OK.\n");
		// Starting from the first child of the root, 3F00, the selection
		// of "starting point" must be done iteratively, selecting all the DF
		// starting from Master File, which are in the upper levels of the tree
		// for(i=0;i<length(list_of_father);i++){
		//		item = pop(list_of_father);
		//		gsm_select(hCard, dwActiveProtocol, item);
		//}

		//printf("\nLIST_OF_FATHER_LENGTH: %d\n",(*list_of_father).b_list_length);
		DBG("\nLIST_OF_FATHER_LENGTH: %d\n",(*list_of_father).b_list_length);
		for(i=0; i <  (((*list_of_father).b_list_length - 1)/2 + 1); i++){
		     id = 256*blist_get_element(list_of_father, 2*i) + blist_get_element(list_of_father, (2*i) + 1);
		     //printf("\nID__FATHER: %X\n", id);
		     DBG("\nID__FATHER: %X\n", id);
		     gsm_select(hCard, dwActiveProtocol, id);
		}
		gsm_select(hCard, dwActiveProtocol, starting_point);

		reset = 0;
	}
	if (gsm_find_node(hCard, dwActiveProtocol, file_to_test, tnode) == 1) {
		nlist_add_element(to_fill, tnode);
		if(((*tnode).file_type == FTYP_DF) || ((*tnode).file_type == FTYP_MF)){
			reset = 1;
		}
		tnode = malloc(sizeof(TREE_NODE_TYPE));
	}

  }

  //printf("\nEXIT find_selectable con lista uguale a:\n");
  DBG("\nEXIT find_selectable con lista uguale a:\n");
  visualizza_file_id_lista(to_fill);


  return(to_fill);

}


//
// FUNCTION NAME: find_selectable_partial
//
// DESCRIPTION: executes a scan of the entire file identifier space, saving the
//              informations related to the selectable files in a list. This
//              list represent the set of the files selectable from the
//              directory of the card that is current when this function is
//              called.
//
// INPUT PARAMETERS:
//   SCARDHANDLE hCard                     handle for a connection to the card.
//   SCARD_IO_REQUEST* dwActiveProtocol    pointer to the pcsclite defined
//                                         structure that implement the desired
//                                         protocol.
//   long int          starting_point      this parameter is necessary to keep
//                                         the current directory the same during
//                                         the entire cicle, despite of any
//                                         successfull selection
//
// OUTPUT PARAMETERS:  LIST_NODE_TYPE*
//   The list representing the nodes relative to all the files selectable on
//   the card at this level.
//
// NOTES:
//   - It is necessary to analize if it's a better implementation to remove an
//     input paramenter, shifting LIST_NODE_TYPE* to_fill on the return value.
//   - We suppose that the list to_fill is empty when this function is called:
//     a control to this prerequisite is needed.
//
TREE_NODE_LIST* find_selectable_partial(SCARDHANDLE hCard,
				SCARD_IO_REQUEST* dwActiveProtocol,
				long int starting_point,
				long int id_father,
				BYTE_LIST* list_of_father) {

  long int file_to_test,id;
  int reset = 1, i;

  TREE_NODE_LIST* to_fill = create_n_list();
  TREE_NODE_TYPE* tnode = malloc(sizeof(TREE_NODE_TYPE));

  //printf("\nENTERED find_selectable.\n");
  DBG("\nENTERED find_selectable.\n");

  //print_tree_node(tnode);
  //printf("\nACQUISITION MODE IS %d\n", dumping_mode);
  DBG("\nACQUISITION MODE IS %d\n", dumping_mode);

  for (file_to_test=0; file_to_test <= sizeof(EF_list)/sizeof(EF_list[0]) - 1; file_to_test++) {
  	if(reset) {
		//printf("\nFILE SELECTION OK.\n");
		DBG("\nFILE SELECTION OK.\n");
		// Starting from the first child of the root, 3F00, the selection
		// of "starting point" must be done iteratively, selecting all the DF
		// starting from Master File, which are in the upper levels of the tree
		// for(i=0;i<length(list_of_father);i++){
		//      item = pop(list_of_father);
		//      gsm_select(hCard, dwActiveProtocol, item);
		// }

		//printf("\nLIST_OF_FATHER_LENGTH: %d\n",(*list_of_father).b_list_length);
		DBG("\nLIST_OF_FATHER_LENGTH: %d\n",(*list_of_father).b_list_length);
		for(i=0; i <  (((*list_of_father).b_list_length - 1)/2 + 1); i++){
		    id = 256*blist_get_element(list_of_father, 2*i) + blist_get_element(list_of_father, (2*i) + 1);
		    //printf("\nID__FATHER: %X\n", id);
		    DBG("\nID__FATHER: %X\n", id);
		    gsm_select(hCard, dwActiveProtocol, id);
		}
		gsm_select(hCard, dwActiveProtocol, starting_point);

		reset = 0;
	}
	if (gsm_find_node(hCard, dwActiveProtocol, EF_list[file_to_test], tnode) == 1) {
		nlist_add_element(to_fill, tnode);
		if(((*tnode).file_type == FTYP_DF) || ((*tnode).file_type == FTYP_MF)){
			reset = 1;
		}
		tnode = malloc(sizeof(TREE_NODE_TYPE));
	}
  }

  //printf("\nEXIT find_selectable con lista uguale a:\n");
  DBG("\nEXIT find_selectable con lista uguale a:\n");
  visualizza_file_id_lista(to_fill);

  return(to_fill);

}




//
// FUNCTION NAME: extract_sons
//
// DESCRIPTION: extract from the list of the files selectable from the current
//              directory those that are sons of that directory. To accomplish
//              this task, further information is required about who is the
//              current card directory, its father and its sibling. To achieve
//              a better understanding of how this function works, read the
//              GSM standards.
//
// INPUT PARAMETERS:
//   LIST_NODE_TYPE* selectable        list representing the files selectable
//                                     from the current card directory.
//   TREE_NODE_TYPE* father            node representing the father of current
//                                     card directory.
//   TREE_NODE_TYPE* current           node representing the current card
//                                     directory.
//   LIST_NODE_TYPE* brothers          list representing the ??DFs at the same
//                                     level of current card directory.
//
// OUTPUT PARAMETERS:  TREE_NODE_LIST*
//   The list where the DF and EF sons of current card directory will be stored
//
// NOTES:
//   - To avoid the passing of a parameter representing MF, this node is
//     constructed here in a fictitious manner.
//   - The function modifies the parameter selectable
//
TREE_NODE_LIST* extract_sons(TREE_NODE_LIST* selectable_copy,
			     TREE_NODE_TYPE* father,
			     TREE_NODE_TYPE* current,
			     TREE_NODE_LIST* brothers) {

  BYTE_LIST* pbl_aux = create_b_list();
  TREE_NODE_TYPE* aux = malloc(sizeof(TREE_NODE_TYPE));
  TREE_NODE_TYPE* bro_aux = malloc(sizeof(TREE_NODE_TYPE));
  TREE_NODE_TYPE* MF_copy = malloc(sizeof(TREE_NODE_TYPE));
  int i,j;
  int finito = 0;

  (*MF_copy).file_id.b_list_length = (*pbl_aux).b_list_length;
  (*MF_copy).file_id.p_byte_list = (*pbl_aux).p_byte_list;
  blist_add_element((&(*MF_copy).file_id),0x3F);
  blist_add_element((&(*MF_copy).file_id),0x00);

  i=0;
  while(!finito) {

	if( (i == ((*selectable_copy).n_list_length - 1)) || (*selectable_copy).n_list_length == 0 ) {
		finito = 1; //Termination of cycle
    	}
    	aux = nlist_get_element(selectable_copy,i); //i-esim element of selectable set

    	if(same_file_id(aux,MF_copy) || same_file_id(aux,current) || 	//checking for current DF
       		       (father!=NULL) && same_file_id(aux,father))   	//checking for father,
                                                   			//provided that father of MF
                                                   			//doesn't exists.
      	{
		nlist_delete_element(selectable_copy, aux);
		i--;  	//This crucial instruction 'reset' the index on the correct
	      		//position in the list. Infact, without this, the element
	      		//immediately following the deleted one would be skipped
      	}
    	else {
      		if(brothers!=NULL) {  	//I take into account the fact that a node can be
	                    		//be an 'only child'.
			for(j=0; j<(*brothers).n_list_length; j++) {
	  			bro_aux = nlist_get_element(brothers,j);
	  			if(same_file_id(aux,bro_aux)) {
	    				nlist_delete_element(selectable_copy, aux);
	    				i--; 	 //See comment above
	    				break;   //Once the aux node has been identified as a brother, we do
	           				 //not need further comparisons.
	  			}
			}
      		}
    	}
    	i++;
  }

  return(selectable_copy);
}



//
// FUNCTION NAME: extract_DF_sons
//
// DESCRIPTION: extract from the list of the sons of the current card directory
//              those that are DF, looking at the file_type field.
//
// INPUT PARAMETERS:
//   LIST_NODE_TYPE* sons        list representing the files that are sons of
//                               the current card directory.
//   LIST_NODE_TYPE* DF_sons     list where DF sons of the current directory
//                               will be stored.
//
// OUTPUT PARAMETERS:  TREE_NODE_LIST*
//   Returns a pointer to the list formed by the elements of the input list
//   that are DFs.
//
// NOTES:
//   None.
//
TREE_NODE_LIST* extract_DF_sons(TREE_NODE_LIST* sons) {

  TREE_NODE_LIST* DF_sons = create_n_list();
  TREE_NODE_TYPE* aux;
  int i;

  for(i=0; i<(*sons).n_list_length; i++) {
    	aux = nlist_get_element(sons,i);
    	if((*aux).file_type == FTYP_DF)
      	{
		nlist_add_element(DF_sons, aux);
      	}
  }

  return(DF_sons);
}


//
// FUNCTION NAME: gsm_build_up_tree
//
// DESCRIPTION: extract the entire filesystem from the SIM card.
//
// INPUT PARAMETERS:
//   SCARDHANDLE hCard                     handle for a connection to the card.
//   SCARD_IO_REQUEST* dwActiveProtocol    pointer to the pcsclite defined
//                                         structure that implement the desired
//                                         protocol.
//   TREE_NODE_TYPE* tree                  that's the node representing the
//                                         subtree that is currently being
//                                         built. For this function is
//                                         recursive, first time it is called
//                                         that parameter represent the root of
//                                         the directory tree (the Master File).
//                                         filesystem of the card.
//   TREE_NODE_TYPE* father                father of current directory.
//
// OUTPUT PARAMETERS:  int
//   0 if the computation finishes without errors.
//   1 otherwise.
//
// NOTES:
//   - It is necessary to analize if it's a better implementation to remove an
//     input paramenter, shifting TREE_NODE_TYPE* tree on the return value.
//   - This function hasn't been tested yet.
//
int gsm_build_up_tree(SCARDHANDLE hCard, SCARD_IO_REQUEST* dwActiveProtocol,
		      TREE_NODE_TYPE* tree, TREE_NODE_TYPE* father, BYTE_LIST* list_of_father) {

  int i;
  long int id, id_f;
  TREE_NODE_LIST *selectable, *sons, *brothers;
  TREE_NODE_TYPE *son, *bro;

  // ID of current file
  id = 256 * blist_get_element((&((*tree).file_id)),0) + blist_get_element((&((*tree).file_id)),1);
  //printf("\nCURRENT ID:: 0x%ld\n", id);
  DBG("\nCURRENT ID:: 0x%ld\n", id);
  if ( !(father == NULL) ){
  	id_f = 256 * blist_get_element((&((*father).file_id)),0) + blist_get_element((&((*father).file_id)),1);
   	//printf("\nID FATHER:   0x%.2X\n", id_f);
   	DBG("\nID FATHER:   0x%X\n", id_f);
	blist_add_element(list_of_father, blist_get_element((&((*father).file_id)),0));
	blist_add_element(list_of_father, blist_get_element((&((*father).file_id)),1));

  }
  else {
  	id_f = 0x3F00; // I select Master File as default
	blist_add_element(list_of_father, 0x3F);
	blist_add_element(list_of_father, 0x00);
  }

  //printf("\nDEBUG: sto analizzando il DF %ld\n", id);
  //printf("\nENTERED find_selectable.\n");
  //printf("\nLIST_OF_FATHER_LENGTH::: %d\n",(*list_of_father).b_list_length);
  DBG("\nDEBUG: sto analizzando il DF %ld\n", id);
  DBG("\nENTERED find_selectable.\n");
  DBG("\nLIST_OF_FATHER_LENGTH::: %d\n",(*list_of_father).b_list_length);

  for(i=0; i < (((*list_of_father).b_list_length-1)/2 + 1); i++){
  	//printf("\n FIRST: %.2X\n", blist_get_element(list_of_father,2*i));
	//printf("\n SECOND: %.2X\n", blist_get_element(list_of_father,2*i+1));
	DBG("\n FIRST: %.2X\n", blist_get_element(list_of_father,2*i));
	DBG("\n SECOND: %.2X\n", blist_get_element(list_of_father,2*i+1));
  }




  //SELECTABLE_SET CREATION

  if(dumping_mode == 0){
	//printf("\nDUMPING_MODE = %d -- FULL ACQUISITION\n", dumping_mode);
	DBG("\nDUMPING_MODE = %d -- FULL ACQUISITION\n", dumping_mode);
	selectable = find_selectable_full(hCard, dwActiveProtocol, id, id_f, list_of_father);
  } else {
	//printf("\nDUMPING_MODE = %d -- PARTIAL ACQUISITION\n", dumping_mode);
	DBG("\nDUMPING_MODE = %d -- PARTIAL ACQUISITION\n", dumping_mode);
	selectable = find_selectable_partial(hCard, dwActiveProtocol, id, id_f, list_of_father);
  }


  //printf("\nDEBUG: gsm_build_up_tree: lista dei file selezionabili a partire da %.2X%.2X.\n",
  // blist_get_element((&((*tree).file_id)),0),
  // blist_get_element((&((*tree).file_id)),1));
  DBG("\nDEBUG: gsm_build_up_tree: lista dei file selezionabili a partire da %.2X%.2X.\n",
	 blist_get_element((&((*tree).file_id)),0),
	 blist_get_element((&((*tree).file_id)),1));
  visualizza_file_id_lista(selectable);

  //DEBUG CODE
  if(!(father == NULL)) {
  	//printf("\nDEBUG: FATHER OF CURRENT NODE: -- FATHER --\n");
  	DBG("\nDEBUG: FATHER OF CURRENT NODE: -- FATHER --\n");
    #ifdef DEBUG
    print_tree_node(father);
    #endif
  }
  if(!(tree == NULL)) {
  	//printf("\nDEBUG: CURRENT NODE: -- TREE --\n");
  	DBG("\nDEBUG: CURRENT NODE: -- TREE --\n");
  	#ifdef DEBUG
	print_tree_node(tree);
	#endif
  }
  //printf("\nDEBUG: CURENT NODE:\n");
  DBG("\nDEBUG: CURENT NODE:\n");
  print_tree_node(tree);
  if(!((*tree).p_brothers == NULL)) {
    	//printf("\nDEBUG: SIBLINGS OF CURRENT NODE:\n");
    	DBG("\nDEBUG: SIBLINGS OF CURRENT NODE:\n");
    	visualizza_file_id_lista((*tree).p_brothers);
  }
  //END OF DEBUG CODE

  /***************************************************************/
  //SONS_SET CREATION
  sons = extract_sons(selectable, father, tree, (*tree).p_brothers);

  //printf("\nDEBUG: FOUND SONS FROM extract_sons FUNCTION:\n");
  DBG("\nDEBUG: FOUND SONS FROM extract_sons FUNCTION:\n");
  visualizza_file_id_lista(sons);

  if(!(sons == NULL) && !((*sons).n_list_length == 0)) {

    	//printf("\nNumber of sons: %i.\n",(*sons).n_list_length);
    	//printf("\nNumber of sons according to the SIM: %i.\n",(*tree).sons_num);
    	DBG("\nNumber of sons: %i.\n",(*sons).n_list_length);
    	DBG("\nNumber of sons according to the SIM: %i.\n",(*tree).sons_num);

    	//Check not done because the number of direct directories declared
    	//by the card is not equal to what has been found in a test: the problem
    	//originated from hidden directory 7F21.
    	/*
    	//Check the correctness of sons computation
    	if((*tree).sons_num != (*sons).n_list_length) {
      		perror(strcat("gsm_build_up_tree: ", "number of sons declared by the card differs from that calculated"));
      		exit(EXIT_FAILURE);
    	}
    	*/

    	//Assign sons to current node.
    	(*tree).p_sons = sons;

    	//printf("\nDEBUG: gsm_build_up_tree: files list son of %.2X%.2X.\n",
       	//	blist_get_element((&(*tree).file_id),0),
       	//	blist_get_element((&(*tree).file_id),1));
       	DBG("\nDEBUG: gsm_build_up_tree: files list son of %.2X%.2X.\n",
       		blist_get_element((&(*tree).file_id),0),
       		blist_get_element((&(*tree).file_id),1));
    	visualizza_file_id_lista((*tree).p_sons);


    	for(i=0; i<(*sons).n_list_length; i++) {
		//Siblings of a node are all the
                //sons of the father of that node
		//excluded the node itself

		son = nlist_get_element(sons,i);
      		brothers = nlist_duplicate(sons);
      		nlist_delete_element(brothers, son);
      		(*son).p_brothers = brothers;


		//printf("\nDEBUG: siblings of %.2X%.2X = ",
       	//	blist_get_element((&(*son).file_id),0),
       	//	blist_get_element((&(*son).file_id),1));
       	DBG("\nDEBUG: siblings of %.2X%.2X = ",
       		blist_get_element((&(*son).file_id),0),
       		blist_get_element((&(*son).file_id),1));
      		visualizza_file_id_lista((*son).p_brothers);
    	} // for()

    	//printf("\nDEBUG: gsm_build_tree: All the sets are defined.\n");
    	DBG("\nDEBUG: gsm_build_tree: All the sets are defined.\n");


    	//Building of directory tree pointer structure.
    	brothers = nlist_duplicate(sons);
    	son = nlist_get_element(brothers,0);
    	(*tree).firstborn = malloc(sizeof(TREE_NODE_TYPE));
    	(*(*tree).firstborn) = (*son);
    	nlist_delete_element(brothers, son);

    	//printf("\nDEBUG: gsm_build_tree: First pointer inizialized.\n");
    	//printf("\nDEBUG: Number of siblings = %i", (*brothers).n_list_length);
    	DBG("\nDEBUG: gsm_build_tree: First pointer inizialized.\n");
    	DBG("\nDEBUG: Number of siblings = %i", (*brothers).n_list_length);

    	if((*brothers).n_list_length == 0) {
      		(*(*tree).firstborn).next_brother = NULL;
    	}
    	else {
      		(*(*tree).firstborn).next_brother = malloc(sizeof(TREE_NODE_TYPE));
      		bro = (*(*tree).firstborn).next_brother;
    	}

    	for(i=0; i<(*brothers).n_list_length; i++) {
      		son = nlist_get_element(brothers,i);
      		(*bro) = (*son);
      		if(i == (*brothers).n_list_length - 1) {
			(*bro).next_brother = NULL;
      		}
      		else {
			(*bro).next_brother = malloc(sizeof(TREE_NODE_TYPE));
			bro = (*bro).next_brother;
      		}
    	} // for()

    	TREE_NODE_TYPE* t_aux;

    	if((*tree).firstborn != NULL) {
      		//printf("\nDEBUG: gsm_build_up_tree: siblings of the first pointer of %.2X%.2X:\n",
	     	//	blist_get_element(&(*tree).file_id,0),
	     	//	blist_get_element(&(*tree).file_id,1));
	     	DBG("\nDEBUG: gsm_build_up_tree: siblings of the first pointer of %.2X%.2X:\n",
	     		blist_get_element(&(*tree).file_id,0),
	     		blist_get_element(&(*tree).file_id,1));
      		t_aux = (*(*tree).firstborn).next_brother;

      		while(!(t_aux == NULL)) {
			#ifdef DEBUG
			print_tree_node(t_aux);
			#endif
			t_aux = (*t_aux).next_brother;
      		}
    	}

    	son = (*tree).firstborn;
    	if(son!=NULL) {
      		bro = (*(*tree).firstborn).next_brother;
    	}

    	if(son != NULL) {
      		if(((*son).file_type == FTYP_DF)) {
			// I recursively construct the tree descending into every son that is
			// a directory:
			// 'tree' becomes new father
			// 'son = (*tree).firstborn' becomes tree.
			//printf("\nRecursion in every son that is DF...\n");
			DBG("\nRecursion in every son that is DF...\n");
			gsm_build_up_tree(hCard, dwActiveProtocol, son, tree, list_of_father);
      		}
    	}

    	if(bro != NULL) {
      		//printf("\nNumber of siblings to analyze = %i\n",
            // 	(*(*son).p_brothers).n_list_length);
            DBG("\nNumber of siblings to analyze = %i\n",
             	(*(*son).p_brothers).n_list_length);
      		for(i=0; i<(*(*son).p_brothers).n_list_length; i++) {
			if(((*bro).file_type == FTYP_DF)) {
	  			// I recursively construct the tree descending into every brother
				// that is a directory:
				// 'tree' becomes new father
				// 'bro' becomes tree
				//printf("\nRecursion in every sibling that is DF...\n");
				DBG("\nRecursion in every sibling that is DF...\n");
	  			gsm_build_up_tree(hCard, dwActiveProtocol, bro, tree, list_of_father);
			}
			bro = (*bro).next_brother;
      		}
       	}

  } // if()


}


/***********************
 *                     *
 *   DEBUG FUNCTIONS   *
 *                     *
 ***********************/


int visualizza_file_id_lista(TREE_NODE_LIST* list) {

  int index;
  LIST_NODE_TYPE* l;
  TREE_NODE_TYPE* t;

  if (list == NULL) {
    	perror(strcat("visualizza_file_id_lista: ","il parametro list è NULL."));
    	exit(EXIT_FAILURE);
  }
  else {
    	l = (*list).p_node_list;
  }

  if (l == NULL) {
    	//printf("\nvisualizza_file_id_lista: LISTA VUOTA.\n");
    	DBG("\nvisualizza_file_id_lista: LISTA VUOTA.\n");
    	return(0);
  } else {
    	for (index=0; index<(*list).n_list_length; index++) {
      		t = (*l).p_tree_node;
      		//printf(" ");
      		//printf("%.2X", blist_get_element(&((*t).file_id),0));
      		//printf("%.2X", blist_get_element(&((*t).file_id),1));
      		//printf(" ");
      		DBG(" ");
      		DBG("%.2X", blist_get_element(&((*t).file_id),0));
      		DBG("%.2X", blist_get_element(&((*t).file_id),1));
      		DBG(" ");
      		l = (*l).next_node;
    	}
  }

  return(0);
}
