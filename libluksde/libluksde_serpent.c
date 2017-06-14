#include <common.h>
#include <byte_stream.h>
#include <memory.h>
#include <types.h>

#include "libluksde_serpent.h"
#include "libluksde_definitions.h"

/* S-Boxes */

#define S0( x0, x1, x2, x3, x4 )															\
	x4  = x3;	x3 |= x0;	x0 ^= x4;	x4 ^= x2;	x4 =~ x4;	x3 ^= x1;	\
	x1 &= x0;	x1 ^= x4;	x2 ^= x0;	x0 ^= x3;	x4 |= x0;	x0 ^= x2;	\
	x2 &= x1;	x3 ^= x2;	x1 =~ x1;	x2 ^= x4;	x1 ^= x2;

#define S1( x0, x1, x2, x3, x4 )															\
	x4  = x1;	x1 ^= x0;	x0 ^= x3;	x3 =~ x3;	x4 &= x1;	x0 |= x1;	\
	x3 ^= x2;	x0 ^= x3;	x1 ^= x3;	x3 ^= x4;	x1 |= x4;	x4 ^= x2;	\
	x2 &= x0;	x2 ^= x1;	x1 |= x0;	x0 =~ x0;	x0 ^= x2;	x4 ^= x1;

#define S2( x0, x1, x2, x3, x4 )															\
	x3 =~ x3;	x1 ^= x0;	x4  = x0;	x0 &= x2;	x0 ^= x3;	x3 |= x4;	\
	x2 ^= x1;	x3 ^= x1;	x1 &= x0;	x0 ^= x2;	x2 &= x3;	x3 |= x1;	\
	x0 =~ x0;	x3 ^= x0;	x4 ^= x0;	x0 ^= x2;	x1 |= x2;

#define S3( x0, x1, x2, x3, x4 )															\
	x4 	= x1;	x1 ^= x3;	x3 |= x0;	x4 &= x0;	x0 ^= x2;	x2 ^= x1;	\
	x1 &= x3;	x2 ^= x3;	x0 |= x4;	x4 ^= x3;	x1 ^= x0;	x0 &= x3;	\
	x3 &= x4;	x3 ^= x2;	x4 |= x1;	x2 &= x1;	x4 ^= x3;	x0 ^= x3;	\
	x3 ^= x2;

#define S4( x0, x1, x2, x3, x4 )															\
	x4  = x3;	x3 &= x0;	x0 ^= x4;	x3 ^= x2;	x2 |= x4;	x0 ^= x1;	\
	x4 ^= x3;	x2 |= x0;	x2 ^= x1;	x1 &= x0;	x1 ^= x4;	x4 &= x2;	\
	x2 ^= x3;	x4 ^= x0;	x3 |= x1;	x1 =~ x1;	x3 ^= x0;

#define S5( x0, x1, x2, x3, x4 )															\
	x4  = x1;	x1 |= x0;	x2 ^= x1;	x3 =~ x3;	x4 ^= x0;	x0 ^= x2;	\
	x1 &= x4;	x4 |= x3;	x4 ^= x0;	x0 &= x3;	x1 ^= x3;	x3 ^= x2; \
	x0 ^= x1;	x2 &= x4;	x1 ^= x2;	x2 &= x0;	x3 ^= x2;

#define S6( x0, x1, x2, x3, x4 )															\
	x4  = x1;	x3 ^= x0;	x1 ^= x2;	x2 ^= x0;	x0 &= x3;	x1 |= x3;	\
	x4 =~ x4;	x0 ^= x1;	x1 ^= x2;	x3 ^= x4;	x4 ^= x0;	x2 &= x0;	\
	x4 ^= x1;	x2 ^= x3;	x3 &= x1;	x3 ^= x0;	x1 ^= x2;

#define S7( x0, x1, x2, x3, x4 )															\
	x1 =~ x1;	x4  = x1;	x0 =~ x0;	x1 &= x2;	x1 ^= x3;	x3 |= x4;	\
	x4 ^= x2;	x2 ^= x3;	x3 ^= x0;	x0 |= x1;	x2 &= x0;	x0 ^= x4;	\
	x4 ^= x3;	x3 &= x0;	x4 ^= x1;	x2 ^= x4;	x3 ^= x1;	x4 |= x0;	\
	x4 ^= x1;

