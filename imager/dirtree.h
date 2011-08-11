/******************************************************************************
 *
 * NAME:          dirtree.h
 *
 * AUTHORS:       Fabio Casadei
 *		  fabio.casadei@mercurio-ws.com
 *
 *		  Antonio Savoldi
 *		  antonio.savoldi@ing.unibs.it
 *
 * FILE TYPE:     Header file
 *
 * DESCRIPTION:   this file contains types definitions and function prototypes
 *                for the directory tree extraction functionality. 
 *
 *****************************************************************************/



/****************************
 *                          *
 *   CONSTANTS DEFINITION   *
 *                          *
 ****************************/

// SMART CARD TECHNICAL PARAMETERS DEFINITIONS

#define ADDRSIZE 2   // Every file is identified by a two bytes address


// WARNING MESSAGES DEFINITION

#define WRN_WRONG_STATUS_RETURNED "wrong status condition returned by the card"
#define WRN_SAT_BUSY              "SIM Application Toolkit busy. Command cannot be executed at present but further normal commands are allowed"
#define WRN_MEM_PROBLEM           "there is a problem with card memory"
#define WRN_TECH_PROBLEM          "a technical problem with no diagnostic given has happened"


// STATUS CONDITIONS RETURNED BY THE CARD DEFINITION

#define SW_NORMAL_ENDING                           1
#define SW_WRONG_STATUS_RETURNED                   2
#define SW_NORMAL_ENDING_WITH_PROACTIVE_COMMAND    3
#define SW_NORMAL_ENDING_WITH_DOWNLOAD_ERROR       4
#define SW_NORMAL_ENDING_WITH_RESPONSE             5
#define SW_SAT_BUSY                                6
#define SW_NORMAL_ENDING_INTERNAL_UPDATE_RETRY     7
#define SW_MEMORY_PROBLEM                          8
#define SW_NO_EF_SELECTED                          9
#define SW_INVALID_ADDRESS                        10
#define SW_FILE_ID_PATTERN_NOT_FOUND              11
#define SW_FILE_INCONSISTENT_WITH_COMMAND         12
#define SW_NO_CHV_INITIALIZED                     13
#define SW_WRONG_CHV_OR_ACCESS_NOT_GRANTED        14
#define SW_CONTRADICTION_WITH_CHV_STATUS          15
#define SW_CONTRADICTION_WITH_INVALIDATION_STATUS 16
#define SW_CHV_OR_UNBLOCKCHV_BLOCKED              17
#define SW_MAX_INCREASE_VALUE_REACHED             18
#define SW_INCORRECT_PARAMETER_P3                 19
#define SW_INCORRECT_PARAMETER_P1_P2              20
#define SW_UNKNOWN_INSTRUCTION_CODE               21
#define SW_UNKNOWN_INSTRUCTION_CLASS              22
#define SW_TECHNICAL_PROBLEM                      23



/************************
 *                      *
 *   TYPES DEFINITION   *
 *                      *
 ************************/

typedef struct {
  BYTE_LIST    ef_size;      // File size is specified in bytes 3-4 of
                             // response to a select
  BYTE         ef_cyclic_increase_allowed;      // byte 8 of response
  BYTE         ef_readseek_update_perm;         // byte 9 of response
  BYTE         ef_increase_rfu_perm;            // byte 10 of response
  BYTE         ef_rehabilitate_invalidate_perm; // byte 11 of response
  BYTE         ef_status;                       // byte 12 of response
  BYTE         ef_structure;                    // byte 14 of response
  BYTE         ef_record_length;                // byte 15 of response
  BYTE_LIST    ef_rfu;                          // byte 16 of response
} EF_HEADER;

typedef struct {
  BYTE_LIST    df_free_space; // Also free space under selected directory
                              // is specified in bytes 3-4 of response
  BYTE_LIST    df_rfu;                          // bytes 8-12 of response
  BYTE         df_characteristics;              // byte 14 of response
  BYTE         df_DF_sons;                      // byte 15 of response
  BYTE         df_EF_sons;                      // byte 16 of response
  BYTE         chv_quantity;                    // byte 17 of response
  BYTE         df_rfu_18;                       // byte 18 of response
  BYTE         chv1_status;                     // byte 19 of response
  BYTE         unblock_chv1_status;             // byte 20 of response
  BYTE         chv2_status;                     // byte 21 of response
  BYTE         unblock_chv2_status;             // byte 22 of response
  BYTE         df_rfu_23;                       // byte 23 of response
  BYTE_LIST    df_adm_management;               // bytes 24-34 of response
} DF_HEADER;

typedef struct T_N_L TREE_NODE_LIST; // Forward definition

typedef struct NODE TREE_NODE_TYPE;
struct NODE {
  int          readable_body;
  // Header interpretation fields
  BYTE         file_type;   // byte 7
  BYTE         rfu[2];      // bytes 1-2
  FILE_ID      file_id;     // bytes 5-6
  BYTE         header_size; // header dimensions in byte,
                            // result of: 14 + content of byte 13
  EF_HEADER*   p_ef_header;
  DF_HEADER*   p_df_header;
  int sons_num;

  // Forensic byte image raw header field
  RESPONSE  p_raw_header;

