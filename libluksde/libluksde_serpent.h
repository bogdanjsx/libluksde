#if !defined( _LIBLUKSDE_SERPENT_H )
#define _LIBLUKSDE_SERPENT_H

#include <common.h>
#include <types.h>

#include "libluksde_libcerror.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libluksde_serpent_context libluksde_serpent_context_t;

struct libluksde_serpent_context
{
	uint32_t key[ 132 ];
};

int libluksde_serpent_context_initialize(
     libluksde_serpent_context_t **context,
     libcerror_error_t **error );

int libluksde_serpent_context_free(
     libluksde_serpent_context_t **context,
     libcerror_error_t **error );

int libluksde_serpent_context_set_key(
     libluksde_serpent_context_t *context,
     int mode,
     const uint8_t *key,
     size_t key_bit_size,
     libcerror_error_t **error );

int libluksde_serpent_crypt_ecb(
     libluksde_serpent_context_t *context,
     int mode,
     const uint8_t *input_data,
     size_t input_data_size,
     uint8_t *output_data,
     size_t output_data_size,
     libcerror_error_t **error );

int libluksde_serpent_encrypt_block(
     libluksde_serpent_context_t *context,
     const uint8_t *input_data,
     uint8_t *output_data,
     libcerror_error_t **error );

int libluksde_serpent_decrypt_block(
     libluksde_serpent_context_t *context,
     const uint8_t *input_data,
     uint8_t *output_data,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif
