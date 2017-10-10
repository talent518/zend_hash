#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "zend_hash.h"

int main(int argc, char **argv) {
	HashTable ht;
	HashPosition pos;
	char *str_key, *str_val;
	ulong num_key;
	uint str_key_len;
	int key_type;

	zend_hash_init(&ht, 2, NULL);

	zend_hash_add(&ht, "aaa", 3, "1111111111111111", 0, NULL);
	zend_hash_add(&ht, "bbb", 3, "2222222222222222", 0, NULL);
	zend_hash_add(&ht, "ccc", 3, "3333333333333333", 0, NULL);
	zend_hash_index_update(&ht, 4, "4444444444444444", 0, NULL);

    str_val = NULL;
    zend_hash_find(&ht, "bbb", 3, (void**)&str_val);
    printf("find(bbb) = %s\n", str_val);

	zend_hash_internal_pointer_reset_ex(&ht, &pos);
	while (zend_hash_get_current_data_ex(&ht, (void **)&str_val, &pos) == SUCCESS) {
		key_type = zend_hash_get_current_key_ex(&ht, &str_key, &str_key_len, &num_key, 0, &pos);

		switch(key_type) {
			case HASH_KEY_IS_STRING:
				printf("%s => %s\n", str_key, str_val);
				break;
			case HASH_KEY_IS_LONG:
				printf("%ld => %s\n", num_key, str_val);
				break;
			EMPTY_SWITCH_DEFAULT_CASE()
		}

		zend_hash_move_forward_ex(&ht, &pos);
	}
	
	zend_hash_destroy(&ht);

	return 0;
}
