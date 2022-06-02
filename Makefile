#!/bin/bash
gcc -std=gnu99 -o keygen keygen.c
gcc -std=gnu99 -o enc_server enc_server.c
gcc -std=gnu99 -o enc_client enc_client.c
gcc -std=gnu99 -o dec_server dec_server.c
gcc -std=gnu99 -o dec_client dec_client.c

# main:
# 	gcc -std=gnu99 -Wall -g -o keygen keygen.c
# 	gcc -std=gnu99 -Wall -g -o enc_server enc_server.c
# 	gcc -std=gnu99 -Wall -g -o enc_client enc_client.c
# 	gcc -std=gnu99 -Wall -g -o dec_server dec_server.c
# 	gcc -std=gnu99 -Wall -g -o dec_client dec_client.c
# clean:
# 	rm -f keygen
# 	rm -f enc_server
# 	rm -f enc_client
# 	rm -f dec_server
# 	rm -f dec_client