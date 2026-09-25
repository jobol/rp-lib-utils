/*
 * Copyright (C) 2020-2026 IoT.bzh Company
 * Author: José Bollo <jose.bollo@iot.bzh>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, something express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#ifdef	__cplusplus
extern "C" {
#endif

#include <stddef.h>

/**
 * structure for returning the value found
 */
typedef struct {
	/** the value found for the given name */
	const char *value;

	/** length (used if filled) */
	size_t length;

	/** when some action must be taken after use */
	struct {
		/** function to call when the value is used */
		void (*function)(void*);

		/** closure to the function */
		void *closure;
	} dispose;
}
	rp_expand_vars_result_t;

/**
 * Definition of the type of callback functions for @see rp_expand_vars_function
 *
 * The function receives a closure and a name represented by a pointer to
 * characters and a length. Based on that the function returns the value
 * associated to the name in result.
 *
 * @param closure the closure of the function
 * @param name    start pointer for the name
 * @param len     the length in chars of the name
 * @param result the data
 *
 * @return a boolean indicating if found (true) or not (false)
 */
typedef int (*rp_expand_vars_fun_t)(void *closure, const char *name, size_t len, rp_expand_vars_result_t *result);

/**
 * Definition of the type of callback functions for @see rp_expand_vars_function
 *
 * The function receives a closure and a name represented by a pointer to
 * characters and a length. Based on that the function returns the value
 * associated to the name or NULL.
 *
 * @param closure the closure data
 * @param name    start pointer for the name
 * @param len     the length in chars of the name
 *
 * @return the value for the name or NULL if not found
 */
typedef const char *(*rp_expand_vars_cb_t)(void *closure, const char *name, size_t len);

/**
 * Search for the variable in an array of variable definitions
 *
 * @param vars null terminated array of variable definitions
 *             of the form "NAME=VALUE..."
 * @param name begin of the name of the searched variable
 * @param len  length of the searched variable
 *
 * @return a pointer to the value or NULL if the variable is not found
 */
extern const char *rp_expand_vars_search(char **vars, const char *name, size_t len);

/**
 * Search for the variable in the environment. Like getenv but the name has
 * not to be null terminated.
 *
 * @param name begin of the name of the searched variable
 * @param len  length of the searched variable
 *
 * @return a pointer to the value or NULL if the variable is not found
 */
extern const char *rp_expand_vars_search_env(const char *name, size_t len);

/**
 * Return the result of expanding variables of 'value'.
 * When 'value' does not contains variables, returns either NULL (when 'copy' == 0)
 * or a copy of value (when 'copy' != 0).
 *
 * The variables can appear in one of the form ${...} or $(...) or $ALPHA_NUM
 *
 * The resolution of the variables if done by the function.
 *
 * @param value  the string to expand
 * @param copy   behavior in lack of variable (0 return NULL, not zero return copy)
 * @param function function to resolve the variables
 * @param closure closure of the function
 *
 * @return The result of expanding variables of value or NULL if lake of variables and copy == 0
 */
extern char *rp_expand_vars_function(const char *value, int copy, rp_expand_vars_fun_t function, void *closure);

/**
 * Return the result of expanding variables of 'value'.
 * When 'value' does not contains variables, returns either NULL (when 'copy' == 0)
 * or a copy of value (when 'copy' != 0).
 *
 * The variables can appear in one of the form ${...} or $(...) or $ALPHA_NUM
 *
 * The resolution of the variables if done by the function.
 *
 * @param value  the string to expand
 * @param copy   behavior in lack of variable (0 return NULL, not zero return copy)
 * @param function function to resolve the variables
 * @param closure closure of the function
 *
 * @return The result of expanding variables of value or NULL if lake of variables and copy == 0
 */
extern char *rp_expand_vars_callback(const char *value, int copy, rp_expand_vars_cb_t function, void *closure);

/**
 * Return the result of expanding variables of 'value'.
 * When 'value' does not contains variables, returns either NULL (when 'copy' == 0)
 * or a copy of value (when 'copy' != 0).
 *
 * The variables can appear in one of the form ${...} or $(...) or $ALPHA_NUM
 *
 * The resolution of the variables if done by searching in the null terminated array
 * of array of null terminated of variable definitions in the form "NAME=VALUE..."
 *
 * @param value  the string to expand
 * @param copy   behavior in lack of variable (0 return NULL, not zero return copy)
 * @param varsarray
 *
 * @return The result of expanding variables of value or NULL if lake of variables and copy == 0
 */
