/*
 * %CopyrightBegin%
 *
 * Copyright Ericsson AB 2010-2018. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * %CopyrightEnd%
 */

#include "info.h"

#ifdef HAVE_DYNAMIC_CRYPTO_LIB

# if defined(DEBUG)
char *crypto_callback_name = "crypto_callback.debug";
# elif defined(VALGRIND)
char *crypto_callback_name = "crypto_callback.valgrind";
# else
char *crypto_callback_name = "crypto_callback";
# endif

int change_basename(ErlNifBinary* bin, char* buf, int bufsz, const char* newfile)
{
    int i;

    for (i = bin->size; i > 0; i--) {
	if (bin->data[i-1] == '/')
	    break;
    }
    if (i + strlen(newfile) >= bufsz) {
	PRINTF_ERR0("CRYPTO: lib name too long");
	return 0;
    }
    memcpy(buf, bin->data, i);
    strcpy(buf+i, newfile);
    return 1;
}

void error_handler(void* null, const char* errstr)
{
    PRINTF_ERR1("CRYPTO LOADING ERROR: '%s'", errstr);
}
#endif /* HAVE_DYNAMIC_CRYPTO_LIB */

ERL_NIF_TERM info_lib(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    /* [{<<"OpenSSL">>,9470143,<<"OpenSSL 0.9.8k 25 Mar 2009">>}] */

    static const char libname[] = "OpenSSL";
    unsigned name_sz = strlen(libname);
    const char* ver = SSLeay_version(SSLEAY_VERSION);
    unsigned ver_sz = strlen(ver);
    ERL_NIF_TERM name_term, ver_term;
    int ver_num = OPENSSL_VERSION_NUMBER;
    /* R16:
     * Ignore library version number from SSLeay() and instead show header
     * version. Otherwise user might try to call a function that is implemented
     * by a newer library but not supported by the headers used at compile time.
     * Example: DES_ede3_cfb_encrypt in 0.9.7i but not in 0.9.7d.
     *
     * Version string is still from library though.
     */

    memcpy(enif_make_new_binary(env, name_sz, &name_term), libname, name_sz);
    memcpy(enif_make_new_binary(env, ver_sz, &ver_term), ver, ver_sz);

    return enif_make_list1(env, enif_make_tuple3(env, name_term,
						 enif_make_int(env, ver_num),
						 ver_term));
}