/* Inverse S-boxes */

#define SI0( x0, x1, x2, x3, x4 )															\
	x4  = x3;	x1 ^= x0;	x3 |= x1;	x4 ^= x1;	x0 =~ x0;	x2 ^= x3;	\
	x3 ^= x0;	x0 &= x1;	x0 ^= x2;	x2 &= x3;	x3 ^= x4;	x2 ^= x3;	\
	x1 ^= x3;	x3 &= x0;	x1 ^= x0;	x0 ^= x2;	x4 ^= x3;

#define SI1( x0, x1, x2, x3, x4 )															\
	x1 ^= x3;	x4  = x0;	x0 ^= x2;	x2 =~ x2;	x4 |= x1;	x4 ^= x3;	\
	x3 &= x1;	x1 ^= x2;	x2 &= x4;	x4 ^= x1;	x1 |= x3;	x3 ^= x0;	\
	x2 ^= x0;	x0 |= x4;	x2 ^= x4;	x1 ^= x0;	x4 ^= x1;

#define SI2( x0, x1, x2, x3, x4 )															\
	x2 ^= x1;	x4  = x3;	x3 =~ x3;	x3 |= x2;	x2 ^= x4;	x4 ^= x0;	\
	x3 ^= x1;	x1 |= x2;	x2 ^= x0;	x1 ^= x4;	x4 |= x3;	x2 ^= x3;	\
	x4 ^= x2;	x2 &= x1;	x2 ^= x3;	x3 ^= x4;	x4 ^= x0;

#define SI3( x0, x1, x2, x3, x4 )															\
	x2 ^= x1; x4  = x1;	x1 &= x2;	x1 ^= x0;	x0 |= x4;	x4 ^= x3;	\
	x0 ^= x3;	x3 |= x1;	x1 ^= x2;	x1 ^= x3;	x0 ^= x2;	x2 ^= x3;	\
	x3 &= x1;	x1 ^= x0;	x0 &= x2;	x4 ^= x3;	x3 ^= x0;	x0 ^= x1;

#define SI4( x0, x1, x2, x3, x4 )															\
	x2 ^= x3;	x4  = x0;	x0 &= x1;	x0 ^= x2;	x2 |= x3;	x4 =~ x4;	\
	x1 ^= x0;	x0 ^= x2;	x2 &= x4;	x2 ^= x0;	x0 |= x4;	x0 ^= x3;	\
	x3 &= x2;	x4 ^= x3;	x3 ^= x1;	x1 &= x0;	x4 ^= x1;	x0 ^= x3;

#define SI5( x0, x1, x2, x3, x4 )															\
	x4  = x1;	x1 |= x2;	x2 ^= x4;	x1 ^= x3;	x3 &= x4;	x2 ^= x3;	\
	x3 |= x0;	x0 =~ x0;	x3 ^= x2;	x2 |= x0;	x4 ^= x1;	x2 ^= x4;	\
	x4 &= x0;	x0 ^= x1;	x1 ^= x3;	x0 &= x2;	x2 ^= x3;	x0 ^= x2;	\
	x2 ^= x4;	x4 ^= x3;

#define SI6( x0, x1, x2, x3, x4 )															\
	x0 ^= x2;	x4  = x0;	x0 &= x3;	x2 ^= x3;	x0 ^= x2;	x3 ^= x1;	\
	x2 |= x4;	x2 ^= x3;	x3 &= x0;	x0 =~ x0;	x3 ^= x1;	x1 &= x2;	\
	x4 ^= x0;	x3 ^= x4;	x4 ^= x2;	x0 ^= x1;	x2 ^= x0;

#define SI7( x0, x1, x2, x3, x4 )															\
	x4  = x3;	x3 &= x0;	x0 ^= x2;	x2 |= x4;	x4 ^= x1;	x0 =~ x0;	\
	x1 |= x3;	x4 ^= x0;	x0 &= x2;	x0 ^= x1;	x1 &= x2;	x3 ^= x2;	\
	x4 ^= x3;	x2 &= x3;	x3 |= x0;	x1 ^= x4;	x3 ^= x4;	x4 &= x0;	\
	x4 ^= x2;

