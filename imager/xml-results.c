/******************************************************************************
 *
 * NAME:          xml-results.c
 *
 * AUTHORS:       Fabio Casadei
 *                fabio.casadei@mercurio-ws.com
 *
 *                Antonio Savoldi
 *                antonio.savoldi@ing.unibs.it
 *
 * FILE TYPE:     C source
 *
 * DESCRIPTION:   this is the file where functions for creating an xml file of
 *                the results reside.
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "utils.h"
#include "gsm-constants.h"
#include "dirtree.h"
#include "debug.h"



/*************************
 *                       *
 *   UTILITY FUNCTIONS   *
 *                       *
 *************************/


//
// FUNCTION NAME: indent
//
// DESCRIPTION: this function writes on the xml output file a number of TAB
//              characters equal to that specified by the value passed as
//              parameter.
//
// INPUT PARAMETERS:
//   int    indent         the number of TAB charachters to write.
//
// OUTPUT PARAMETERS:
//   None.
//
// NOTES:
//   None.
//
void indent(FILE* output_file, int indent_level) {

  int i;

  for(i=0; i<indent_level; i++) {
    	fputs("\t", output_file);
  }
}


//
// FUNCTION NAME: write_tag
//
// DESCRIPTION: this function writes on the xml output file the specified tag,
//              with the specified indentation.
//
// INPUT PARAMETERS:
//   FILE*    output_file         the xml formatted output file.
//   int      indent_level        level of indentation
//   char*    tag_name            the name of the tag
//   int      closure_tag         0 if an opening tag is needed, 1 if a closure
//                                tag is needed.
//
// OUTPUT PARAMETERS:
//   None.
//
// NOTES:
//   Tag attributes are passed toghether with tag name. A better
//   implementation would receive tag attributes as separate list.
//
void write_tag(FILE* output_file, int indent_level,
               char* tag_name, int closure_tag) {

  indent(output_file, indent_level);
  if(closure_tag) {
    	fprintf(output_file,"</");
  }
  else {
    	fprintf(output_file,"<");
  }
  fprintf(output_file,"%s>\n",tag_name);

}



//
// FUNCTION NAME: hex_filter
//
// DESCRIPTION: this function, given an hexadecimal number, returns a string
//              that represent it, formatted on two digits.
//
// INPUT PARAMETERS:
//   BYTE    hex_number         number to format
//
// OUTPUT PARAMETERS: char*
//   String representation of the input parameter.
//
// NOTES:
//   None.
//
char* hex_filter(BYTE hex_number) {

  char* filtered_num;

  if(hex_number > 0xF) {
    	//filtered_num = strcat(filtered_num, atoi(hex_number));
  }
  else {
    	filtered_num = "0";
   	//filtered_num = strcat(filtered_num, atoi(hex_number));
  }

  return(filtered_num);

}



//
// FUNCTION NAME: get_SWs_from_resp
//
// DESCRIPTION: this function returns the the status words SW1 and SW2,
//              appended to a response from the SIM. This is useful, in
//              combination to header and body extraction, to know if the
//              computation that led to any header or body was eventually
//              affected by some anomalies.
//
// INPUT PARAMETERS:
//   RESPONSE* resp           raw response from which status words can be
//                            extracted.
//
// OUTPUT PARAMETERS:  RESPONSE*
//   The status words appended to the response.
//
// NOTES:
//   None.
//
RESPONSE* get_SWs_from_resp (RESPONSE* resp) {

  int i;
  RESPONSE* response = create_b_list();

  for(i=((*resp).b_list_length - 2); i<(*resp).b_list_length; i++){
    	blist_add_element(response, blist_get_element(resp,i));
  }

  return(response);
}


//
// FUNCTION NAME: clean_resp_from_SWs
//
// DESCRIPTION: this function returns a response from the SIM card, cleaned
//              from the status words SW1 and SW2. This is useful for header
//              and body storage purposes.
//
// INPUT PARAMETERS:
//   RESPONSE* resp           raw response to clean.
//
// OUTPUT PARAMETERS:  RESPONSE*
//   The cleaned response.
//
// NOTES:
//   None.
//
RESPONSE* clean_resp_from_SWs (RESPONSE* resp) {

  int i;
  RESPONSE* response = create_b_list();

  for(i=0; i<((*resp).b_list_length - 2); i++){
  	  blist_add_element(response, blist_get_element(resp,i));
  }

  return(response);
}


