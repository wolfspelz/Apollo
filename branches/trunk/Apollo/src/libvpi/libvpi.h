// ============================================================================
//
// libvpi
//
// ============================================================================

// Configuration:
// --------------

#define VPI_LIB_MAJORVERSION "0"
#define VPI_LIB_MINORVERSION "6"
#define VPI_LIB_NAME "libvpi"
#define VPI_LIB_DESCRIPTION VPI_LIB_NAME " " VPI_LIB_MAJORVERSION "." VPI_LIB_MINORVERSION 

// Definitions:
// ------------

#include <sys/types.h>

#define VPI_IN
#define VPI_OUT
#define VPI_INOUT

#if defined(__GNUC__)
  #define VPI_UNUSED_FUNCTION __attribute__((unused))
#else
  #define VPI_UNUSED_FUNCTION
#endif

#if defined(__GNUC__)
  #define VPI_UNUSED_VARIABLE __attribute__((unused))
#else
  #define VPI_UNUSED_VARIABLE
#endif

#if defined(__GNUC__)
  #define VPI_UNUSED_ARG(a) do {} while (&a == 0)
#else
  #define VPI_UNUSED_ARG(a) (a)
#endif

#if defined(__cplusplus)
extern "C" {
#endif

typedef void* vpi_context;
typedef long vpi_callback_ref;

// ctor/dtor:
// ----------

// Create a new libvpi context
vpi_context vpi_new();

// Destroy the libvpi context
// context: libvpi context
int vpi_delete(vpi_context context);

// Config:
// -------

// Set the function that will be called when the library needs additional data
// ref: forwarded from vpi_set_requestfile_callback
// url: VPI file URL to be fetched
typedef int (*vpi_requestfile_callback) (vpi_callback_ref ref, const char* url);
// context: libvpi context
// callback: callback function
// ref: value transparently forwarded to the callback function
int vpi_set_requestfile_callback(vpi_context context, vpi_requestfile_callback callback, vpi_callback_ref ref);

// Set the function that will be called when the library asks to assemble the location url
// ref: forwarded from vpi_set_requestfile_callback
// protocol: protocol identifier
// name: room name
// service: server/service name
// url: buffer for assembled URL. The URL will be 0 terminated.
// url_len: buffer length
typedef int (*vpi_composelocation_callback) (vpi_callback_ref ref, const char* protocol, const char* name, const char* service, char* url, unsigned int* url_len);
// context: libvpi context
// callback: callback function
// ref: value transparently forwarded to the callback function
int vpi_set_composelocation_callback(vpi_context context, vpi_composelocation_callback callback, vpi_callback_ref ref);

// Operation:
// ----------

// Delete all cached files
// context: libvpi context
int vpi_cache_clear(vpi_context context);

// Check for expired files and remove them
// context: libvpi context
int vpi_cache_expire(vpi_context context);

// Add an xml file
// context: libvpi context
// url: VPI file URL
// xml: VPI file data
int vpi_cache_add_xml(vpi_context context, VPI_IN const char* url, VPI_IN const char* xml);

// Add an empty file, if the data is no xml, invalid xml or empty
// context: libvpi context
// url: VPI file URL
int vpi_cache_add_nocontent(vpi_context context, VPI_IN const char* url);

// Search and return matching location data
// context: libvpi context
// document_url: web document URL
// location_xml: buffer to return xml data. The data will be 0 terminated. The buffer must be released with vpi_free()
int vpi_get_location_xml(vpi_context context, VPI_IN const char* document_url, VPI_OUT char** location_xml);
// Good result: VPI_SUCCESS
// Demi-good (ask me again) result: VPI_ERROR_REQUESTED_DATA or VPI_ERROR_NEED_DATA
// All others are errors

// Extract data from location tag
// context: libvpi context
// location_xml: location tag from vpi_get_location_xml()
// path: node path, e.g. "name" to get the name-child node of the location node
// flags: flags control how the xml is returned, default: 0 = return outer XML
// detail_xml: buffer to return xml data. The data will be 0 terminated. The buffer must be released with vpi_free()
int vpi_get_detail_xml(vpi_context context, VPI_IN const char* location_xml, VPI_IN const char* path, VPI_IN int flags, VPI_OUT char** detail_xml);
#define VPI_GDX_INNERXML 1 // Return inner XML omitting the outer tag. Used to extract node text

// Extract location URL (room URL) from location tag
// context: libvpi context
// location_xml: location tag from vpi_get_location_xml()
// document_url: web document URL
// location_url: buffer to return url. The url will be 0 terminated. The buffer must be released with vpi_free()
int vpi_get_location_url(vpi_context context, VPI_IN const char* location_xml, VPI_OUT char** location_url);

// Extract suffix select options for the document url
// location_xml: location tag from vpi_get_location_xml()
// options_list: return value: list of available suffixes, release the list with vpi_free_list()
int vpi_get_select_options(vpi_context context, VPI_IN const char* location_xml, VPI_OUT char*** options_list);

// Extract parameters for a suffix option and a document URL
// location_xml: location tag from vpi_get_location_xml()
// suffix: suffix
// properties_list: return value: list of key/value pairs of properties for the option, 2n strings, release the list with vpi_free_list()
int vpi_get_select_option_properties(vpi_context context, VPI_IN const char* location_xml, VPI_IN const char* suffix, VPI_OUT char*** properties_list);

// Set the room name suffix to be used for all generated locations
// suffix: well, the suffix
int vpi_set_suffix(vpi_context context, VPI_IN const char* suffix);

// Set the room name suffix to be used for all generated locations
// suffix: well, the suffix
int vpi_get_suffix(vpi_context context, VPI_OUT char** suffix);

// Utility:
// --------

// Memory allocation using the libvpi memory manager
void* vpi_malloc(size_t len);
void vpi_free(void* p);

// Release a list of strings
int vpi_free_list(char** list);

// Run tests
int vpi_run_tests(vpi_context context);

// Extract all status info values
// status_list: return value: key/value list of internal variables, status counters, etc., n strings, release the list with vpi_free_list()
int vpi_get_status(vpi_context context, VPI_OUT char*** status_list);

// Get the list of VPI files used during the last vpi_get_location_xml()
// files_list: return value: list of VPI file URLs, release the list with vpi_free_list()
int vpi_get_traversed_files(vpi_context context, VPI_OUT char*** files_list);

// Set unix time to be used to delete old cache files (vpi_cache_expire)
// now: time
int vpi_set_time(vpi_context context, time_t now);

// Logging/Errors:
// ---------------

#define VPI_LOG_NONE          1
#define VPI_LOG_FATAL         2
#define VPI_LOG_ERROR         4
#define VPI_LOG_WARNING       8
#define VPI_LOG_DEBUG         16
#define VPI_LOG_INFO          32
#define VPI_LOG_VERBOSE       64
#define VPI_LOG_TRACE         128
#define VPI_LOG_VERYVERBOSE   256

#define VPI_LOGCHANNEL "libvpi"
#define VPI_LOGCONTEXT __FUNCTION__

typedef void (*vpi_log_callback) (int level, const char* channel, const char* method, const char* message);
typedef const char* (*vpi_config_read_callback) (const char* path, const char* default_value, char* buf, size_t buf_len);
typedef void (*vpi_config_write_callback) (const char* path, const char* value);

int vpi_set_log_callback(vpi_log_callback callback);
int vpi_set_config_read_callback(vpi_config_read_callback callback);
int vpi_set_config_write_callback(vpi_config_write_callback callback);

// set log mask
// returns old mask
int vpi_set_log_mask(int mask);

// API return values
#define VPI_ERROR 0
#define VPI_SUCCESS 1

// if API returns VPI_ERROR then ask vpi_get_last_error
int vpi_get_last_error(vpi_context context);
int vpi_get_last_error_string(vpi_context context, char* err, unsigned int* len);

// vpi_get_last_error returns:
#define VPI_ERROR_UNKNOWN 0
#define VPI_ERROR_BUFFER_TOO_SMALL 2
#define VPI_ERROR_INVALID_PARAMETER 3
#define VPI_ERROR_TEST_FAILED 4
#define VPI_ERROR_NEED_DATA 5
#define VPI_ERROR_REQUESTED_DATA 6
#define VPI_ERROR_INVALID_DATA 7
#define VPI_ERROR_INTERNAL_ERROR 8
#define VPI_ERROR_NO_LOCATION_DATA 9
#define VPI_ERROR_ROOM_COMPONENT 10
#define VPI_ERROR_XMLPARSER 11
#define VPI_ERROR_INVALID_URL 12
#define VPI_ERROR_ASSEMBLELOCATION 13
#define VPI_ERROR_REQUEST_DATA_FAILED 14
#define VPI_ERROR_NO_SUCH_NODE 15

#if defined(__cplusplus)
};
#endif
/*
php sample

$sLocationUrl = '';
$vpi = vpi_new();
vpi_cache_expire($vpi);
while (!$bDone) {
  $sLocationXML = vpi_get_location_xml($vpi, $sDocumentUrl);
  if ($sLocationXML != '') {
    $sLocationUrl = vpi_get_location_url($vpi, $sLocationXML);
    $bDone = 1; // successfully
  } else {
    $sError = vpi_get_last_error($vpi);
    if ($sError == 'VPI_ERROR_NEED_DATA') {
      $sVpiUrl = vpi_get_last_error_string($vpi);
      $sVpiData = file_get_contents($sVpiUrl);
      if (IsValidXml($sVpiData)) {
        vpi_cache_add_nocontent($vpi, $sVpiUrl);
      } else {
        vpi_cache_add_xml($vpi, $sVpiUrl, $sVpiData);
      }
    } else {
      $bDone = 1; // with error
    }
  }
} // while

*/