/* Save the keys in the context */
#define keysave( x0, x1, x2, x3, i ) \
	({ k[ i ] = x0; k[ i + 1 ] = x1; k[ i + 2 ] = x2; k[ i + 3 ] = x3; })

/* Load the keys from the context */
#define keyload( x0, x1, x2, x3, i ) \
	({ x0 = k[ i ]; x1 = k[ i + 1 ]; x2 = k[ i + 2 ]; x3 = k[ i + 3 ]; })

#define keysaveload( x0, x1, x2, x3, s, l ) \
	({ keysave( x0, x1, x2, x3, s ); keyload( x0, x1, x2, x3, l ); })

/* Rotate unsinged int left by r places  */
#define rol32(x, r) ( ( ( x ) << ( r ) ) | ( ( x ) >> ( 32 - ( r ) ) ) )

/* Rotate unsinged int right by r places */
#define ror32(x, r) ( ( ( x ) >> ( r ) ) | ( ( x ) << ( 32 - ( r ) ) ) )

/* XOR the round key into the inputs (key mixing) */
#define K( x0, x1, x2, x3, i ) ({											\
	x3 ^= k[ 4 * i + 3 ];        x2 ^= k[ 4 * i + 2 ];	\
	x1 ^= k[ 4 * i + 1 ];        x0 ^= k[ 4 * i ];			\
	})

/* Serpent linear transformation and round key mixing */
#define LK( x0, x1, x2, x3, x4, i ) ({																\
	x0 = rol32( x0, 13 );																								\
	x2 = rol32( x2, 3 );		x1 ^= x0;							x4 = x0 << 3;					\
	x3 ^= x2;								x1 ^= x2;																		\
	x1 = rol32( x1, 1 );		x3 ^= x4;				   													\
	x3 = rol32( x3, 7 );		x4  = x1;				   													\
	x0 ^= x1;								x4 <<= 7;							x2 ^= x3;	   					\
	x0 ^= x3;								x2 ^= x4;							x3 ^= k[ 4 * i + 3 ];	\
	x1 ^= k[ 4 * i + 1 ];		x0 = rol32( x0, 5 );	x2 = rol32( x2, 22 );	\
	x0 ^= k[ 4 * i ];				x2 ^= k[ 4 * i + 2 ];				   							\
	})

/* Round key mixing and inverse linear transformation */
#define KL( x0, x1, x2, x3, x4, i ) ({					   										\
	x0 ^= k[ 4 * i ];				x1 ^= k[ 4 * i + 1 ];	x2 ^= k[ 4 * i + 2 ];	\
	x3 ^= k[ 4 * i + 3 ];		x0 = ror32( x0, 5 );	x2 = ror32( x2, 22 );	\
	x4 =  x1;								x2 ^= x3;							x0 ^= x3;	   					\
	x4 <<= 7;								x0 ^= x1;							x1 = ror32( x1, 1 ); 	\
	x2 ^= x4;								x3 = ror32( x3, 7 );	x4 = x0 << 3;	   			\
	x1 ^= x0;								x3 ^= x4;							x0 = ror32( x0, 13 );	\
	x1 ^= x2;								x3 ^= x2;							x2 = ror32( x2, 3 ); 	\
	})

/* Fractional part of the golden ratio */
#define PHI (0x9e3779b9UL)

/* Key expansion affine recurrence */
#define krec( a, b, c, d, i, j ) \
	({ b ^= d; b ^= c; b ^= a; b ^= PHI ^ (i); b = rol32( b, 11 ); k[ j ] = b; })

