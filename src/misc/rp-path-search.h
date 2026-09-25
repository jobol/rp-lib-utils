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
* structure for searching in path list
*/
typedef struct rp_path_search rp_path_search_t;

/**
* structure for discovering items of path list
* received in callbacks
*/
struct rp_path_search_entry
{
  /** path to the entry */
  const char *path;

  /** filename (last part of path) */
  const char *name;

  /** length of the path */
  unsigned short pathlen;

  /** length of the name, can be zero at root */
  unsigned short namelen;

  /** is entry a directory */
  unsigned short isDir;

  /** action */
  enum {
	/** this is a file */
	rp_path_search_file,
	/** this is a directory, called before entering in it */
	rp_path_search_directory_before,
	/** this is a directory, called to check if entering is allowed (filter only) */
	rp_path_search_directory_enter,
	/** this is a directory, called after entering in it */
	rp_path_search_directory_after
	} action;
};
typedef struct rp_path_search_entry rp_path_search_entry_t;

/**
* callback receiving items of a path list
*
* @param closure  closure given at call
* @param path     the path item zero terminated
* @param length   length of the path (excluding trailing zero)
*
* @return should return 0 to continue or a non zero value to stop listing
*/
typedef int (*rp_path_search_cb)(void *closure, const char *path, size_t length);

/**
* callback receiving entries of direstories of a path list
*
* @param closure  closure given at call
* @param path     the entry
*/
typedef int (*rp_path_search_item_cb)(void *closure, const rp_path_search_entry_t *entry);

/**
* callback receiving entries of direstories of a path list
*
* @param closure  closure given at call
* @param path     the entry
* @param action
*/
typedef int (*rp_path_search_filter_cb)(void *closure, const rp_path_search_entry_t *entry);

/**
* Add a reference to the path's list
*
* @param paths the path's list to reference
*
* @return the given path's list
*/
extern rp_path_search_t *rp_path_search_addref(rp_path_search_t *paths);

/**
* Remove a refernce to the path's list.
* Can destroy the path's list data when it is not referenced any more.
*
* @param paths the path's list to reference
*/
extern void rp_path_search_unref(rp_path_search_t *paths);

/**
* Creates a path's list with paths of the given string
*
* The given string is a colon separated list of paths.
*
* The string can contain reference to environement variables
* (in one of the form ${...} or $(...) or $ALPHA_NUM)
* that will be expanded.
*
* Synonym of rp_path_search_add_dirs(paths, dirs, 1, NULL)
*
* @param paths address receiving the created path's list
* @param dirs  string indicating the directories colon separated
*
* @return 0 in case of success and in that case, paths receives the result
*         or a negative number in case of error
*/
extern int rp_path_search_make_dirs(rp_path_search_t **paths, const char *dirs);

/**
* Creates a path's list with paths of the given environment variable.
*
* Synonym of rp_path_search_make_dirs(paths, getenv(var))
*
* @param paths address receiving the created path's list
* @param var    name of the environment variable
*
* @return 0 in case of success and in that case, paths receives the result
*         or a negative number in case of error
*/
extern int rp_path_search_make_env(rp_path_search_t **paths, const char *var);

/**
* Add a path's with paths of the given string 'dirs'.
*
* The given string is a colon separated list of paths.
*
* The string can contain reference to environement variables
* (in one of the form ${...} or $(...) or $ALPHA_NUM)
* that will be expanded.
*
* @param paths  address receiving the modified path's list
* @param dirs   string indicating the directories colon separated
* @param before flag indicating if the paths are prepended (not zero) or appended (zero)
* @param other  the base path list to extend
*
* @return 0 in case of success and in that case, paths receives the result
*         or a negative number in case of error
*/
extern int rp_path_search_add_dirs(rp_path_search_t **paths, const char *dirs, int before, rp_path_search_t *other);

/**
* Add a path's with paths of the given environment variable.
*
* Synonym of rp_path_search_make_dirs(paths, getenv(var), 1, NULL)
*
* @param paths  address receiving the modified path's list
* @param var    name of the environment variable
* @param before flag indicating if the paths are prepended (not zero) or appended (zero)
* @param other  the base path list to extend
*
* @return 0 in case of success and in that case, paths receives the result
*         or a negative number in case of error
*/
extern int rp_path_search_add_env(rp_path_search_t **paths, const char *var, int before, rp_path_search_t *other);