//
// FUNCTION NAME: write_content
//
// DESCRIPTION: this function writes on the xml output file the specified
//              content, that is a list of bytes, with the specified
//              indentation. Besides this, it writes the status words
//              appended to the response, so that further information is
//              available to understand, when something goes wrong, what the
//              cause was.
//
// INPUT PARAMETERS:
//   FILE*      output_file         the xml formatted output file.
//   int        indent_level        level of indentation
//   RESPONSE*  resp                response obtained from the sim.
//   char*      tag_name            the name of the tag surrounding the
//                                  content present in the response
//   char*      attributes          additional attributes eventually required
//                                  with the tag
//
// OUTPUT PARAMETERS:
//   None.
//
// NOTES:
//   This function is useful wherever a response from SIM is involved. In fact,
//
//   implementation would receive tag attributes as separate list.
//
void write_content(FILE* output_file, int indent_level, RESPONSE* resp,
		   char* tag_name, char* attributes) {

  RESPONSE* sws;
  RESPONSE* content;
  int i;
  int ind_lev = indent_level;
  char* attr_app = malloc(100*sizeof(char));

  // First i divide the real content from the status words

  sws = get_SWs_from_resp(resp);

  content = clean_resp_from_SWs(resp);

  // Then, i open the tag with the appropriate attributes and increment
  // indentation
  if(!strcmp(attributes,"")) {
    	sprintf(attr_app,"%s %s", tag_name, attributes);
    	write_tag(output_file, ind_lev, attr_app, 0);
  }
  else {
    	write_tag(output_file, ind_lev, tag_name, 0);
  }
  ind_lev++;

  // Third, i write the status words in the output file positioning them into
  // the appropriate file

  write_tag(output_file, ind_lev, "sw", 0);
  indent(output_file, ind_lev);
  for(i=0; i<(*sws).b_list_length; i++) {
    	fprintf(output_file, "%.2X ", blist_get_element(sws,i));
  }
  fputs("\n",output_file);
  write_tag(output_file, ind_lev, "sw", 1);

  // Fourth, i write the content surrounded by the appropriate tag

  write_tag(output_file, ind_lev, "content", 0);
  indent(output_file, ind_lev);
  for(i=0; i<(*content).b_list_length; i++) {
    	fprintf(output_file, "%.2X ", blist_get_element(content,i));
  }
  fputs("\n",output_file);
  write_tag(output_file, ind_lev, "content", 1);

  // Finally, i close the high level tag
  ind_lev--;
  write_tag(output_file, ind_lev, tag_name, 1);

}


//
// FUNCTION NAME: xml_convert_ef_transp
//
// DESCRIPTION: given a node representing an EF, this function translate it
//              into the XML representation and write that into the output file
//
// INPUT PARAMETERS:
//
//   FILE*           file_to_write     this is the file where the xml
//                                     formatted results must be written.
//
//   int             indent_level      making the written results
//                                     human-readable is easily achieved with
//                                     the indentation of the tags.
//
//   TREE_NODE_TYPE* node       that's the node representing the subtree that
//                              is currently being built. For this function is
//                              recursive, first time it is called this
//                              parameter represent the root of the directory
//                              tree (the Master File).
//
// OUTPUT PARAMETERS:
//   None.
//
// NOTES:
//   None.
//
void xml_convert_ef_transp(FILE* file_to_write, int indent_level,
			   TREE_NODE_TYPE* node) {

  int i;
  char * attributes = malloc(50*sizeof(char));

  write_tag(file_to_write, indent_level, "ef", 0);
  indent_level++;

  write_tag(file_to_write, indent_level, "header", 0);
  indent(file_to_write, indent_level);
  for(i=0; i<(*node).p_raw_header.b_list_length; i++) {
    	fprintf(file_to_write, "%.2X ", blist_get_element(&(*node).p_raw_header,i));
  }
  fputs("\n",file_to_write);
  write_tag(file_to_write, indent_level, "header", 1);

  write_content(file_to_write, indent_level, &((*node).p_raw_content_tEF), "body","");

  indent_level--;
  write_tag(file_to_write, indent_level, "ef", 1);
}



