#include "string_convert.h"



int cv_asci_to_utf8(const unsigned char * in, unsigned char ** out, unsigned int in_size, int in_settings)
{
	*out = NULL;
	unsigned int i;
	unsigned ret_count = in_size;
	unsigned char * ret = malloc(sizeof(char) * (ret_count)); 
	if(NULL == ret)
		return -3;

	for(i = 0; i < in_size; i+=1)
	{
		if((in[i] & 0x80) != 0)
		{
			/* this isn't defined in asci */
			if((in_settings & DON_T_ABORT_ON_NON_CONVERTABLE_CHAR) != 0)
				ret[i] = '?';
			else
			{
				free(ret);
				return -1;
			}
		}
		ret[i] = in[i];
	}

	*out = ret;
	return ret_count;
}

int cv_utf8_to_utf8(const unsigned char * in, unsigned char ** out, unsigned int in_size)
{
	*out = NULL;
	unsigned ret_count = in_size;
	unsigned char * ret = malloc(sizeof(char) * (ret_count)); 
	if(NULL == ret)
		return -3;

		memmove(ret, in, ret_count);

	*out = ret;
	return ret_count;
}

int cv_utf16_to_utf8(const unsigned char * in, unsigned char ** out, unsigned int in_size, int in_settings)
{
	*out = NULL;
	
	/* check size (must be divisible by 2) */
	if((in_size & 1) != 0)
		return -4; /* not aligned */
	
	unsigned int i;
	
	/* alloc as much space as we think we might need */
	unsigned ret_count = 0;
	/* buffer for the converted string, 4 is used as a multiplier as _right_now_ a 4-byte wchar can be
	 * converted to at most 4 chars
	 * WARING: when there are changes to the UTF-32 standard this might need to be changed to! */
	unsigned char * ret = (unsigned char *)malloc(sizeof(char) * (in_size + 1) * 3); 
	if(NULL == ret)
		return -3;
	


	/* select mapping of bytes */
		int m0, m1;
		if((in_settings & IS_LITTLE_ENDIAN) != 0)
		{ m0=0;m1=1; }
		else if((in_settings & IS_BIG_ENDIAN) != 0)
		{ m0=1;m1=0; }
		else
		{
			#if defined SYSTEM_IS_LITTLE_ENDIAN
				m0=0;m1=1;
			#elif defined SYSTEM_IS_BIG_ENDIAN
				m0=1;m1=0;
			#endif
		}
	/* do the conversion */
	for(i = 0; i < in_size; i+=2)
	{
		if((in[i+m1] & 0xF8) != 0)
		{	/* we need 3 chars */
			ret[ret_count]   = (0xE0 | in[i+m1] >> 4);
			ret[ret_count+1] = (0x80 | ((0x0F & in[i+m1]) << 2) | (in[i+m0]) >> 6);
			ret[ret_count+2] = (0x80 | (0x3F & in[i+m0]));
			ret_count += 3;
		}
		else if(((in[i+m1] & 0x07) != 0) || ((in[i+m0] & 0x80) != 0))
		{	/* we need 2 chars */
			ret[ret_count]   = (0xC0 | ((0x07 & in[i+m1]) << 2) | ((in[i+m0]) >> 6));
			ret[ret_count+1] = (0x80 | (0x3F & in[i+m0]));
			ret_count += 2;
		}
		else
		{   /* we need 1 char */
			ret[ret_count] = in[i+m0];
			ret_count++;
		}
	}

	/* ensure the last sign is a nil character */
	if(ret_count > 0)
		ret[ret_count - 1] = '\0';
	else
	{
		free(ret);
		return -2; /* ret_count must be bigger than 0, if not something strange happened */
	}

	ret = (unsigned char*)realloc(ret, (sizeof(char) * ret_count));
		
	*out = (unsigned char *) ret; /* make the buffer accessible to caller */
	return ret_count;
}

