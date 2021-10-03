#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
/* WIN32_LEAN_AND_MEAN?define???WINDOWS.H???
???????????????????????
????????????????? */
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <Mmsystem.h>

#include "../systeminc/version.h"
#include "../systeminc/tool.h"

#ifdef _TIME_GET_TIME
LARGE_INTEGER tickCount;
LARGE_INTEGER CurrentTick;
#endif

unsigned int TimeGetTime(void)
{
#ifdef _TIME_GET_TIME
	static __int64 time;

	QueryPerformanceCounter(&CurrentTick);
	return (unsigned int)(time = CurrentTick.QuadPart/tickCount.QuadPart);
#else
	return timeGetTime();
#endif
}

static int copyStringUntilDelim( unsigned char *, char delim, int, unsigned char * );


// ???????????????????
int wordchk( char **pp )
{
	char *p = *pp;

	while( *p != 0 )
	{
		switch( *p )
		{
			case '\t':
				p++;
				break;

			case ' ':
				p++;
				break;

			default:
				*pp = p;
				return 1;
		}
	}

	return 0;
}


// ?????????????????????
int getword( char **pp, char *q )
{
	int i=0;
	char *p = *pp;

	wordchk( &p );

	for( i = 0; *p != ' ' && *p != '\t' && *p != '\0'; p++ , q++ , i++ )
	{
		*q = *p;
	}

	*q = '\0';
	*pp = p;

	return i;
}


#if 0
// ?????????
void freadline( char *ptbuf, FILE *fp )
{
	char buf;
	int i;

	for( i = 0; ; i++ )
	{
		if( feof( fp ) != 0 )
		{
			*ptbuf--;
			*ptbuf = '\0';
			break;
		}

		fread( &buf, 1, sizeof( unsigned char ), fp );
		if( buf == ' ' )
		{
			*ptbuf++ = buf;
		}else if (buf == '\t'){
			*ptbuf++ = buf;
		}else if (buf != 0x0d && buf != 0x0a){
			*ptbuf++ = buf;
		}else{
			*ptbuf++ = '\0';
			break;
		}
	}
	while ( feof(fp ) == 0 ){
		fread( &buf ,1 , sizeof(unsigned char) , fp );
		if (buf != 0x0d && buf != 0x0a ){
			fseek( fp, -1, SEEK_CUR);
			break;
		}
	}
}
#endif


// ??????????????????
int strstr2( char **pp , char *q )
{
	char *p = *pp;
	int i;

	wordchk( &p );

	for( i = 1; *p++ == *q++; i++ )
	{
		if( *q == 0 )
		{
			*pp = p;
			return i;
		}
	}

	return 0;
}


// ??????????????????????
int strint2( char **pp )
{
	char moji[1024] = "";
	char *p = *pp;

	getword( &p , moji );
	*pp = p;

	return atoi(moji);
}


// ????????????????long???????
long strlong2( char **pp )
{
	char moji[1024] = "";
	char *p = *pp;

	getword( &p , moji );
	*pp = p;

	return atol(moji);
}

// ???????????????
//   ??：NULL?? ... ??????????
//           NULL     ... ??????
inline unsigned char *searchDelimPoint( unsigned char *src, unsigned char delim )
{
	unsigned char *pt = src;

	while( 1 )
	{
		if( *pt == '\0' )
			return (unsigned char *)0;

		if( *pt < 0x80 )
		{
			// 1bayte????
			if( *pt == delim )
			{
				// ??????????????????
				pt++;
				return pt;
			}
			pt++;
		}
		else
		{
			// 2byte????
			pt++;
			if( *pt == '\0' )
				return (unsigned char *)0;
			pt++;
		}
	}
}