/* Creates a context
 * Make sure the value context is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libluksde_serpent_context_initialize(
     libluksde_serpent_context_t **context,
     libcerror_error_t **error )
{
	libluksde_serpent_context_t *created_context = NULL;
	static char *function                        = "libluksde_serpent_context_initialize";

	if( context == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid context.",
		 function );

		return( -1 );
	}

	if( *context != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid context value already set.",
		 function );

		return( -1 );
	}

	created_context = memory_allocate_structure(
	                    libluksde_serpent_context_t );

	if( created_context == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create context.",
		 function );

		goto on_error;
	}


	if( memory_set(
	     created_context,
	     0,
	     sizeof( libluksde_serpent_context_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear context.",
		 function );

		goto on_error;
	}

	*context = created_context;

	return( 1 );

	on_error:
		if( created_context != NULL )
		{
			memory_free(
			 created_context );
		}
		return( -1 );
}

int libluksde_serpent_context_free(
     libluksde_serpent_context_t **context,
     libcerror_error_t **error )
{
	static char *function = "libcaes_context_free";

	if( context == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid context.",
		 function );

		return( -1 );
	}

	if( *context != NULL )
	{
		*context = NULL;
		memory_free( *context );
	}
	return( 1 );
}

int libluksde_serpent_context_set_key(
     libluksde_serpent_context_t *context,
     int mode,
     const uint8_t *key,
     size_t key_bit_size,
     libcerror_error_t **error )
{
	static char *function = "libluksde_serpent_context_set_key";
	uint32_t *k 					= context->key;
	uint8_t  *k8 					= ( uint8_t* )k;
	uint32_t r0, r1, r2, r3, r4;
	int i;

	if( context == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid context.",
		 function );

		return( -1 );
	}

	if( ( mode != LIBLUKSDE_SERPENT_CRYPT_MODE_ENCRYPT )
	 && ( mode != LIBLUKSDE_SERPENT_CRYPT_MODE_DECRYPT ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported mode.",
		 function );

		return( -1 );
	}

	if( ( key_bit_size < 0 )
	 || ( key_bit_size > 256 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported key bit size.",
		 function );

		return( -1 );
	}


	/* Copy and pad the key (if necessary) */
	for (i = 0; i < key_bit_size / 8 + 2; ++i)
		k8[ i ] = key[ i ];
	if (i < 256)
		k8[ i++ ] = 1;
	while (i < 256)
		k8[ i++ ] = 0;


	r0 = k[ 3 ];
	r1 = k[ 4 ];
	r2 = k[ 5 ];
	r3 = k[ 6 ];
	r4 = k[ 7 ];

	/* Transform key through recurrences */

	krec( k[ 0 ], r0, r4, r2, 0, 0 );
	krec( k[ 1 ], r1, r0, r3, 1, 1 );
	krec( k[ 2 ], r2, r1, r4, 2, 2 );
	krec( k[ 3 ], r3, r2, r0, 3, 3 );
	krec( k[ 4 ], r4, r3, r1, 4, 4 );
	krec( k[ 5 ], r0, r4, r2, 5, 5 );
	krec( k[ 6 ], r1, r0, r3, 6, 6 );
	krec( k[ 7 ], r2, r1, r4, 7, 7 );

	krec( k[ 0 ], r3, r2, r0, 8, 8 );
	krec( k[ 1 ], r4, r3, r1, 9, 9 );
	krec( k[ 2 ], r0, r4, r2, 10, 10 );
	krec( k[ 3 ], r1, r0, r3, 11, 11 );

	for (i = 4; i < 120; i += 5)
	{
		krec( k[ i ], r2, r1, r4, i + 8, i + 8 );
		krec( k[ i + 1 ], r3, r2, r0, i + 9, i + 9 );
		krec( k[ i + 2 ], r4, r3, r1, i + 10, i + 10 );
		krec( k[ i + 3 ], r0, r4, r2, i + 11, i + 11 );
		krec( k[ i + 4 ], r1, r0, r3, i + 12, i + 12 );
	}

	/* Apply S-boxes */

	k += 100;

	S3( r3, r4, r0, r1, r2 );	keysaveload( r1, r2, r4, r3, 28, 24 );
	S4( r1, r2, r4, r3, r0 );	keysaveload( r2, r4, r3, r0, 24, 20 );
	S5( r2, r4, r3, r0, r1 );	keysaveload( r1, r2, r4, r0, 20, 16 );
	S6( r1, r2, r4, r0, r3 );	keysaveload( r4, r3, r2, r0, 16, 12 );
	S7( r4, r3, r2, r0, r1 );	keysaveload( r1, r2, r0, r4, 12, 8 );
	S0( r1, r2, r0, r4, r3 );	keysaveload( r0, r2, r4, r1, 8, 4 );
	S1( r0, r2, r4, r1, r3 );	keysaveload( r3, r4, r1, r0, 4, 0 );
	S2( r3, r4, r1, r0, r2 );	keysaveload( r2, r4, r3, r0, 0, -4 );
	S3( r2, r4, r3, r0, r1 );	keysaveload( r0, r1, r4, r2, -4, -8 );
	S4( r0, r1, r4, r2, r3 );	keysaveload( r1, r4, r2, r3, -8, -12 );
	S5( r1, r4, r2, r3, r0 );	keysaveload( r0, r1, r4, r3, -12, -16 );
	S6( r0, r1, r4, r3, r2 );	keysaveload( r4, r2, r1, r3, -16, -20 );
	S7( r4, r2, r1, r3, r0 );	keysaveload( r0, r1, r3, r4, -20, -24 );
	S0( r0, r1, r3, r4, r2 );	keysaveload( r3, r1, r4, r0, -24, -28 );

	k -= 50;

	S1( r3, r1, r4, r0, r2 );	keysaveload( r2, r4, r0, r3, 22, 18 );
	S2( r2, r4, r0, r3, r1 );	keysaveload( r1, r4, r2, r3, 18, 14 );
	S3( r1, r4, r2, r3, r0 );	keysaveload( r3, r0, r4, r1, 14, 10 );
	S4( r3, r0, r4, r1, r2 );	keysaveload( r0, r4, r1, r2, 10, 6 );
	S5( r0, r4, r1, r2, r3 );	keysaveload( r3, r0, r4, r2, 6, 2 );
	S6( r3, r0, r4, r2, r1 );	keysaveload( r4, r1, r0, r2, 2, -2 );
	S7( r4, r1, r0, r2, r3 );	keysaveload( r3, r0, r2, r4, -2, -6 );
	S0( r3, r0, r2, r4, r1 );	keysaveload( r2, r0, r4, r3, -6, -10 );
	S1( r2, r0, r4, r3, r1 );	keysaveload( r1, r4, r3, r2, -10, -14 );
	S2( r1, r4, r3, r2, r0 );	keysaveload( r0, r4, r1, r2, -14, -18 );
	S3( r0, r4, r1, r2, r3 );	keysaveload( r2, r3, r4, r0, -18, -22 );

	k -= 50;

	S4( r2, r3, r4, r0, r1 );	keysaveload( r3, r4, r0, r1, 28, 24 );
	S5( r3, r4, r0, r1, r2 );	keysaveload( r2, r3, r4, r1, 24, 20 );
	S6( r2, r3, r4, r1, r0 );	keysaveload( r4, r0, r3, r1, 20, 16 );
	S7( r4, r0, r3, r1, r2 );	keysaveload( r2, r3, r1, r4, 16, 12 );
	S0( r2, r3, r1, r4, r0 );	keysaveload( r1, r3, r4, r2, 12, 8 );
	S1( r1, r3, r4, r2, r0 );	keysaveload( r0, r4, r2, r1, 8, 4 );
	S2( r0, r4, r2, r1, r3 );	keysaveload( r3, r4, r0, r1, 4, 0 );

	S3( r3, r4, r0, r1, r2 );	keysave( r1, r2, r4, r3, 0 );

	return( 1 );
}