extern char *rp_expand_vars_array(const char *value, int copy, char ***varsarray);

/**
 * Return the result of expanding variables of 'value'.
 * When 'value' does not contains variables, returns either NULL (when 'copy' == 0)
 * or a copy of value (when 'copy' != 0).
 *
 * The variables can appear in one of the form ${...} or $(...) or $ALPHA_NUM
 *
 * The resolution of the variables if done by searching in the null terminated array
 * of variable definitions in the form "NAME=VALUE..."
 *
 * @param value  the string to expand
 * @param copy   behavior in lack of variable (0 return NULL, not zero return copy)
 * @param vars   null terminated array of definitions "NAME=VALUE..."
 *
 * @return The result of expanding variables of value or NULL if lake of variables and copy == 0
 */
extern char *rp_expand_vars_only(const char *value, int copy, char **vars);

/**
 * Return the result of expanding variables of 'value'.
 * When 'value' does not contains variables, returns either NULL (when 'copy' == 0)
 * or a copy of value (when 'copy' != 0).
 *
 * The variables can appear in one of the form ${...} or $(...) or $ALPHA_NUM
 *
 * The resolution of the variables if done using the environment
 *
 * @param value  the string to expand
 * @param copy   behavior in lack of variable (0 return NULL, not zero return copy)
 *
 * @return The result of expanding variables of value or NULL if lake of variables and copy == 0
 */
extern char *rp_expand_vars_env_only(const char *value, int copy);

/**
 * Return the result of expanding variables of 'value'.
 * When 'value' does not contains variables, returns either NULL (when 'copy' == 0)
 * or a copy of value (when 'copy' != 0).
 *
 * The variables can appear in one of the form ${...} or $(...) or $ALPHA_NUM
 *
 * The resolution of the variables if done by searching
 *   1. in the null terminated array 'before'
 *   2. in the environment
 *   3. in the null terminated array 'after'
 *
 * @param value  the string to expand
 * @param copy   behavior in lack of variable (0 return NULL, not zero return copy)
 * @param before null terminated array of definitions "NAME=VALUE..."
 * @param after  null terminated array of definitions "NAME=VALUE..."
 *
 * @return The result of expanding variables of value or NULL if lake of variables and copy == 0
 */
extern char *rp_expand_vars(const char *value, int copy, char **before, char **after);

/**
 * Return the result of expanding variables of 'value'.
 * When 'value' does not contains variables, returns either NULL (when 'copy' == 0)
 * or a copy of value (when 'copy' != 0).
 *
 * The variables can appear in one of the form ${...} or $(...) or $ALPHA_NUM
 *
 * The resolution of the variables if done by searching
 *   1. in the null terminated array 'vars'
 *   2. in the environment
 *
 * @param value  the string to expand
 * @param copy   behavior in lack of variable (0 return NULL, not zero return copy)
 * @param vars   null terminated array of definitions "NAME=VALUE..."
 *
 * @return The result of expanding variables of value or NULL if lake of variables and copy == 0
 */
extern char *rp_expand_vars_first(const char *value, int copy, char **vars);

/**
 * Return the result of expanding variables of 'value'.
 * When 'value' does not contains variables, returns either NULL (when 'copy' == 0)
 * or a copy of value (when 'copy' != 0).
 *
 * The variables can appear in one of the form ${...} or $(...) or $ALPHA_NUM
 *
 * The resolution of the variables if done by searching
 *   1. in the environment
 *   2. in the null terminated array 'vars''
 *
 * @param value  the string to expand
 * @param copy   behavior in lack of variable (0 return NULL, not zero return copy)
 * @param vars   null terminated array of definitions "NAME=VALUE..."
 *
 * @return The result of expanding variables of value or NULL if lake of variables and copy == 0
 */
extern char *rp_expand_vars_last(const char *value, int copy, char **vars);

#ifdef	__cplusplus
}
#endif
