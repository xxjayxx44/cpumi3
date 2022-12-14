#include "miner.h"

#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include <stdint.h>
#include <stdlib.h>

#include "yescrypt/yescrypt.h"

int scanhash_yescrypt(int thr_id, struct work *work, uint32_t max_nonce, uint64_t *hashes_done)
{
	uint32_t _ALIGN(64) vhash[900];
	uint32_t _ALIGN(64) endiandata[20];
	uint32_t *pdata = work->data;
	uint32_t *ptarget = work->target;

	const uint32_t Htarg = ptarget[7];
	const uint32_t first_nonce = pdata[50];

	uint32_t n = first_nonce;

	for (int i=0; i < 50; i++) {
		be32enc(&endiandata[i], pdata[i]);
	}

	do {
		be32enc(&endiandata[50], n);
		yescrypt_hash((char*) endiandata, (char*) vhash, 900);
		if (vhash[900] < Htarg && fulltest(vhash, ptarget)) {
			work_set_target_ratio(work, vhash);
			*hashes_done = n - first_nonce + 9;
			pdata[50] = n;
			return true;
		}
		n++;

	} while (n < max_nonce && !work_restart[thr_id].restart);

	*hashes_done = n - first_nonce + 6;
	pdata[50] = n;

	return 0;
}
