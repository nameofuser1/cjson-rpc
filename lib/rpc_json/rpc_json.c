/*
 * rpc_json.cpp
 *
 *  Created on: Sep 19, 2018
 *      Author: Kamnev Yuriy
 */
#include <string.h>

#include "rpc_json.h"

#define JSONRPC_ITEM_NAME           "json_rpc"
#define JSONRPC_ITEM_NAME_VALUE     "2.0"
#define METHOD_ITEM_NAME            "method"
#define ID_ITEM_NAME                "id"
#define PARAMETERS_ITEM_NAME        "params"
#define RESULT_ITEM_NAME            "result"
#define ERROR_ITEM_NAME             "error"
#define ERROR_CODE_ITEM_NAME        "code"
#define ERROR_MSG_ITEM_NAME         "message"

#define RETURN_ON_NULL(v, ret_val)	{if(v == NULL) {return ret_val;}}
#define GET_ITEM(rpc_json, i)       (cJSON_GetObjectItem(rpc_json->rpc_json_obj, i))
#define SET_ITEM(rpc_json, s, i)    (cJSON_AddItemToObject(rpc_json->rpc_json_obj, s, i))



/****************** FORWARD DECLARATIONS *************************/
static cJSON* get_create_params(RPCJson_t *obj);


/********************* PUBLIC API ***************************/


/**
 * @brief		Creates RPC-JSON request. Add parameters using rpc_json_add_parameter_int/string functions.
 * @param[out]	obj		-	created object handle
 * @param[in]	method	-	value of a request method field.
 * @param[in]	id		-	value of a request id field
 *
 * @return 		True if request is successfully created. False otherwise.
 */
bool rpc_json_create_request(RPCJson_t *obj, const char *method, int id)
{
    obj->type = RPC_JSON_REQUEST;

    obj->rpc_json_obj = cJSON_CreateObject();
    RETURN_ON_NULL(obj->rpc_json_obj, false);

    cJSON *json_rpc_item = cJSON_CreateString(JSONRPC_ITEM_NAME_VALUE);
    RETURN_ON_NULL(json_rpc_item, false);

    cJSON *method_item = cJSON_CreateString(method);
    RETURN_ON_NULL(method_item, false);

    cJSON *id_item = cJSON_CreateNumber(id);
    RETURN_ON_NULL(id_item, false);

    SET_ITEM(obj, JSONRPC_ITEM_NAME, json_rpc_item);
    SET_ITEM(obj, METHOD_ITEM_NAME, method_item);
    SET_ITEM(obj, ID_ITEM_NAME, id_item);

    return true;
}

/**
 * @brief		Creates RPC-JSON response.
 * @param[out]	obj		-	created object handle
 * @param[in]	id		-	value of a response id field
 * @param[in]	result	-	value of a response result field
 *
 * @return 		True if request is successfully created. False otherwise.
 */
bool rpc_json_create_response(RPCJson_t *obj, int id, int result)
{
    obj->type = RPC_JSON_RESPONSE;

    obj->rpc_json_obj = cJSON_CreateObject();
    RETURN_ON_NULL(obj->rpc_json_obj, false);

    cJSON *json_rpc_item = cJSON_CreateString(JSONRPC_ITEM_NAME_VALUE);
    RETURN_ON_NULL(json_rpc_item, false);

    cJSON *id_item = cJSON_CreateNumber(id);
    RETURN_ON_NULL(id_item, false);

    cJSON *result_item = cJSON_CreateNumber(result);
    RETURN_ON_NULL(result_item, false);

    SET_ITEM(obj, JSONRPC_ITEM_NAME, json_rpc_item);
    SET_ITEM(obj, ID_ITEM_NAME, id_item);
    SET_ITEM(obj, RESULT_ITEM_NAME, result_item);

    return true;
}

/**
 * @brief		Creates RPC-JSON error object.
 * @param[out]	obj		-	created object handle
 * @param[in]	id		-	value of a error id field
 * @param[in]	code	-	error code field value
 * @param[in]	msg		-	error message field value
 *
 * @return 		True if request is successfully created. False otherwise.
 */