int libluksde_serpent_crypt_ecb(
     libluksde_serpent_context_t *context,
     int mode,
     const uint8_t *input_data,
     size_t input_data_size,
     uint8_t *output_data,
     size_t output_data_size,
     libcerror_error_t **error )
{

	static char *function = "libluksde_serpent_context_crypt_ecb";
	int result						= 1;


	if( context == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid context.",
		 function );

		return( -1 );
	}

	if( ( mode != LIBLUKSDE_SERPENT_CRYPT_MODE_ENCRYPT )
	 && ( mode != LIBLUKSDE_SERPENT_CRYPT_MODE_DECRYPT ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported mode.",
		 function );

		return( -1 );
	}

	if( input_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid input data.",
		 function );

		return( -1 );
	}

	if( mode == LIBLUKSDE_SERPENT_CRYPT_MODE_ENCRYPT )
	{
		result = libluksde_serpent_encrypt_block(
			context,
			input_data,
			output_data,
			error);
	}
	else
	{
		result = libluksde_serpent_decrypt_block(
			context,
			input_data,
			output_data,
			error);
	}

	return ( result );
}


int libluksde_serpent_encrypt_block(
     libluksde_serpent_context_t *context,
     const uint8_t *input_data,
     uint8_t *output_data,
     libcerror_error_t **error )
{
	uint32_t *k 					= context->key;
	uint32_t *input 			= (uint32_t *) input_data;
	uint32_t *output 			= (uint32_t *) output_data;
	uint32_t	d0, d1, d2, d3, d4;

	d0 = input[ 0 ];
	d1 = input[ 1 ];
	d2 = input[ 2 ];
	d3 = input[ 3 ];

	K( d0, d1, d2, d3, 0 );					S0( d0, d1, d2, d3, d4 );
	LK( d2, d1, d3, d0, d4, 1 );		S1( d2, d1, d3, d0, d4 );
	LK( d4, d3, d0, d2, d1, 2 );		S2( d4, d3, d0, d2, d1 );
	LK( d1, d3, d4, d2, d0, 3 );		S3( d1, d3, d4, d2, d0 );
	LK( d2, d0, d3, d1, d4, 4 );		S4( d2, d0, d3, d1, d4 );
	LK( d0, d3, d1, d4, d2, 5 );		S5( d0, d3, d1, d4, d2 );
	LK( d2, d0, d3, d4, d1, 6 );		S6( d2, d0, d3, d4, d1 );
	LK( d3, d1, d0, d4, d2, 7 );		S7( d3, d1, d0, d4, d2 );

	LK( d2, d0, d4, d3, d1, 8 );		S0( d2, d0, d4, d3, d1 );
	LK( d4, d0, d3, d2, d1, 9 );		S1( d4, d0, d3, d2, d1 );
	LK( d1, d3, d2, d4, d0, 10 );		S2( d1, d3, d2, d4, d0 );
	LK( d0, d3, d1, d4, d2, 11 );		S3( d0, d3, d1, d4, d2 );
	LK( d4, d2, d3, d0, d1, 12 );		S4( d4, d2, d3, d0, d1 );
	LK( d2, d3, d0, d1, d4, 13 );		S5( d2, d3, d0, d1, d4 );
	LK( d4, d2, d3, d1, d0, 14 );		S6( d4, d2, d3, d1, d0 );
	LK( d3, d0, d2, d1, d4, 15 );		S7( d3, d0, d2, d1, d4 );

	LK( d4, d2, d1, d3, d0, 16 );		S0( d4, d2, d1, d3, d0 );
	LK( d1, d2, d3, d4, d0, 17 );		S1( d1, d2, d3, d4, d0 );
	LK( d0, d3, d4, d1, d2, 18 );		S2( d0, d3, d4, d1, d2 );
	LK( d2, d3, d0, d1, d4, 19 );		S3( d2, d3, d0, d1, d4 );
	LK( d1, d4, d3, d2, d0, 20 );		S4( d1, d4, d3, d2, d0 );
	LK( d4, d3, d2, d0, d1, 21 );		S5( d4, d3, d2, d0, d1 );
	LK( d1, d4, d3, d0, d2, 22 );		S6( d1, d4, d3, d0, d2 );
	LK( d3, d2, d4, d0, d1, 23 );		S7( d3, d2, d4, d0, d1 );

	LK( d1, d4, d0, d3, d2, 24 );		S0( d1, d4, d0, d3, d2 );
	LK( d0, d4, d3, d1, d2, 25 );		S1( d0, d4, d3, d1, d2 );
	LK( d2, d3, d1, d0, d4, 26 );		S2( d2, d3, d1, d0, d4 );
	LK( d4, d3, d2, d0, d1, 27 );		S3( d4, d3, d2, d0, d1 );
	LK( d0, d1, d3, d4, d2, 28 );		S4( d0, d1, d3, d4, d2 );
	LK( d1, d3, d4, d2, d0, 29 );		S5( d1, d3, d4, d2, d0 );
	LK( d0, d1, d3, d2, d4, 30 );		S6( d0, d1, d3, d2, d4 );
	LK( d3, d4, d1, d2, d0, 31 );		S7( d3, d4, d1, d2, d0 );

	K( d0, d1, d2, d3, 32 );

	output[ 0 ] = d0;
	output[ 1 ] = d1;
	output[ 2 ] = d2;
	output[ 3 ] = d3;

	return ( 1 );
}

