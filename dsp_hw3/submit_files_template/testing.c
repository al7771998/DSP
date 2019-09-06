#include <iconv.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
int main( int argc, char **argv )
{
	iconv_t cd;
	size_t in_s, out_s;

	/* UTF-8 轉 Big5 */
	cd = iconv_open("BIG-5", "UTF8");

	char *ibuf = "我是阿信", *in_ptr;
	char *obuf = NULL, *out_ptr;

	in_s = strlen(ibuf);
	in_ptr = ibuf;

	obuf = malloc( in_s * 3 );
	out_s = in_s * 3;
	out_ptr = obuf;

	if( cd == (iconv_t )-1 ){
		fprintf( stderr, "error opening iconv \n" );
		exit(1);
	}

	if( iconv( cd, &in_ptr, &in_s, &out_ptr, &out_s) == -1 ){
		printf("errno: %s\n", strerror(errno));
	}

	*out_ptr = '\0';
	printf("%s, %s\n",ibuf, obuf);

	iconv_close( cd );
	free( obuf );
	return 0;
}