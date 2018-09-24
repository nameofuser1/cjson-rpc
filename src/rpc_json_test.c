/*
 * rpc_json_test.c
 *
 *  Created on: Sep 24, 2018
 *      Author: yuriy
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "rpc_json_test.h"
#include "rpc_json/rpc_json.h"


void test_rpc_json(void)
{
	RPCJson_t json;
	RPCJson_t parsed_json;
	char *json_str;
	char *parsed_json_str;
	int result;
	int id;
	int error;
	int int_param;
	char *str_param;
	char *error_msg;

	/*** REQUEST CREATION/PARSING TEST ***/
	assert(rpc_json_create_request(&json, "call", 1));
	assert(rpc_json_add_parameter_int(&json, "param1", -2560));
	assert(rpc_json_add_parameter_string(&json, "param_string", "MYStringParam"));

	assert(rpc_json_get_id(&json, &id));
	assert(rpc_json_get_param_int(&json, "param1", &int_param));
	assert((str_param = (char*)rpc_json_get_param_string(&json, "param_string")) != NULL);

	assert(strcmp(rpc_json_get_method(&json), "call") == 0);
	assert(id == 1);
	assert(int_param == -2560);
	assert(strcmp(str_param, "MYStringParam") == 0);

	json_str = rpc_json_print(&json);

	assert(rpc_json_parse(&parsed_json, json_str));
	parsed_json_str = rpc_json_print(&parsed_json);

	assert(strcmp(json_str, parsed_json_str) == 0);
	assert(rpc_json_get_obj_type(&parsed_json) == RPC_JSON_REQUEST);

	free(json_str);
	free(parsed_json_str);
	rpc_json_delete(&json);
	rpc_json_delete(&parsed_json);

	/*** NOTIFICATION CREATION/PARSING TEST ***/
	assert(rpc_json_create_notification(&json, "notify"));
	assert(rpc_json_add_parameter_int(&json, "int_param", -2561));
	assert(rpc_json_add_parameter_string(&json, "param_string", "OLOLO"));

	assert(rpc_json_get_id(&json, &id) == false);
	assert(rpc_json_get_param_int(&json, "int_param", &int_param));
	assert((str_param = (char*)rpc_json_get_param_string(&json, "param_string")) != NULL);

	assert(strcmp(rpc_json_get_method(&json), "notify") == 0);
	assert(int_param == -2561);
	assert(strcmp(str_param, "OLOLO") == 0);

	json_str = rpc_json_print(&json);

	assert(rpc_json_parse(&parsed_json, json_str));
	parsed_json_str = rpc_json_print(&parsed_json);

	assert(strcmp(json_str, parsed_json_str) == 0);
	assert(rpc_json_get_obj_type(&parsed_json) == RPC_JSON_NOTIFICATION);

	free(json_str);
	free(parsed_json_str);
	rpc_json_delete(&json);
	rpc_json_delete(&parsed_json);


	/*** RESPONSE CREATION/PARSING TEST ***/
	assert(rpc_json_create_response(&json, 0, 245));
	assert(rpc_json_get_id(&json, &id));
	assert(rpc_json_get_result(&json, &result));
	assert(rpc_json_get_error_code(&json, &error) == false);
	assert(rpc_json_get_error_message(&json) == NULL);

	assert(id == 0);
	assert(result == 245);

	json_str = rpc_json_print(&json);

	assert(rpc_json_parse(&parsed_json, json_str));
	parsed_json_str = rpc_json_print(&parsed_json);

	assert(strcmp(json_str, parsed_json_str) == 0);
	assert(rpc_json_get_obj_type(&parsed_json) == RPC_JSON_RESPONSE);

	free(json_str);
	free(parsed_json_str);
	rpc_json_delete(&json);
	rpc_json_delete(&parsed_json);


	/*** ERROR CREATION/PARSING TEST ***/
	assert(rpc_json_create_error(&json, 1, -25, "Error!!!"));
	assert(rpc_json_get_id(&json, &id));
	assert(rpc_json_get_result(&json, &result) == false);
	assert(rpc_json_get_error_code(&json, &error));
	assert((error_msg = (char*)rpc_json_get_error_message(&json)) != NULL);

	assert(id == 1);
	assert(error == -25);
	assert(strcmp(error_msg, "Error!!!") == 0);

	json_str = rpc_json_print(&json);

	assert(rpc_json_parse(&parsed_json, json_str));
	parsed_json_str = rpc_json_print(&parsed_json);

	assert(strcmp(json_str, parsed_json_str) == 0);
	assert(rpc_json_get_obj_type(&parsed_json) == RPC_JSON_ERROR);

	free(json_str);
	free(parsed_json_str);
	rpc_json_delete(&json);
	rpc_json_delete(&parsed_json);

}
