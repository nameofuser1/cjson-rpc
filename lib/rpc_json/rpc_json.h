/*
 * rpc_json.hpp
 *
 *  Created on: Sep 19, 2018
 *      Author: Kamnev Yuriy
 */

#ifndef RPC_JSON_H_
#define RPC_JSON_H_


#include <stdbool.h>
#include "cJSON/cJSON.h"


#ifdef __cpluplus
extern "C" {
#endif


typedef enum {
    RPC_JSON_REQUEST=0,
    RPC_JSON_RESPONSE,
    RPC_JSON_NOTIFICATION,
    RPC_JSON_ERROR,
    RPC_JSON_WRONG_OBJECT
} RPCJson_type_t;


typedef struct RPCJson {
    cJSON *rpc_json_obj;

    RPCJson_type_t type;
} RPCJson_t;


bool rpc_json_create_request(RPCJson_t *obj, const char *method, int id);
bool rpc_json_create_response(RPCJson_t *obj, int id, int result);
bool rpc_json_create_error(RPCJson_t *obj, int id, int code, const char *msg);
bool rpc_json_create_notification(RPCJson_t *obj, const char *method);
bool rpc_json_add_parameter_string(RPCJson_t *obj, const char *name, const char *value);
bool rpc_json_add_parameter_int(RPCJson_t *obj, const char *name, int value);
char *rpc_json_print(RPCJson_t *obj);

bool rpc_json_parse(RPCJson_t *obj, const char *str);
RPCJson_type_t rpc_json_get_obj_type(RPCJson_t *obj);
const char* rpc_json_get_method(RPCJson_t *obj);
bool rpc_json_get_id(RPCJson_t *obj, int *id);
bool rpc_json_get_result(RPCJson_t *obj, int *res);
bool rpc_json_get_error_code(RPCJson_t *obj, int *err);
const char* rpc_json_get_error_message(RPCJson_t *obj);
bool rpc_json_get_param_int(RPCJson_t *obj, const char *name, int *i);
const char* rpc_json_get_param_string(RPCJson_t *obj, const char *name);
void rpc_json_delete(RPCJson_t *obj);


#ifdef __cplusplus
}
#endif


#endif /* RPC_JSON_H_ */
