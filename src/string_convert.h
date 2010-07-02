#ifndef string_convert_h
#define string_convert_h

/* WARNING: This functions are highly experimental, they are inearly untested! Use at your own risk. */


#include "config.h"
#include <wchar.h>
#include <stdlib.h>
#include <string.h>

/* Settings for the convert functions */
#define IS_LITTLE_ENDIAN 1
#define IS_BIG_ENDIAN 2
#define DON_T_ABORT_ON_NON_CONVERTABLE_CHAR 4

/* Error values of the convert functions */
#define TODO_E1 -1
#define TODO_E2 -2
#define TODO_E3 -3
#define TODO_E4 -4
#define TODO_E5 -5
#define TODO_E6 -6
#define TODO_E7 -7


/** @brief Convert asci to utf8
 *
 * If an extended asci character is used, an error value will be returned and out is NULL.
 * Hint: Error values are described in the "Convert Errors Section"
 *
 * We strongly advice to use utf8 encoding for everything.
 * @param in			Char array in extended asci encoding
 * @param out			Returns: Char array in utf8 encoding or NULL
 * @param in_size		Size of in in byte (including the '\0' character)
 * @param in_settings	Should we return an error if a char isn't convertable (due to limitations of the encoding)? (You can use DON_T_ABORT_ON_NON_CONVERTABLE_CHAR to controle this)
 *
 * @return Size of out, or a negative error value
 */
int cv_asci_to_utf8(const unsigned char * in, unsigned char ** out, unsigned int in_size, int in_settings);

/** @brief Convert utf8 to utf8
 *
 * This function only copies buffer in in a new buffer out.
 * Hint: Error values are described in the "Convert Errors Section"
 *
 * @param in		Char array in extended asci encoding
 * @param out		Returns: Char array in utf8 encoding or NULL
 * @param in_size	Size of in in byte (including the '\0' character)
 *
 * @return Size of out, or a negative error value
 */
int cv_utf8_to_utf8(const unsigned char * in, unsigned char ** out, unsigned int in_size);

/** @brief Convert utf16 to utf8
 *
 * This function only allows characters be of size 2 byte
 * TODO check if errors in in are detected!
 * Hint: Error values are described in the "Convert Errors Section"
 *
 * @param in			Char array in utf16 encoding
 * @param out			Returns: Char array in utf8 encoding or NULL
 * @param in_size		Size of in in byte (including the '\0' character)
 * @param in_settings	Information about the in buffer. (Little or Big Endian). If set to 0, the system defaults will be used.
 *
 * @return Size of out, or a negative error value
 */
int cv_utf16_to_utf8(const unsigned char * in, unsigned char ** out, unsigned int in_size, int in_settings);

/** @brief Convert utf32 to utf8
 *
 * This first 11 Bits of the characters must be 0, or an error will be thrown. (Don't be confused by this, this shouldn't border you as this region should not be mapped right now)
 * Hint: Error values are described in the "Convert Errors Section"
 *
 * @param in			Char array in utf32 encoding
 * @param out			Returns: Char array in utf8 encoding or NULL
 * @param in_size		Size of in in byte (including the '\0' character)
 * @param in_settings	Information about the in buffer. (Little or Big Endian). If set to 0, the system defaults will be used.
 *
 * @return Size of out, or a negative error value
 */
int cv_utf32_to_utf8(const unsigned char * in, unsigned char ** out, unsigned int in_size, int in_settings);

/** @brief Convert utf8 to utf32
 *
 * Hint: Error values are described in the "Convert Errors Section"
 *
 * @param in			Char array in utf8 encoding
 * @param out			Returns: Char array in utf32 encoding or NULL
 * @param in_size		Size of in in byte (including the '\0' character)
 * @param out_settings	Information about the out buffer. (Little or Big Endian). If set to 0, the system defaults will be used.
 *
 * @return Size of out, or a negative error value
 */
int cv_utf8_to_utf32(const unsigned char * in, unsigned char ** out, unsigned int in_size, int out_settings);

/** @brief Convert utf8 to utf16
 *
 * Hint: Error values are described in the "Convert Errors Section"
 *
 * @param in			Char array in utf8 encoding
 * @param out			Returns: Char array in utf16 encoding or NULL
 * @param in_size		Size of in in byte (including the '\0' character)
 * @param out_settings	Information about the out buffer. (Little or Big Endian). If neither Little nor Big Endian is specified, the system defaults will be used. You can also specify we should return an error if a char isn't convertable (due to limitations of the encoding)? (You can use DON_T_ABORT_ON_NON_CONVERTABLE_CHAR to controle this)

 *
 * @return Size of out, or a negative error value
 */
int cv_utf8_to_utf16(const unsigned char * in, unsigned char ** out, unsigned int in_size, int out_settings);

/** @brief Convert utf8 to asci
 *
 * Hint: Error values are described in the "Convert Errors Section"
 *
 * @param in			Char array in utf8 encoding
 * @param out			Returns: Char array in asci encoding or NULL
 * @param in_size		Size of in in byte (including the '\0' character)
 * @param out_settings	Should we return an error if a char isn't convertable (due to limitations of the encoding)? (You can use DON_T_ABORT_ON_NON_CONVERTABLE_CHAR to controle this)

 *
 * @return Size of out, or a negative error value
 */
int cv_utf8_to_asci(const unsigned char * in, unsigned char ** out, unsigned int in_size, int out_settings);



#ifdef HAVE_WCHAR_T /* we only want to do this if we have a wchar_t */
/** @brief Convert wchar to utf8-char
 *
 * Hint: Error values are described in the "Convert Errors Section"
 *
 * @param wchar				Wchar array 
 * @param utf				Returns: Char array in utf8 encoding or NULL
 * @param utf_settings		Set to 0 otherwise you might get unexspected results.

 *
 * @return Size of out, or a negative error value
 */
int cv_wchar_to_utf8(const wchar_t * wchar, char ** utf, int utf_settings);

/** @brief Convert utf8-char to wchar
 *
 * Hint: Error values are described in the "Convert Errors Section"
 *
 * @param utf				Char array in utf8 encoding
 * @param wchar				Returns: Char array in utf8 encoding or NULL
 * @param wchar_settings	Set to 0 or to DON_T_ABORT_ON_NON_CONVERTABLE_CHAR, if you want unconvertable characters (due to the possible limitations of the type wchar), to be converted to '?' instead of returning an error.

 *
 * @return Size of out, or a negative error value
 */
int cv_utf8_to_wchar(const char * utf, wchar_t ** wchar, int wchar_settings);
#endif

#endif
