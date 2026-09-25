/*
 * sha1.h
 *
 * Originally taken from the public domain SHA1 implementation
 * written by by Steve Reid <steve@edmweb.com>
 *
 * Modified by Aaron D. Gifford <agifford@infowest.com>
 *
 * Modified by José Bollo <jose.bollo@iot.bzh>
 *
 * NO COPYRIGHT - THIS IS 100% IN THE PUBLIC DOMAIN
 *
 * The original unmodified version is available at:
 *    ftp://ftp.funet.fi/pub/crypt/hash/sha/sha1.c
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR(S) OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#pragma once

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/**
 * SHA1 hash code are SHA1_DIGEST_LENGTH bytes length
 */
#define SHA1_DIGEST_LENGTH	20

/**
 * The structure used for computing a SHA1
 */
typedef struct {
	/** current computation state */
	uint32_t   state[5];

	/** current length */
	uint64_t   length;

	/** internal buffer */
	union {
		/** as bytes */
		uint8_t    buffer[64];
		/** as uint32 */
		uint32_t   blocks[16];
	};
} SHA1_t;

/**
 * Initialize the structure context used for computing a SHA1
 *
 * @param context the context to initialize
 */
extern void SHA1_init(SHA1_t *context);

/**
 * Update the SHA computation for the given data of len
 *
 * @param context the context to update
 * @param data data of updating
 * @param length of the update
 */
extern void SHA1_update(SHA1_t *context, const void *data, size_t len);

/**
 * Terminates the computation of the SHA1 of context and return its value
 * in digest
 *
 * @param context the context to terminate
 * @param digest where to store the result
 */
extern void SHA1_final(SHA1_t *context, uint8_t digest[SHA1_DIGEST_LENGTH]);

#ifdef	__cplusplus
}
#endif