//
// FUNCTION NAME: xml_convert_ef_linfixcyc
//
// DESCRIPTION: given the entire filesystem extracted from the SIM card, this
//              function converts it from the internal representation to the
//              XML representation.
//
// INPUT PARAMETERS:
//
//   FILE*           file_to_write     this is the file where the xml
//                                     formatted results must be written.
//
//   int             indent_level      making the written results
//                                     human-readable is easily achieved with
//                                     the indentation of the tags.
//
//   TREE_NODE_TYPE* node       that's the node representing the subtree that
//                              is currently being built. For this function is
//                              recursive, first time it is called this
//                              parameter represent the root of the directory
//                              tree (the Master File).
//
// OUTPUT PARAMETERS:
//   None.
//
// NOTES:
//   None.
//
void xml_convert_ef_linfixcyc(FILE* file_to_write, int indent_level,
			   TREE_NODE_TYPE* node, int linfix) {

  RECORD* aux;
  int i,j,rec_length;
  char* attributes = malloc(50*sizeof(char));

  if(linfix) {
    	//write_tag(file_to_write, indent_level, "ef type=\"linear fixed\"", 0);
    	write_tag(file_to_write, indent_level, "ef", 0);
    	indent_level++;
  }
  else {
    	//write_tag(file_to_write, indent_level, "ef type=\"cyclic\"", 0);
    	write_tag(file_to_write, indent_level, "ef", 0);
    	indent_level++;
  }

  write_tag(file_to_write, indent_level, "header", 0);
  indent(file_to_write, indent_level);
  for(i=0; i<(*node).p_raw_header.b_list_length; i++) {
    	fprintf(file_to_write, "%.2X ", blist_get_element(&(*node).p_raw_header,i));
  }
  fputs("\n",file_to_write);
  write_tag(file_to_write, indent_level, "header", 1);

  write_tag(file_to_write, indent_level, "body", 0);
  indent_level++;

  for(i=0; i<(*node).p_raw_content_lfcEF.rec_list_length; i++) {
    aux = reclist_get_element(&((*node).p_raw_content_lfcEF), i);
    sprintf(attributes, "number = %2i",i);
    write_content(file_to_write, indent_level, aux, "record", attributes);
  }

  indent_level--;
  write_tag(file_to_write, indent_level, "body", 1);

  indent_level--;
  write_tag(file_to_write, indent_level, "ef", 1);
}