int cv_utf32_to_utf8(const unsigned char * in, unsigned char ** out, unsigned int in_size, int in_settings)
{
	*out = NULL;
	
	/* check size (must be aligned) */
	if((in_size & 3) != 0)
		return -4; /* not aligned */
	
	unsigned int i;
	
	/* alloc as much space as we think we might need */
	unsigned ret_count = 0;
	/* buffer for the converted string, 4 is used as a multiplier as _right_now_ a 4-byte wchar can be
	 * converted to at most 4 chars
	 * WARING: when there are changes to the UTF-32 standard this might need to be changed to! */
	unsigned char * ret = (unsigned char *)malloc(sizeof(char) * (in_size + 1) * 4); 
	if(NULL == ret)
		return -3;
	


	/* select mapping of bytes */
		int m0, m1, m2, m3;
		if((in_settings & IS_LITTLE_ENDIAN) != 0)
		{ m0=0;m1=1;m2=2;m3=3; }
		else if((in_settings & IS_BIG_ENDIAN) != 0)
		{ m0=3;m1=2;m2=1;m3=0; }
		else
		{
			#if defined SYSTEM_IS_LITTLE_ENDIAN
				m0=0;m1=1;m2=2;m3=3;
			#elif defined SYSTEM_IS_BIG_ENDIAN
				m0=3;m1=2;m2=1;m3=0;
			#endif
		}
	/* do the conversion */
	for(i = 0; i < in_size; i+=4)
	{
		
		if(in[i+m3] != 0 || (in[i+m2] & 0xE0) != 0)
		{
			/* this isn't defined until now */
			free(ret);
			return -1;
		}
		
		if((in[i+m2] & 0x1F) != 0)
		{	/* we need 4 chars */
			ret[ret_count]   = (0xF0 | ((0x1F & in[i+m2]) >> 2));
			ret[ret_count+1] = (0x80 | (0x03 & in[i+m2]) << 4) | (in[i+m1] >> 4);
			ret[ret_count+2] = (0x80 | ((0x0F & in[i+m1]) << 2) | (in[i+m0] >> 6));
			ret[ret_count+3] = (0x80 | (0x3F & in[i+m0]));
			ret_count += 4;
		}
		else if((in[i+m1] & 0xF8) != 0)
		{	/* we need 3 chars */
			ret[ret_count]   = (0xE0 | in[i+m1] >> 4);
			ret[ret_count+1] = (0x80 | ((0x0F & in[i+m1]) << 2) | (in[i+m0]) >> 6);
			ret[ret_count+2] = (0x80 | (0x3F & in[i+m0]));
			ret_count += 3;
		}
		else if(((in[i+m1] & 0x07) != 0) || ((in[i+m0] & 0x80) != 0))
		{	/* we need 2 chars */
			ret[ret_count]   = (0xC0 | ((0x07 & in[i+m1]) << 2) | ((in[i+m0]) >> 6));
			ret[ret_count+1] = (0x80 | (0x3F & in[i+m0]));
			ret_count += 2;
		}
		else
		{   /* we need 1 char */
			ret[ret_count] = in[i+m0];
			ret_count++;
		}
	}

	/* ensure the last sign is a nil character */
	if(ret_count > 0)
		ret[ret_count - 1] = '\0';
	else
	{
		free(ret);
		return -2; /* ret_count must be bigger than 0, if not something strange happened */
	}

	ret = (unsigned char*)realloc(ret, (sizeof(char) * ret_count));
		
	*out = (unsigned char *) ret; /* make the buffer accessible to caller */
	return ret_count;
}