  // Contents interpretation fields are not provided since the interpretation
  // changes from file to file, as defined in the standards. Interpretation
  // will be carried on by the XML examining tool.

  // Forensic byte image raw content field: for transparent files store data
  // organized in a different way respect to linear-fixed and cyclic files,
  // two pointers at two different structured types are needed here. Every
  // node will have, based on the type of file it represents, only one of
  // these pointers not null. Note that linear-fixed and cyclic files, being
  // very similar in structure, can use the same pointer, because their
  // contents are represented by the same structured type.
  BYTE_LIST  p_raw_content_tEF;
  REC_LIST p_raw_content_lfcEF;

  // Informations needed to correctly extract sons from selectable files
  //
  // NOTE ABOUT THE IMPLEMENTATION: this implementation wastes a lot of memory
  //    becauses saves a node where the sole file id will suffice. However,
  //    correcting adding a new type like FILE_ID_LIST is unrefined.
  //
  TREE_NODE_LIST* p_brothers;
  TREE_NODE_LIST* p_sons;
  
  // Fields that implement the structure of the directory tree (that is a
  // binary tree)
  TREE_NODE_TYPE* next_brother;
  TREE_NODE_TYPE* firstborn;
};

typedef struct {

  // General information: RAW ATR
  RESPONSE* raw_atr;

  // Space reserved to fields that store other informations about the SIM card,
  // obtained in ways different from ATR and standard filesystem exploring
  // (for example, CHVs and UNBLOCK CHVs).
  // ...

  TREE_NODE_TYPE* root;  // Reproduction of the entire filesystem of the SIM
} SIM_CARD;

typedef struct LNODE LIST_NODE_TYPE;
struct LNODE {
  TREE_NODE_TYPE* p_tree_node;
  LIST_NODE_TYPE* next_node;
};

struct T_N_L {
  int n_list_length;
  LIST_NODE_TYPE* p_node_list;
};


//
// FUNCTION PROTOTYPES
//

   //
   // GSM COMMANDS SPECIFIC FUNCTIONS
   //


int get_status_after_command (RESPONSE* resp);


   //
   // DIRECTORY_TREE-SPECIFIC FUNCTIONS: create_tree, create_EF_header,
   //                                    create_DF_header,
   //                                    create_node_contents, and
   //                                    print_tree_node
   //

EF_HEADER* create_EF_header(RESPONSE* resp);

DF_HEADER* create_DF_header(RESPONSE* resp);

TREE_NODE_LIST* create_n_list();

TREE_NODE_TYPE* create_node_contents(RESPONSE* resp,
				     BYTE_LIST* transp_EF_contents,
				     REC_LIST* linfix_cyc_EF_contents);

int print_tree_node(TREE_NODE_TYPE* node);

int same_file_id(TREE_NODE_TYPE* first, TREE_NODE_TYPE* second);

TREE_NODE_TYPE* extract_root(SCARDHANDLE hCard,
			     SCARD_IO_REQUEST* dwActiveProtocol);

SIM_CARD* create_SIM_card(SCARDHANDLE hCard,
			  SCARD_IO_REQUEST* dwActiveProtocol);

   //
   // TREE_NODE_LIST-SPECIFIC FUNCTIONS: create_n_list, nlist_add_element and
   //                                    print_n_list
   // They are very similar to those regarding byte-lists: the difference is
   // that here the content of a list element is a pointer to a nose tree
   //  instead of a byte.
   //

TREE_NODE_TYPE* nlist_get_element(TREE_NODE_LIST* list, int position);

int nlist_add_element(TREE_NODE_LIST* list, TREE_NODE_TYPE* to_add);

int nlist_delete_element(TREE_NODE_LIST* list, TREE_NODE_TYPE* to_delete);

TREE_NODE_LIST* nlist_duplicate(TREE_NODE_LIST* list);

int print_n_list(TREE_NODE_LIST* to_print);

int visualizza_file_id_lista(TREE_NODE_LIST* list);


   //
   // CORE-FUNCTIONS
   //

int gsm_find_node(SCARDHANDLE hCard, SCARD_IO_REQUEST* dwActiveProtocol,
                  long int id, TREE_NODE_TYPE* node);

TREE_NODE_LIST* find_selectable_full(SCARDHANDLE hCard,
				SCARD_IO_REQUEST* dwActiveProtocol,
                long int starting_point,
				long int id_father,
				BYTE_LIST* list_of_father);

TREE_NODE_LIST* find_selectable_partial(SCARDHANDLE hCard,
				SCARD_IO_REQUEST* dwActiveProtocol,
                long int starting_point,
				long int id_father,
				BYTE_LIST* list_of_father);


TREE_NODE_LIST* extract_sons(TREE_NODE_LIST* selectable,
			     TREE_NODE_TYPE* father, TREE_NODE_TYPE* current,
			     TREE_NODE_LIST* brothers);

TREE_NODE_LIST* extract_DF_sons(TREE_NODE_LIST* sons);

int gsm_build_up_tree(SCARDHANDLE hCard, SCARD_IO_REQUEST* dwActiveProtocol,
		      TREE_NODE_TYPE* tree, TREE_NODE_TYPE* father, BYTE_LIST* list_of_father);