/*
   ??????????????????????????????????
   ??????????

  char *src : ?????
  char delim : ????
  int count : ????????????????
  int maxlen : out ???????????
  char *out : ?


  ??  0：????????
          1:??????
*/
int getStringToken( char *src, char delim, int count, int maxlen, char *out )
{
	int c = 1;
	int i;
	unsigned char *pt;

	pt = (unsigned char *)src;
	for( i = 0; i < count-1; i++ )
	{
		if( pt == (unsigned char *)0 )
			break;

		pt = searchDelimPoint( pt, delim );
	}

	if( pt == (unsigned char *)0 )
	{
		out[0] = '\0';
		return 1;
	}

	return copyStringUntilDelim( pt, delim, maxlen, (unsigned char *)out );
}


/*
  ?????????????????????????????

  char *src : ????
  char delim : ????
  int maxlen : ???????
  char *out : ?

  ??  0：????????
          1:??????
*/
static int copyStringUntilDelim( unsigned char *src, char delim,
	int maxlen, unsigned char *out )
{
	int i;

	for( i = 0; i < maxlen; i++ )
	{
		if( src[i] < 0x80 )
		{
			// 1byte????

			if( src[i] == delim )
			{
				// ????????
				out[i] = '\0';
				return 0;
			}

			// ????????
			out[i] = src[i];

			// ?????
			if( out[i] == '\0' )
				return 1;
		}
		else
		{
			// 2byte????

			// ????????
			out[i] = src[i];

			i++;
			if( i >= maxlen )	// ???????????
				break;

			// ????????
			out[i] = src[i];

			// ???????????????????
			if( out[i] == '\0' )
				return 1;
		}
	}

	out[i] = '\0';

	return 1;
}


/*
  ???????int??????

  char *src : ?????
  char delim: ?????????
  int count :????????? ??????????

  return value : 

*/
int getIntegerToken( char *src, char delim, int count )
{
	char s[128];

	getStringToken( src, delim, count, sizeof( s )-1, s );

	if( s[0] == '\0' )
		return -1;

	return atoi( s );
}


/*
  double???????????
  char *src : ?????
  char delim: ?????????
  int count :????????? ??????????

  return value: 
*/
double getDoubleToken( char *src, char delim, int count )
{
	char s[128];

	getStringToken( src, delim, count, sizeof( s )-1, s );

	return strtod( s , NULL );
}


/*
  chop??(UNIX????)

  char *src : ?????  ??????????????

*/
void chop( char *src )
{
	int i;

	for( i = 0; ; i++ )
	{
		if( src[i] == 0 )
			break;
		if( src[i] == '\n' && src[i+1] == '\0' )
		{
			src[i] = '\0';
			break;
		}
	}
}


/*
int??????????

  int *a : ??????
  int siz :  ?????
  int count : ???????????
  ????????????
*/
void shiftIntArray( int *a, int siz, int count )
{
	int i;

	for( i = 0; i < siz - count; i++ )
	{
		a[i] = a[i+count];
	}
	for( i = siz - count; i <siz; i++ )
	{
		a[i] = 0;
	}
}


// 62???int????
// 0-9,a-z(10-35),A-Z(36-61)
int a62toi( char *a )
{
	int ret = 0;
	int fugo = 1;

	while( *a != NULL )
	{
		ret *= 62;
		if( '0' <= (*a) && (*a) <= '9' )
			ret += (*a)-'0';
		else
		if( 'a' <= (*a) && (*a) <= 'z' )
			ret += (*a)-'a'+10;
		else
		if( 'A' <= (*a) && (*a) <= 'Z' )
			ret += (*a)-'A'+36;
		else
		if( *a == '-' )
			fugo = -1;
		else
			return 0;
		a++;
	}
	return ret*fugo;
}


/*
  ???????int??????

  char *src : ?????
  char delim: ?????????
  int count :????????? ??????????

  return value : 

*/
int getInteger62Token( char *src, char delim, int count )
{
	char  s[128];

	getStringToken( src, delim, count, sizeof( s )-1, s );
	if( s[0] == '\0' )
		return -1;

    return a62toi( s );
}