int cv_utf8_to_utf32(const unsigned char * in, unsigned char ** out, unsigned int in_size, int out_settings)
{
	*out = NULL;
	unsigned int i;

	/* alloc as much space as we think we might need */
	unsigned ret_count = 0;
	/* buffer for the converted string, 4 is used as a multiplier as this is the worst case 
	 * (all utf8 characters are 1 byte long) */
	unsigned char * ret = (unsigned char *)malloc(sizeof(char) * (in_size + 1) * 4); 
	if(NULL == ret)
		return -3;

	/* select mapping of bytes */
		int m0, m1, m2, m3;
		if((out_settings & IS_LITTLE_ENDIAN) != 0)
		{ m0=0;m1=1;m2=2;m3=3; }
		else if((out_settings & IS_BIG_ENDIAN) != 0)
		{ m0=3;m1=2;m2=1;m3=0; }
		else
		{
			#if defined SYSTEM_IS_LITTLE_ENDIAN
				m0=0;m1=1;m2=2;m3=3;
			#elif defined SYSTEM_IS_BIG_ENDIAN
				m0=3;m1=2;m2=1;m3=0;
			#endif
		}
	for(i = 0; i < in_size;)
	{
		ret[ret_count+m0] = 0;
		ret[ret_count+m1] =	0;
		ret[ret_count+m2] =	0;
		ret[ret_count+m3] =	0;

		if((in[i] & 0x80) == 0)
		{ /* great it is a one byte long character */
			ret[ret_count+m3] =	in[i];
			i++;
		}
		else if((in[i] & 0xE0) == 0xC0) /* check for beginning 110... */
		{ /* it is a two byte long character */	
			/* check if we can access in[i+1] */
				if((i+1) >= in_size)
				{
					free(ret);
					return -5; /* input is to short */
				}
			/* check if the following bytes are malformed */
				if((in[i+1] & 0xC0) != 0x80)
				{
					free(ret);
					return -6; /* follow character(s) are malformed */
				}
			ret[ret_count+m2] =	((in[i] & 0x1C) >> 2);
			ret[ret_count+m3] =	(((in[i] & 0x03) << 6) | (in[i+1] & 0x3F));
			i+=2;
		}
		else if((in[i] & 0xF0) == 0xE0) /* check for beginning 1110... */
		{ /* it is a three byte long character */
			/* check if we can access in[i+2] */
				if((i+2) >= in_size)
				{
					free(ret);
					return -5;
				}
			/* check if the following bytes are malformed */
				if(((in[i+1] & 0xC0) != 0x80) || ((in[i+2] & 0xC0) != 0x80))
				{
					free(ret);
					return -6;
				}
			ret[ret_count+m2] = (((in[i] & 0x0F) << 4) | ((in[i+1] & 0x3C) >> 2));
			ret[ret_count+m3] = (((in[i+1] & 0x03) << 6) | (in[i+2] & 0x3F));
			i+=3;
		}
		else if((in[i] & 0xF8) == 0xF0) /* check for beginning 11110... */
		{ /* it is a four byte long character */
			/* check if we can access in[i+3] */
				if((i+3) >= in_size)
				{
					free(ret);
					return -5;
				}
			/* check if the following bytes are malformed */
				if(((in[i+1] & 0xC0) != 0x80) || ((in[i+2] & 0xC0) != 0x80) || ((in[i+3] & 0xC0) != 0x80))
				{
					free(ret);
					return -6;
				}
			ret[ret_count+m1] =	(((in[i] & 0x07) << 2 ) | ((in[i+1] & 0x30) >> 4));
			ret[ret_count+m2] =	(((in[i+1] & 0x0F) << 4) | ((in[i+2] & 0x3C) >> 2));
			ret[ret_count+m3] =	(((in[i+2] & 0x03) << 6) | (in[i+3] & 0x3F));
			i+=4;
		}
		else
		{
			free(ret);
			return -7; /* malformed character */
		}
		ret_count = ret_count + 4; 
	}
	
	if(ret_count > 3)
	{
		ret[ret_count - 1] = '\0';
		ret[ret_count - 2] = '\0';
		ret[ret_count - 3] = '\0';
		ret[ret_count - 4] = '\0';
	}
	else
	{
		free(ret);
		return -2; /* ret_count must be bigger than 0, if not something strange happened */
	}

	ret = (unsigned char*)realloc(ret, (sizeof(char) * ret_count));
		
	*out = (unsigned char *) ret; /* make the buffer accessible to caller */
	return ret_count;
}

