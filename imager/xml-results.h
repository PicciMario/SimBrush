/******************************************************************************
 *
 * NAME:          xml-results.h
 *
 * AUTHORS:       Fabio Casadei
 *                fabio.casadei@mercurio-ws.com
 *
 *                Antonio Savoldi
 *                antonio.savoldi@ing.unibs.it
 *     
 * FILE TYPE:     Header file
 *
 * DESCRIPTION:   this file contains types definitions and function prototypes
 *                for the xml results formatting functionality. 
 *
 *****************************************************************************/



/****************************
 *                          *
 *   CONSTANTS DEFINITION   *
 *                          *
 ****************************/



/************************
 *                      *
 *   TYPES DEFINITION   *
 *                      *
 ************************/



//
// FUNCTION PROTOTYPES
//

//PRIVATE
void indent(FILE* output_file, int indent_level);

//PRIVATE
void write_tag(FILE* output_file, int indent_level,
	       char* tag_name, int closure_tag);

//PRIVATE
RESPONSE* get_SWs_from_resp (RESPONSE* resp);

//PRIVATE
RESPONSE* clean_resp_from_SWs (RESPONSE* resp);

//PRIVATE
void xml_convert_ef_transp(FILE* file_to_write, int indent_level,
			   TREE_NODE_TYPE* node);

//PRIVATE
void xml_convert_ef_linfixcyc(FILE* file_to_write, int indent_level,
			      TREE_NODE_TYPE* node, int linfix);

//PRIVATE
void xml_convert_tree(FILE* file_to_write, TREE_NODE_TYPE* tree,
		      int indent_level);

int create_xml_results(FILE* results_file, SIM_CARD* sim_card);