/*
  ???????????????

  char *s1 : ???
  char *s2 : ???
  int len : ?????


*/
int strncmpi( char *s1, char *s2, int len )
{
	int i;
	int c1, c2;

	for( i = 0; i < len; i++ )
	{
		if( s1[i] == '\0' || s2[i] == '\0' )
			return 0;

		c1 = tolower( s1[i] );
		c2 = tolower( s2[i] );

		if( c1 != c2 )
			return 1;
	}

	return 0;
}




RECT intToRect( int left, int top, int right, int bottom )
{
	RECT rc = { left, top, right, bottom };

	return rc;
}




/*****************************************************************
  ?????????????1??????????
  0???SJIS??
******************************************************************/
int isOnlySpaceChars( char *data )
{
	int i = 0;
	int returnflag = 0;

	while( data[i] != '\0' )
	{
		returnflag = 0;
		if( (unsigned char)data[i] == ' ' )
			returnflag = 1;
		if( IsDBCSLeadByte( data[i] ) )
		{
			if( (unsigned char)data[i] == 0x81
			 && (unsigned char)data[i+1] == 0x40 )
			{
				returnflag = 1;
			}
			i++;
		}
		if( returnflag == 0 )
			return 0;
		i++;
	}

	if( i == 0 )
		return 0;

	return returnflag;
}


/****************************
  ???????????
  buffer: ????
  string: ????
  whereToinsert: ??????
****************************/
void insertString( char *buffer, char *string, int whereToInsert )
{
	int stringLength, bufferLength, i;

	stringLength = strlen( string );
	bufferLength = strlen( buffer );

	for( i = 0; i <= bufferLength - whereToInsert; i++ )
	{
		buffer[bufferLength+stringLength-i] = buffer[bufferLength-i];
	}
	for( i = 0; i < stringLength; i++ )
	{
		buffer[whereToInsert+i] = string[i];
	}
}


/****************************
  ??????????
  buffer: ????
  character: ????
  whereToinsert: ??????
****************************/
void insertChar( char *buffer, char character, int whereToInsert )
{
	int bufferLength, i;

	bufferLength = strlen( buffer );

	for( i = 0; i <= bufferLength - whereToInsert; i++ )
	{
		buffer[bufferLength+1-i] = buffer[bufferLength-i];
	}
	buffer[whereToInsert] = character;
}


/*************************************************************
??????????????????
*************************************************************/
typedef struct tagEscapeChar
{
	char escapechar;
    char escapedchar;
} EscapeChar;

static EscapeChar escapeChar[] =
{
	{ '\n',   'n' },
	{ ',',    'c' },
	{ '|',    'z' },
	{ '\\',   'y' },
};


// ???????????
//   ??：?(?????????????????)
char makeCharFromEscaped( char c )
{
	int i;

	for( i = 0; i < sizeof( escapeChar )/sizeof( escapeChar[0] ); i++ )
	{
		if( escapeChar[i].escapedchar == c )
		{
			c = escapeChar[i].escapechar;
			break;
		}
	}

	return c;
}



/*----------------------------------------
 * makeEscapeString????????????
 * ??
 *  src             char*       ??????????????
 * ?
 *  src    ???(?????????????)
 ----------------------------------------*/
char *makeStringFromEscaped(char *src)
{
	int		srclen=strlen(src);
	int		searchindex=0;
	for(int i=0;i<srclen;i++){
		if(IsDBCSLeadByte(src[i])){
			src[searchindex++]=src[i++];
			src[searchindex++]=src[i];
		}else{
			if(src[i]=='\\'){
				int j;
				i++;
				for(j=0;j<sizeof(escapeChar)/sizeof(escapeChar[0]);j++)
					if(escapeChar[j].escapedchar==src[i]){
						src[searchindex++]=escapeChar[j].escapechar;
						goto NEXT;
					}
				src[searchindex++]=src[i];
			}else
				src[searchindex++]=src[i];
		}
NEXT:
		;
	}
	src[searchindex]='\0';
	return src;
}