//
// FUNCTION NAME: xml_convert_tree
//
// DESCRIPTION: given the entire filesystem extracted from the SIM card, this
//              function converts it from the internal representation to the
//              XML representation.
//
// INPUT PARAMETERS:
//
//   FILE*           file_to_write     this is the file where the xml
//                                     formatted results must be written.
//
//   TREE_NODE_TYPE* root       that's the node representing the subtree that
//                              is currently being built. For this function is
//                              recursive, first time it is called this
//                              parameter represent the root of the directory
//                              tree (the Master File).
//
//   int             indent_level      making the written results
//                                     human-readable is easily achieved with
//                                     the indentation of the tags.
//
// OUTPUT PARAMETERS:
//   None.
//
// NOTES:
//   None.
//
void xml_convert_tree(FILE* file_to_write, TREE_NODE_TYPE* tree,
		      int indent_level) {

  int i;

  switch((*tree).file_type) {
  case FTYP_EF: {

    	if((*tree).firstborn != NULL){
      		//printf("\nDEBUG: xml_convert_tree: EF cannot have sons! Something wrong happened.\n");
      		exit(1);
    	}
    	switch(blist_get_element(&(*tree).p_raw_header, 13)) {
    		case FSTR_TRANSP: {
      			xml_convert_ef_transp(file_to_write, indent_level, tree);
      			break;
    		}
    		case FSTR_LINFIX : {
      			xml_convert_ef_linfixcyc(file_to_write, indent_level, tree, 1);
      			break;
    		}
    		case FSTR_CYCLIC: {
      			xml_convert_ef_linfixcyc(file_to_write, indent_level, tree, 0);
      			break;
    		}
    		default: {
      			//printf("\nDEBUG: xml_convert_tree: node represent an EF of an unknown type!\n");
      			exit(1);
    		}
    	}

    	if((*tree).next_brother != NULL){
      		xml_convert_tree(file_to_write, (*tree).next_brother, indent_level);
    	}

    	break;
  }
  case FTYP_DF: {
    	indent(file_to_write, indent_level);
    	fprintf(file_to_write, "<df>\n");

    	indent_level++;

    	//writing of df header to output file
    	write_tag(file_to_write, indent_level, "header", 0);
    	indent(file_to_write, indent_level);
    	for(i=0; i<(*tree).p_raw_header.b_list_length; i++) {
      		fprintf(file_to_write, "%.2X ",
	      		blist_get_element(&(*tree).p_raw_header,i));
    	}
    	fputs("\n",file_to_write);
    	write_tag(file_to_write, indent_level, "header", 1);

    	if((*tree).firstborn != NULL) {
      		xml_convert_tree(file_to_write, (*tree).firstborn, indent_level);
    	}

    	indent_level--;
    	write_tag(file_to_write, indent_level,  "df", 1);

    	if((*tree).next_brother != NULL) {
      		xml_convert_tree(file_to_write, (*tree).next_brother, indent_level);
    	}
    	break;
  }
  case FTYP_MF: {
    	indent(file_to_write, indent_level);
    	fprintf(file_to_write, "<mf>\n");

    	indent_level++;

    	//writing of mf header to output file
    	write_tag(file_to_write, indent_level, "header", 0);
    	indent(file_to_write, indent_level);
    	for(i=0; i<(*tree).p_raw_header.b_list_length; i++) {
      		fprintf(file_to_write, "%.2X ",
	      	blist_get_element(&(*tree).p_raw_header,i));
    	}
    	fputs("\n",file_to_write);
    	write_tag(file_to_write, indent_level, "header", 1);

    	if((*tree).firstborn != NULL){
      		xml_convert_tree(file_to_write, (*tree).firstborn, indent_level);
    	}

    	indent_level--;
    	write_tag(file_to_write, indent_level, "mf", 1);

    	if((*tree).next_brother != NULL){
      		//printf("\nDEBUG: xml_convert_tree: MF cannot have brothers! Something wrong happened.\n");
      		exit(1);
    	}
    	break;
  }
  default: {
    	//printf("\nDEBUG: xml_convert_tree: node represent an unknown type of file!\n");
    	exit(1);
  }
  }

}


//
// FUNCTION NAME: create_xml_results
//
// DESCRIPTION: given the internal representation, as a tree of pointers, of
//              all the data stored into the SIM under examination, this
//              function translate it into the xml representation required by the
//              tools that operate at the examination level.
//
// INPUT PARAMETERS:
//   FILE* results_file      the file where the xml formatted results
//                                     will be written.
//   TREE_NODE_TYPE* seized_card       the internal representation of SIM data.
//
// OUTPUT PARAMETERS:
//   None.
//
// NOTES:
//   None.
//
void create_xml_results(FILE* results_file, SIM_CARD* sim_card) {

  int i;

  // xml file header, with informations such as xml version and reference to
  // the DTD, should be created here.
  // For now, just the core xml is produced as output.

  //fputs("<--! File automatically generated by SIMbrush tool -->",
  //	results_file);
  //fputs("\n\n",results_file);

  write_tag(results_file, 0, "simcard level=\"raw\"", 0);

  // ATR information
  write_tag(results_file, 1, "atr", 0);
  indent(results_file,1);
  for(i=0; i<(*(*sim_card).raw_atr).b_list_length; i++) {
    	fprintf(results_file, "%.2X ", blist_get_element((*sim_card).raw_atr,i));
  }
  fputs("\n", results_file);
  write_tag(results_file, 1, "atr", 1);

  //recursive_function
  xml_convert_tree(results_file, (*sim_card).root, 1);

  write_tag(results_file, 0, "simcard", 1);

}