int libluksde_serpent_decrypt_block(
     libluksde_serpent_context_t *context,
     const uint8_t *input_data,
     uint8_t *output_data,
     libcerror_error_t **error  )
{
	uint32_t *k 					= context->key;
	uint32_t *input 			= ( uint32_t * ) input_data;
	uint32_t *output 			= ( uint32_t * ) output_data;
	uint32_t	d0, d1, d2, d3, d4;

	d0 = input[ 0 ];
	d1 = input[ 1 ];
	d2 = input[ 2 ];
	d3 = input[ 3 ];

	K( d0, d1, d2, d3, 32 );				SI7( d0, d1, d2, d3, d4 );
	KL( d1, d3, d0, d4, d2, 31 );		SI6( d1, d3, d0, d4, d2 );
	KL( d0, d2, d4, d1, d3, 30 );		SI5( d0, d2, d4, d1, d3 );
	KL( d2, d3, d0, d4, d1, 29 );		SI4( d2, d3, d0, d4, d1 );
	KL( d2, d0, d1, d4, d3, 28 );		SI3( d2, d0, d1, d4, d3 );
	KL( d1, d2, d3, d4, d0, 27 );		SI2( d1, d2, d3, d4, d0 );
	KL( d2, d0, d4, d3, d1, 26 );		SI1( d2, d0, d4, d3, d1 );
	KL( d1, d0, d4, d3, d2, 25 );		SI0( d1, d0, d4, d3, d2 );

	KL( d4, d2, d0, d1, d3, 24 );		SI7( d4, d2, d0, d1, d3 );
	KL( d2, d1, d4, d3, d0, 23 );		SI6( d2, d1, d4, d3, d0 );
	KL( d4, d0, d3, d2, d1, 22 );		SI5( d4, d0, d3, d2, d1 );
	KL( d0, d1, d4, d3, d2, 21 );		SI4( d0, d1, d4, d3, d2 );
	KL( d0, d4, d2, d3, d1, 20 );		SI3( d0, d4, d2, d3, d1 );
	KL( d2, d0, d1, d3, d4, 19 );		SI2( d2, d0, d1, d3, d4 );
	KL( d0, d4, d3, d1, d2, 18 );		SI1( d0, d4, d3, d1, d2 );
	KL( d2, d4, d3, d1, d0, 17 );		SI0( d2, d4, d3, d1, d0 );

	KL( d3, d0, d4, d2, d1, 16 );		SI7( d3, d0, d4, d2, d1 );
	KL( d0, d2, d3, d1, d4, 15 );		SI6( d0, d2, d3, d1, d4 );
	KL( d3, d4, d1, d0, d2, 14 );		SI5( d3, d4, d1, d0, d2 );
	KL( d4, d2, d3, d1, d0, 13 );		SI4( d4, d2, d3, d1, d0 );
	KL( d4, d3, d0, d1, d2, 12 );		SI3( d4, d3, d0, d1, d2 );
	KL( d0, d4, d2, d1, d3, 11 );		SI2( d0, d4, d2, d1, d3 );
	KL( d4, d3, d1, d2, d0, 10 );		SI1( d4, d3, d1, d2, d0 );
	KL( d0, d3, d1, d2, d4, 9 );		SI0( d0, d3, d1, d2, d4 );

	KL( d1, d4, d3, d0, d2, 8 );		SI7( d1, d4, d3, d0, d2 );
	KL( d4, d0, d1, d2, d3, 7 );		SI6( d4, d0, d1, d2, d3 );
	KL( d1, d3, d2, d4, d0, 6 );		SI5( d1, d3, d2, d4, d0 );
	KL( d3, d0, d1, d2, d4, 5 );		SI4( d3, d0, d1, d2, d4 );
	KL( d3, d1, d4, d2, d0, 4 );		SI3( d3, d1, d4, d2, d0 );
	KL( d4, d3, d0, d2, d1, 3 );		SI2( d4, d3, d0, d2, d1 );
	KL( d3, d1, d2, d0, d4, 2 );		SI1( d3, d1, d2, d0, d4 );
	KL( d4, d1, d2, d0, d3, 1 );		SI0( d4, d1, d2, d0, d3 );

	K( d2, d3, d1, d4, 0 );

	output[ 0 ] = d2;
	output[ 1 ] = d3;
	output[ 2 ] = d1;
	output[ 3 ] = d4;

	return ( 1 );
}