/*----------------------------------------
 * ????????
 * ??
 *  src             char*       ??????
 *  dest            char*       ??????????
 *  sizeofdest      int         dest ? ???
 * ?
 *  dest    ???(?????????????)
 ----------------------------------------*/
char *makeEscapeString( char *src, char *dest, int sizeofdest )
{
	int		srclen=strlen(src);
	int		destindex=0;
	for(int i=0;i<srclen;i++){
		if(destindex+1 >= sizeofdest)
			break;
		if(IsDBCSLeadByte(src[i])){
			if(destindex+2 < sizeofdest){
				dest[destindex++]=src[i++];
				dest[destindex++]=src[i];
			}else
				break;
		}else{
			BOOL dirty=FALSE;
			char escapechar='\0';
			for(int j=0;j<sizeof(escapeChar)/sizeof(escapeChar[0]);j++)
				if(src[i]==escapeChar[j].escapechar){
					dirty=TRUE;
					escapechar=escapeChar[j].escapedchar;
					break;
				}
			if(dirty==TRUE){
				if( destindex + 2 < sizeofdest ){
					dest[destindex++]='\\';
					dest[destindex++]=escapechar;
				}else
					break;
			}else
				dest[destindex++]=src[i];
		}
    }
    dest[destindex] = '\0';
    return dest;
}


/***************************************************************
	????????? by Jun
***************************************************************/
/*
char *src	????adress
int srclen	????src????
int keystring	????????????
char *encoded	??????????????????????
int *encodedlen	???????????????????? 
int maxencodedlen ??????????
*/
void jEncode( char *src, int srclen, int key,
	char *encoded, int *encodedlen, int maxencodedlen )
{
	char sum = 0;
	int i;

	if( srclen+1 > maxencodedlen )
	{
		// ????????? 1 ????
		// ????????????????????????
		*encodedlen = maxencodedlen;
		for( i = 0; i < (*encodedlen); i++ )
		{
			encoded[i] = src[i];
		}
	}

	if( srclen+1 <= maxencodedlen )
	{
		// ??????????
		*encodedlen = srclen + 1;
		for( i = 0; i < srclen; i++ )
		{
			sum = sum + src[i];
			// ??7?????2????????????
			if( ((key%7) == (i%5)) || ((key%2) == (i%2)) )
			{
				src[i] = ~src[i];
			}
		}
		for( i = 0; i < (*encodedlen); i++ )
		{
			if( abs( (key%srclen) ) > i )
			{
				encoded[i] = src[i] + sum*((i*i)%3);	// ?????????
			}
			else
			// key%srclen??????????????
			if( abs( (key%srclen) ) == i )
			{
				encoded[i] = sum;
			}
			else
			if( abs( (key%srclen) ) < i )
			{
				encoded[i] = src[i-1] + sum*((i*i)%7);	// ?????????
			}
		}
	}
}


/*
char *src	?????????
int srclen	???????????????
int key		???????????
char *decoded	???????????
int *decodedlen	???????????
*/
void jDecode( char *src, int srclen, int key, char *decoded, int *decodedlen )
{
	char sum;
	int i;

	*decodedlen = srclen - 1;
	sum = src[abs( key%(*decodedlen) )];

	for( i=0; i < srclen; i++ )
	{
		if( abs( (key%(*decodedlen)) ) > i )
		{
			decoded[i] = src[i] - sum*((i*i)%3);
		}

		if( abs( (key%(*decodedlen)) ) < i )
		{
			decoded[i-1] = src[i] - sum*((i*i)%7);
		}
	}
	for( i = 0; i < (*decodedlen); i++ )
	{
		if( ((key%7) == (i%5)) || ((key%2) == (i%2)) )
		{
			decoded[i] = ~decoded[i];
		}
	}
}