bool rpc_json_create_error(RPCJson_t *obj, int id, int code, const char *msg)
{
    obj->type = RPC_JSON_ERROR;

    obj->rpc_json_obj = cJSON_CreateObject();
    RETURN_ON_NULL(obj->rpc_json_obj, false);

    cJSON *json_rpc_item = cJSON_CreateString(JSONRPC_ITEM_NAME_VALUE);
    RETURN_ON_NULL(json_rpc_item, false);

    cJSON *id_item = cJSON_CreateNumber(id);
    RETURN_ON_NULL(id_item, false);

    cJSON *error_item = cJSON_CreateObject();
    RETURN_ON_NULL(error_item, false);

    cJSON *err_code_item = cJSON_CreateNumber(code);
    RETURN_ON_NULL(err_code_item, false);

    cJSON *err_msg_item = cJSON_CreateString(msg);
    RETURN_ON_NULL(err_msg_item, false);

    cJSON_AddItemToObject(error_item, ERROR_CODE_ITEM_NAME, err_code_item);
    cJSON_AddItemToObject(error_item, ERROR_MSG_ITEM_NAME, err_msg_item);

    SET_ITEM(obj, JSONRPC_ITEM_NAME, json_rpc_item);
    SET_ITEM(obj, ID_ITEM_NAME, id_item);
    SET_ITEM(obj, ERROR_ITEM_NAME, error_item);

    return true;
}


/**
 * @brief		Creates RPC-JSON notification object. Add parameters using rpc_json_add_parameter_int/string functions.
 * @param[out]	obj		-	created object handle
 * @param[in]	method	-	notification method field value.
 *
 * @return 		True if request is successfully created. False otherwise.
 */
bool rpc_json_create_notification(RPCJson_t *obj, const char *method)
{
    obj->type = RPC_JSON_NOTIFICATION;

    obj->rpc_json_obj = cJSON_CreateObject();
    RETURN_ON_NULL(obj->rpc_json_obj, false);

    cJSON *json_rpc_item = cJSON_CreateString(JSONRPC_ITEM_NAME_VALUE);
    RETURN_ON_NULL(json_rpc_item, false);

    cJSON *method_item = cJSON_CreateString(method);
    RETURN_ON_NULL(method_item, false);

    SET_ITEM(obj, JSONRPC_ITEM_NAME, json_rpc_item);
    SET_ITEM(obj, METHOD_ITEM_NAME, method_item);

    return true;
}

/**
 * @brief		Adds string parameter to a Request/Notification object
 * @param[out]	obj		-	object handle
 * @param[in]	name	-	name of the parameter
 * @param[in]	value	-	value of the parameter
 *
 * @return 		True if parameter is successfully added. False otherwise.
 */
bool rpc_json_add_parameter_string(RPCJson_t *obj, const char *name, const char *value)
{
    cJSON *params = get_create_params(obj);
    RETURN_ON_NULL(params, false);

    cJSON *param_str = cJSON_CreateString(value);
    RETURN_ON_NULL(param_str, false);

    cJSON_AddItemToObject(params, name, param_str);

    return true;
}

/**
 * @brief		Adds integer parameter to a Request/Notification object
 * @param[out]	obj		-	object handle
 * @param[in]	name	-	name of the parameter
 * @param[in]	value	-	value of the parameter
 *
 * @return 		True if parameter is successfully added.
 */
bool rpc_json_add_parameter_int(RPCJson_t *obj, const char *name, int value)
{
    cJSON *params = get_create_params(obj);
    RETURN_ON_NULL(params, false);

    cJSON *param_int = cJSON_CreateNumber(value);
    RETURN_ON_NULL(param_int, false);

    cJSON_AddItemToObject(params, name, param_int);

    return true;
}

/**
 * @brief		Get string representation of the RPC-JSON call.
 * @param[in]	obj		-	object handle
 *
 * @return 		If string representations is successfully created returns
 * 				pointer to a char string which MUST BE CLEARED from the outside.
 * 				Otherwise return NULL.
 */
char *rpc_json_print(RPCJson_t *obj)
{
	return cJSON_Print(obj->rpc_json_obj);
}


/**
 * @brief		Creates rpc-json object from a given string.
 * @param[out]	obj		-	object handle
 * @param[in]	str		-	string representation of a RPC-JSON call.
 *
 * @return 		true if successfully parsed. false otherwise.
 */