int cv_utf8_to_utf16(const unsigned char * in, unsigned char ** out, unsigned int in_size, int out_settings)
{
	*out = NULL;
	unsigned int i;

	/* alloc as much space as we think we might need */
	unsigned ret_count = 0;
	/* buffer for the converted string, 4 is used as a multiplier as this is the worst case 
	 * (all utf8 characters are 1 byte long) */
	unsigned char * ret = (unsigned char *)malloc(sizeof(char) * (in_size + 1) * 2); 
	if(NULL == ret)
		return -3;

	/* select mapping of bytes */
		int m0, m1;
		if((out_settings & IS_LITTLE_ENDIAN) != 0)
		{ m0=0;m1=1; }
		else if((out_settings & IS_BIG_ENDIAN) != 0)
		{ m0=1;m1=0; }
		else
		{
			#if defined SYSTEM_IS_LITTLE_ENDIAN
				m0=0;m1=1;
			#elif defined SYSTEM_IS_BIG_ENDIAN
				m0=1;m1=0;
			#endif
		}
	for(i = 0; i < in_size;)
	{
		ret[ret_count+m0] = 0;
		ret[ret_count+m1] =	0;

		if((in[i] & 0x80) == 0)
		{ /* great it is a one byte long character */
			ret[ret_count+m1] =	in[i];
			i++;
		}
		else if((in[i] & 0xE0) == 0xC0) /* check for beginning 110... */
		{ /* it is a two byte long character */	
			/* check if we can access in[i+1] */
				if((i+1) >= in_size)
				{
					free(ret);
					return -5; /* input is to short */
				}
			/* check if the following bytes are malformed */
				if((in[i+1] & 0xC0) != 0x80)
				{
					free(ret);
					return -6; /* follow character(s) are malformed */
				}
			ret[ret_count+m0] =	((in[i] & 0x1C) >> 2);
			ret[ret_count+m1] =	(((in[i] & 0x03) << 6) | (in[i+1] & 0x3F));
			i+=2;
		}
		else if((in[i] & 0xF0) == 0xE0) /* check for beginning 1110... */
		{ /* it is a three byte long character */
			/* check if we can access in[i+2] */
				if((i+2) >= in_size)
				{
					free(ret);
					return -5;
				}
			/* check if the following bytes are malformed */
				if(((in[i+1] & 0xC0) != 0x80) || ((in[i+2] & 0xC0) != 0x80))
				{
					free(ret);
					return -6;
				}
			ret[ret_count+m0] = (((in[i] & 0x0F) << 4) | ((in[i+1] & 0x3C) >> 2));
			ret[ret_count+m1] = (((in[i+1] & 0x03) << 6) | (in[i+2] & 0x3F));
			i+=3;
		}
		else if((in[i] & 0xF8) == 0xF0) /* check for beginning 11110... */
		{ /* it is a four byte long character */
			/* check if we can access in[i+3] */
				if((i+3) >= in_size)
				{
					free(ret);
					return -5;
				}
			/* check if the following bytes are malformed */
				if(((in[i+1] & 0xC0) != 0x80) || ((in[i+2] & 0xC0) != 0x80) || ((in[i+3] & 0xC0) != 0x80))
				{
					free(ret);
					return -6;
				}

			/* we can't convert this damned */
			if((out_settings & DON_T_ABORT_ON_NON_CONVERTABLE_CHAR) != 0)
				ret[ret_count+m1] =	'?'; /* we ignore this an print a question mark */
			else
			{
				free(ret);
				return -8; /* in contained a character we can't convert due to the limitations of utf16 */
			}
			i+=4;
		}
		else
		{
			free(ret);
			return -7; /* malformed character */
		}
		ret_count = ret_count + 2; 
	}
	
	if(ret_count > 2)
	{
		ret[ret_count - 1] = '\0';
		ret[ret_count - 2] = '\0';
	}
	else
	{
		free(ret);
		return -2; /* ret_count must be bigger than 0, if not something strange happened */
	}

	ret = (unsigned char*)realloc(ret, (sizeof(char) * ret_count));
		
	*out = (unsigned char *) ret; /* make the buffer accessible to caller */
	return ret_count;
}