/**
* Extend a path's with paths of the given string 'dirs'.
*
* Can be seen like a call to rp_path_search_add_dirs(paths, dirs, 1, *paths)
* but take care of the reference count
*
* @param paths  address of the path list to modify
* @param dirs   string indicating the directories colon separated
* @param before flag indicating if the paths are prepended (not zero) or appended (zero)
*
* @return 0 in case of success and in that case, paths receives the result
*         or a negative number in case of error
*/
extern int rp_path_search_extend_dirs(rp_path_search_t **paths, const char *dirs, int before);

/**
* Extend a path's with paths of the given environment variable.
*
* Synonym of rp_path_search_extend_dirs(paths, getenv(var), 1)
*
* @param paths  address of the path list to modify
* @param var    name of the environment variable
* @param before flag indicating if the paths are prepended (not zero) or appended (zero)
*
* @return 0 in case of success and in that case, paths receives the result
*         or a negative number in case of error
*/
extern int rp_path_search_extend_env(rp_path_search_t **paths, const char *var, int before);

/**
* List the paths of the path's list in the search order.
* Calls the given callback for each path of the list until it
* return a non zero value.
*
* @param paths the search path
* @param callback the callback to call
* @param closure  the closure for the callback
*
* @return zero or the last non zero value returned
*/
extern int rp_path_search_list(rp_path_search_t *paths, rp_path_search_cb callback, void *closure);

/**
* Locates files of filename in the path's list and call the callback
* for them
*
* @param paths the patsh's list
* @param filename the filename to locate
* @param callback the callback to call
* @param closure  the closure for the callback
*
* @return zero or the last non zero value returned
*/
extern int rp_path_search_find(rp_path_search_t *paths, const char *filename, rp_path_search_cb callback, void *closure);

/**
* Check if the module can enumerate directory's entries
* If this function returns zero, the functions
*  - rp_path_search_filter
*  - rp_path_search
*  - rp_path_search_get_path
*  - rp_path_search_get_content
* Have no possible implementation and should not be called.
*/
extern int rp_path_search_can_list_entries();

/**
* Callbacks wants regular files
*/
#define RP_PATH_SEARCH_FILE        1
/**
* Callbacks wants directories
*/
#define RP_PATH_SEARCH_DIRECTORY   2
/**
* Tell to also check items of path's list as file
*/
#define RP_PATH_SEARCH_FLEXIBLE    4
/**
* Makes the search recursive (loops detected)
*/
#define RP_PATH_SEARCH_RECURSIVE   8
/**
* Makes the search recursive (loops detected)
*/
#define RP_PATH_SEARCH_DEPTH_BASE  16
/**
* Set maximum depth if recursive (zero means not recursive)
*/
#define RP_PATH_SEARCH_DEPTH(x)    (RP_PATH_SEARCH_DEPTH_BASE * (1 + (x)))

/**
* CAUTION only effective if rp_path_search_can_list_entries() returns non zero value.
*
* Enumerate all entries in path's list, listing directory content,
* accordingly to flags.
*
* For each entry matching the criteria of flags, call the given
* filter (if any), if the filter return a non zero value, the callback is called.
*
* Enumeration stops when a callback returns a non zero value.
*
* @param paths     the path's list
* @param flags     flags of search (see above)
* @param callback  callback to call for each filtered in entry
* @param callback_closure   the closure for the callback
* @param filter    if not NULL a callback filtering calls to callback
* @param filter_closure   the closure for the filter
*
* @return the last non zero value returned by a callback or zero
*/
extern
int rp_path_search_filter(
	rp_path_search_t *paths,
	int flags,
	rp_path_search_item_cb callback,
	void *callback_closure,
	rp_path_search_filter_cb filter,
	void *filter_closure
);

/**
*/
extern int rp_path_search(rp_path_search_t *paths, int flags, rp_path_search_item_cb callback, void *closure);

/**
* Enumerate entries matching the name and/or the extension
*
*
*/
extern int rp_path_search_match(rp_path_search_t *paths, int flags, const char *name, const char *extension, rp_path_search_item_cb callback, void *closure);

#ifdef	__cplusplus
}
#endif