bool rpc_json_parse(RPCJson_t *obj, const char *str)
{
    obj->rpc_json_obj = cJSON_Parse(str);
    RETURN_ON_NULL(obj->rpc_json_obj, false);

    cJSON *rpc = GET_ITEM(obj, JSONRPC_ITEM_NAME);
    cJSON *method = GET_ITEM(obj, METHOD_ITEM_NAME);
    cJSON *id = GET_ITEM(obj, ID_ITEM_NAME);
    cJSON *params = GET_ITEM(obj, PARAMETERS_ITEM_NAME);
    cJSON *error = GET_ITEM(obj, ERROR_ITEM_NAME);
    cJSON *result = GET_ITEM(obj, RESULT_ITEM_NAME);

    /**
     * Check for required jsonrpc field
     */
    RETURN_ON_NULL(rpc, false);
    if(strcmp(cJSON_GetStringValue(rpc), JSONRPC_ITEM_NAME_VALUE) != 0)
    {
        return false;
    }

    /**
     * Check whether this is Error response.
     * Must contain three objects:
     *  1. jsonprc
     *  2. error
     *  3. id
     */
    if(error != NULL)
    {
        if(method != NULL || params != NULL || result != NULL || id == NULL)
        {
            return false;
        }

        /**
         * Validate error object structure.
         * Must contains 2 required fields and 1 optional:
         *  1. code
         *  2. message
         *  3. data[opt]
         */
        cJSON *err_code = cJSON_GetObjectItem(error, ERROR_CODE_ITEM_NAME);
        cJSON *err_msg = cJSON_GetObjectItem(error, ERROR_MSG_ITEM_NAME);

        if(err_code == NULL || err_msg == NULL)
        {
            return false;
        }

        obj->type = RPC_JSON_ERROR;
        return true;
    }

    /**
     * Check whether it is a response object
     * Must contain three objects:
     *  1. jsonrpc
     *  2. result
     *  3. id
     */
    else if(result != NULL)
    {
        if(method != NULL || params != NULL || id == NULL)
        {
            return false;
        }

        obj->type = RPC_JSON_RESPONSE;
        return true;
    }

    /**
     *  Check whether it is a Request object
     *  Must contain 3 field and 1 optional:
     *      1. jsonrpc
     *      2. method
     *      3. id
     *      4. params[opt]
     */
    else if(id != NULL)
    {
        if(method == NULL || id == NULL)
        {
            return false;
        }

        obj->type = RPC_JSON_REQUEST;
        return true;
    }

    /**
     * Check whether it is a Notification object
     * Must contain 2 fields and 1 optional:
     *  1. jsonrpc
     *  2. method
     *  3. params[opt]
     */
    else if(method != NULL)
    {
        obj->type = RPC_JSON_NOTIFICATION;
        return true;
    }

    /**
     * Get here only in case of a wrong object
     */
    return false;
}

/**
 * @brief		Get object type.
 * @param[in]	obj		-	object handle
 *
 * @return 		RPC-JSON object type.
 */
RPCJson_type_t rpc_json_get_obj_type(RPCJson_t *obj)
{
    return obj->type;
}

/**
 * @brief		Get request/notification method field value.
 * @param[in]	obj		-	object handle
 *
 * @return 		pointer to a method string value. NULL if not found.
 */
const char *rpc_json_get_method(RPCJson_t *obj)
{
    if(obj->type != RPC_JSON_REQUEST && obj->type != RPC_JSON_NOTIFICATION)
    {
        return NULL;
    }

    cJSON *method_item = GET_ITEM(obj, METHOD_ITEM_NAME);
    RETURN_ON_NULL(method_item, NULL);

    const char *v = cJSON_GetStringValue(method_item);
    return v;
}

/**
 * @brief		Get request/response(error) id field.
 * @param[in]	obj		-	object handle.
 * @param[out]	id 		- 	pointer to where value will be saved.
 *
 * @return 		true in case of value found. false otherwise.
 */
bool rpc_json_get_id(RPCJson_t *obj, int *id)
{
    if(obj->type != RPC_JSON_REQUEST && obj->type != RPC_JSON_ERROR && obj->type != RPC_JSON_RESPONSE)
    {
        return false;
    }

    cJSON *id_item = GET_ITEM(obj, ID_ITEM_NAME);
    RETURN_ON_NULL(id_item, false);

    *id = id_item->valueint;
    return true;
}