int cv_utf8_to_asci(const unsigned char * in, unsigned char ** out, unsigned int in_size, int out_settings)
{
	*out = NULL;
	unsigned int i;

	/* alloc as much space as we think we might need */
	unsigned ret_count = 0;
	/* buffer for the converted string, 4 is used as a multiplier as this is the worst case 
	 * (all utf8 characters are 1 byte long) */
	unsigned char * ret = (unsigned char *)malloc(sizeof(char) * (in_size + 1)); 
	if(NULL == ret)
		return -3;

	for(i = 0; i < in_size;)
	{
		ret[ret_count] = 0;

		if((in[i] & 0x80) == 0)
		{ /* great it is a one byte long character */
			ret[ret_count] =	in[i];
			i++;
		}
		else if((in[i] & 0xE0) == 0xC0) /* check for beginning 110... */
		{ /* it is a two byte long character */	
			/* check if we can access in[i+1] */
				if((i+1) >= in_size)
				{
					free(ret);
					return -5; /* input is to short */
				}
			/* check if the following bytes are malformed */
				if((in[i+1] & 0xC0) != 0x80)
				{
					free(ret);
					return -6; /* follow character(s) are malformed */
				}
			
			/* we can't convert this damned */
			if((out_settings & DON_T_ABORT_ON_NON_CONVERTABLE_CHAR) != 0)
				ret[ret_count] =	'?'; /* we ignore this an print a question mark */
			else
			{
				free(ret);
				return -8; /* in contained a character we can't convert due to the limitations of utf16 */
			}
			i+=2;
		}
		else if((in[i] & 0xF0) == 0xE0) /* check for beginning 1110... */
		{ /* it is a three byte long character */
			/* check if we can access in[i+2] */
				if((i+2) >= in_size)
				{
					free(ret);
					return -5;
				}
			/* check if the following bytes are malformed */
				if(((in[i+1] & 0xC0) != 0x80) || ((in[i+2] & 0xC0) != 0x80))
				{
					free(ret);
					return -6;
				}
			
			/* we can't convert this damned */
			if((out_settings & DON_T_ABORT_ON_NON_CONVERTABLE_CHAR) != 0)
				ret[ret_count] =	'?'; /* we ignore this an print a question mark */
			else
			{
				free(ret);
				return -8; /* in contained a character we can't convert due to the limitations of utf16 */
			}
			i+=3;
		}
		else if((in[i] & 0xF8) == 0xF0) /* check for beginning 11110... */
		{ /* it is a four byte long character */
			/* check if we can access in[i+3] */
				if((i+3) >= in_size)
				{
					free(ret);
					return -5;
				}
			/* check if the following bytes are malformed */
				if(((in[i+1] & 0xC0) != 0x80) || ((in[i+2] & 0xC0) != 0x80) || ((in[i+3] & 0xC0) != 0x80))
				{
					free(ret);
					return -6;
				}

			/* we can't convert this damned */
			if((out_settings & DON_T_ABORT_ON_NON_CONVERTABLE_CHAR) != 0)
				ret[ret_count] =	'?'; /* we ignore this an print a question mark */
			else
			{
				free(ret);
				return -8; /* in contained a character we can't convert due to the limitations of utf16 */
			}
			i+=4;
		}
		else
		{
			free(ret);
			return -7; /* malformed character */
		}
		ret_count = ret_count + 1; 
	}
	
	if(ret_count > 1)
	{
		ret[ret_count - 1] = '\0';
	}
	else
	{
		free(ret);
		return -2; /* ret_count must be bigger than 0, if not something strange happened */
	}

	ret = (unsigned char*)realloc(ret, (sizeof(char) * ret_count));
		
	*out = (unsigned char *) ret; /* make the buffer accessible to caller */
	return ret_count;
}


#ifdef HAVE_WCHAR_T /* we only want to do this if we have a wchar_t */
int cv_wchar_to_utf8(const wchar_t * wchar, char ** utf, int utf_settings)
{
	unsigned int len = wcslen(wchar);
	#if (SIZE_OF_WCHAR_T == 4) /* Assuming UTF-32 */
		return cv_utf32_to_utf8((unsigned char *) wchar, (unsigned char **) utf, ((len + 1) * 4), 0);
	#elif (SIZE_OF_WCHAR_T == 2) /* Assuming UTF-16 */
		return cv_utf16_to_utf8((unsigned char *) wchar, (unsigned char **) utf, ((len+1) * 2), 0);
	#elif (SIZE_OF_WCHAR_T == 1) /* Assuming UTF-8 */
		return cv_utf8_to_utf8((unsigned char *) wchar, (unsigned char **) utf, (len+1));
	#else
		#error This size of WCHAR_T is currently not supported, sorry
	#endif
}

int cv_utf8_to_wchar(const char * utf, wchar_t ** wchar, int wchar_settings) 
{
	unsigned int len = strlen(utf);
	#if (SIZE_OF_WCHAR_T == 4) /* Assuming UTF-32 */
		return cv_utf8_to_utf32((unsigned char *) utf, (unsigned char **) wchar, (len+1), 0);
	#elif (SIZE_OF_WCHAR_T == 2) /* Assuming UTF-16 */
		return cv_utf8_to_utf16((unsigned char *) utf, (unsigned char **) wchar, (len+1), wchar_settings);
	#elif (SIZE_OF_WCHAR_T == 1) /* Assuming UTF-8 */
		return cv_utf8_to_utf8((unsigned char *) wchar, (unsigned char **) utf, (len+1));
	#else
		#error This size of WCHAR_T is currently not supported, sorry
	#endif
}
#endif
