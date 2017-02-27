/*
 * vim: ts=8 sw=8
 */

#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#define	DEFAULT_WIDTH	(78)
#define	min(x,y)	( (x) < (y) ? (x) : (y) )

static	char *		me = "shorten";
static	unsigned	nonfatal;
static	unsigned	width = DEFAULT_WIDTH;
static	char *		ofile;
static	unsigned	debugLevel;
static	unsigned	min_margin = 1 + (DEFAULT_WIDTH/2);

static	void
debug(
	unsigned		theLevel,
	char const * const	fmt,
	...
)
{
	if( theLevel <= debugLevel )	{
		va_list		ap;

		va_start( ap, fmt );
		vprintf( fmt, ap );
		va_end( ap );
		printf( "\n" );
	}
}

static	void
usage(
	char *		fmt,
	...
)
{
	if( fmt )	{
		va_list	ap;

		fprintf( stderr, "%s: ", me );
		va_start( ap, fmt );
		vfprintf( stderr, fmt, ap );
		va_end( ap );
		fprintf( stderr, ".\n" );
	}
	fprintf(
		stderr,
		"usage:"
		" %s"
		" [-D]"
		" [-h]"
		" [-l len]"
		" [-m min_margin]"
		" [-o ofile]"
		" [file...]"
		"\n",
		me
	);
}

static	void
process(
	char const * const	fn,
	FILE * const		fyle
)
{
	char			line[ BUFSIZ ];

	debug( 1, "Processing '%s'", fn );
	while( fgets( line, sizeof( line ), fyle ) )	{
		size_t		len;
		char *		bp;

		/* Remove '\n' from the line				*/
		len = strlen( line );
		line[ --len ] = '\0';
		debug( 3, "original line='%s'", line );
		/* Find first space before the margin			*/
		for( bp = line; (len = strlen( bp )) > width; )	{
			char *		margin;
			char *		col;

			/* Find first space before the desired margin	*/
			margin = min( bp + width, bp + len );
			for(
				col = margin;
				(col > bp) && !isspace( *col );
				--col
			);
			if( col <= (bp + min_margin) )	{
				col = min( (bp + min_margin), (bp + len) );
			}
			/* Print the line, up to the stopping point	*/
			len = col - bp;
			printf( "%.*s", len, bp );
			/* Pad the line to the margin width		*/
			for( ; len < width; ++len )	{
				printf( " " );
			}
			/* Output the continuation marker		*/
			printf( " \\\n" );
			/* Skip any leading whitespace on remainder	*/
			bp = col;
			while( *bp && isspace( *bp ) )	{
				++bp;
			}
			debug( 3, "remainder = '%s'", bp );
		}
		if( *bp )	{
			printf( "%s\n", bp );
		}
	}
}

int
main(
	int		argc,
	char * *	argv
)
{
	char *		bp;
	int		c;

	/* Figure out our process name					*/
	me = argv[ 0 ];
	if( (bp = strrchr( me, '/' ) ) != NULL )	{
		me = bp + 1;
	}
	/* Process any command-line arguments				*/
	opterr = 0;
	while( (c = getopt( argc, argv, "Dhl:m:o:" )) != EOF )	{
		switch( c )	{
		default:
			fprintf(
				stderr,
				"%s: switch -%c is not implemented.\n",
				me,
				c
			);
			++nonfatal;
			break;
		case '?':
			fprintf(
				stderr,
				"%s: unknown -%c switch.\n",
				me,
				optopt
			);
			++nonfatal;
			break;
		case 'D':
			++debugLevel;
			break;
		case 'h':
			usage( NULL );
			exit( 0 );
			/*NOTREACHED*/
		case 'l':
			width = atoi( optarg );
			if( (width < 8) || (width >= BUFSIZ) )	{
				width = DEFAULT_WIDTH;
			}
			debug( 2, "width=%u", width );
			break;
		case 'm':
			min_margin = atoi( optarg );
			break;
		case 'o':
			ofile = optarg;
			debug( 2, "ofile='%s'", ofile );
			break;
		}
	}
	do	{
		if( nonfatal )	{
			usage( "Illegal argument(s)" );
			break;
		}
		if( ofile )	{
			(void) unlink( ofile );
			if( freopen( ofile, "wt", stdout ) != stdout )	{
				fprintf(
					stderr,
					"%s: cannot redirect stdout to '%s'.\n",
					me,
					ofile
				);
				++nonfatal;
				break;
			}
		}
		if( optind < argc )	{
			/* Take input file names from the command line	*/
			while( optind < argc )	{
				FILE *	fyle;

				optarg = argv[ optind++ ];
				fyle = fopen( optarg, "rt" );
				if( !fyle )	{
					fprintf(
						stderr,
						"%s: "
						"cannot open '%s' for reading"
						"; errno=%d (%s).\n",
						me,
						optarg,
						errno,
						strerror( errno )
					);
					++nonfatal;
					continue;
				}
				process( optarg, fyle );
				fclose( fyle );
			}
		} else	{
			/* Process lines from stdin			*/
			process( "{stdin}", stdin );
		}
	}
	while( 0 );
	return( nonfatal ? 1 : 0 );
}