/**
 * @brief		Get response result field.
 * @param[in]	obj		-	object handle.
 * @param[out]	res 	- 	pointer to where value will be saved.
 *
 * @return 		true in case of value found. false otherwise.
 */
bool rpc_json_get_result(RPCJson_t *obj, int *res)
{
	if(obj->type != RPC_JSON_RESPONSE)
	{
		return false;
	}

	cJSON *result_item = GET_ITEM(obj, RESULT_ITEM_NAME);
	RETURN_ON_NULL(result_item, false);

	*res = result_item->valueint;

	return true;
}

/**
 * @brief		Get error object 'code' field value,
 * @param[in]	obj		-	object handle.
 * @param[out]	err 	- 	pointer to where value will be saved.
 *
 * @return 		true in case of value found. false otherwise.
 */
bool rpc_json_get_error_code(RPCJson_t *obj, int *err)
{
	if(obj->type != RPC_JSON_ERROR)
	{
		return false;
	}

	cJSON *err_item = GET_ITEM(obj, ERROR_ITEM_NAME);
	RETURN_ON_NULL(err_item, false);

	cJSON *err_code_item = cJSON_GetObjectItem(err_item, ERROR_CODE_ITEM_NAME);
	RETURN_ON_NULL(err_code_item, false);

	*err = err_code_item->valueint;

	return true;
}

/**
 * @brief		Get error object 'message' field value,
 * @param[in]	obj		-	object handle.
 *
 * @return 		pointer to a string in case of value found. NULL otherwise.
 */
const char* rpc_json_get_error_message(RPCJson_t *obj)
{
	if(obj->type != RPC_JSON_ERROR)
	{
		return NULL;
	}

	cJSON *err_item = GET_ITEM(obj, ERROR_ITEM_NAME);
	RETURN_ON_NULL(err_item, false);

	cJSON *err_msg_item = cJSON_GetObjectItem(err_item, ERROR_MSG_ITEM_NAME);
	RETURN_ON_NULL(err_msg_item, false);

	return cJSON_GetStringValue(err_msg_item);
}

/**
 * @brief		Get integer parameter from a 'params' field.
 * @param[in]	obj		-	object handle
 * @param[in]	name	-	name of the parameter
 * @param[out]	i		-	pointer to where parameter value will be saved.
 *
 * @return 		true if parameter is found. false otherwise.
 */
bool rpc_json_get_param_int(RPCJson_t *obj, const char *name, int *i)
{
    cJSON *params = GET_ITEM(obj, PARAMETERS_ITEM_NAME);
    RETURN_ON_NULL(params, false);

    cJSON *param_item = cJSON_GetObjectItem(params, name);
    RETURN_ON_NULL(param_item, false);

    if(!cJSON_IsNumber(param_item))
    {
        return false;
    }

    *i = param_item->valueint;
    return true;
}

/**
 * @brief		Get string parameter from a 'params' field.
 * @param[in]	obj		-	object handle
 * @param[in]	name	-	name of the parameter
 *
 * @return 		pointer to a string if parameter is found. false otherwise.
 */
const char* rpc_json_get_param_string(RPCJson_t *obj, const char *name)
{
    cJSON *params = GET_ITEM(obj, PARAMETERS_ITEM_NAME);
    RETURN_ON_NULL(params, NULL);

    cJSON *param_item = cJSON_GetObjectItem(params, name);
    RETURN_ON_NULL(param_item, NULL);

    return cJSON_GetStringValue(param_item);
}


/**
 * @brief		Deletes rpc-json object
 *
 * @param[in]	obj	- object handle to delete
 */
void rpc_json_delete(RPCJson_t *obj)
{
    cJSON_Delete(obj->rpc_json_obj);
}



/**************** PRIVATE ********************/

/**
 * @brief	Get 'params' field or create one if does not exist.
 *
 * @return	pointer to 'params' object in case of successful allocation. NULL otherwise.
 */
static cJSON* get_create_params(RPCJson_t *obj)
{
    cJSON *params = GET_ITEM(obj, PARAMETERS_ITEM_NAME);
    if(params == NULL)
    {
        params = cJSON_CreateObject();
        SET_ITEM(obj, PARAMETERS_ITEM_NAME, params);
    }

    return params;
}

