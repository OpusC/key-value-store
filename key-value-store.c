#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "database"
#define MAX_KEY_LEN 256

FILE *fptr;

void assert_equals_int(int expected, int actual) {
	if (expected != actual) {
		printf("Assertion Failed: expected %d but got %d\n", expected, actual);
	}
}

// return 1 if key not found
// return -1 if an error occured
// else return 0 and update value_out to the value
int kv_get(void *key, size_t len, void **value_out, size_t *value_len_out) {
	// printf("looking for key")

	fptr = fopen(FILENAME, "rb");
	if (fptr == NULL) {
		return -1;
	}

	fseek(fptr, 0l, SEEK_END); // Go to end of file
	long pos = ftell(fptr);
	if (pos == 0) return 1; // file was empty
	

while (pos > 0) {
		fseek(fptr, - sizeof(size_t), SEEK_CUR);

		// get record length
		size_t record_len; // maybe later check for max_key_len
		fread(&record_len, sizeof(size_t), 1, fptr);

		printf("record len %zu\n", record_len);

		fseek(fptr, -(record_len + sizeof(size_t)), SEEK_CUR); // go to the start of the record
		
		pos = ftell(fptr) - sizeof(size_t); // position is at the record_len of the last record now
		printf("pos: %ld\n", pos);

		char *buf = malloc(record_len);
		if (buf == NULL) {
			printf("malloc failed\n");
			fclose(fptr);
			return -1;
		}

		fread(buf, record_len, 1, fptr); // read entire record
		fseek(fptr, -(record_len), SEEK_CUR); // go to the start of the record

		printf("record len: %zu\n", record_len);

		size_t key_len;
		memcpy(&key_len, buf, sizeof(size_t));
		
		printf("size of read key: %zu\n size of requested key %zu\n", key_len, len);

		char *key_ptr = buf + sizeof(size_t);

		printf("set key ptr, about to enter if statement\n");

		if (key_len == len && memcmp(key, key_ptr, len) == 0) {

			printf("compared key\n");

			size_t value_len;
			char *value_len_ptr = key_ptr + key_len;
			memcpy(&value_len, value_len_ptr, sizeof(size_t));

			printf("copied value\n");

			char *value_ptr = value_len_ptr + sizeof(size_t);
			char *vbuf = malloc(value_len);

			printf("malloc vbuf\n");

			if (vbuf == NULL) {
				fclose(fptr);
				return -1;
			}
			memcpy(vbuf, value_ptr, value_len);

			printf("copied value\n");


			*value_out = vbuf;
			*value_len_out = value_len;
			printf("Record length=%zu, key_len=%zu, value_len=%zu, key='%.*s'\n", 
		  record_len, key_len, value_len, (int)key_len, key_ptr);

			free(buf);

			fclose(fptr);
			return 0;
		}

		printf("xyx\n");
		free(buf);

		printf("zyx\n");
	}

	fclose(fptr);
	return -1;
}

int kv_set(void* key, size_t key_len, void* value, size_t value_len) {
	fptr = fopen(FILENAME, "ab");


	if (fwrite(&key_len, sizeof(size_t), 1, fptr) != 1) return -1;

	if (fwrite(key, 1, key_len, fptr) != key_len) return -1;

	if (fwrite(&value_len, sizeof(size_t), 1, fptr) != 1) return -1;

	if (fwrite(value, 1, value_len, fptr) != value_len) return -1;

	size_t record_len = key_len + value_len + 2 * sizeof(size_t);

	if (fwrite(&record_len, sizeof(size_t), 1, fptr) != 1) return -1;

	printf("Wrote key_len=%zu, key=%.*s, value_len=%zu, record_len=%zu\n", key_len, (int)key_len, (char*)key, value_len, record_len);

	fclose(fptr);
	return 0;
}

int main(int argc, char *argv[]) {
	char key[] = "test1";
	char key1[] = "test2";
	int value = 3;

	int value2 = 7;


	kv_set(key, strlen(key), &value, sizeof(value));

	void *retrieved;
	size_t retrieved_len;

	kv_set(key1, strlen(key1), &value2, sizeof(value2));

	kv_get(key, strlen(key), &retrieved, &retrieved_len);

	assert_equals_int(value, *(int *) retrieved);

	int value1 = 4;

	kv_set(key, strlen(key), &value1, sizeof(value));

	kv_get(key, strlen(key), &retrieved, &retrieved_len);

	void *retrieved1;
	size_t retrieved1_len;
	kv_get(key1, strlen(key1), &retrieved1, &retrieved1_len);

	assert_equals_int(7, *(int *) retrieved1);

	assert_equals_int(value1, *(int *) retrieved);

	return 0;
}
