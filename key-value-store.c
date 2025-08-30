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

// return 1
// return -1 if an error occured
// else return 0 and update value_out to the value
int kv_get(void *key, size_t len, void **value_out, size_t *value_len_out) {
	fptr = fopen(FILENAME, "r");
	if (fptr == NULL) {
		return -1;
	}

	fseek(fptr, 0l, SEEK_END); // how do I get to the last line instead of the end of the file?
	long pos = ftell(fptr);
	if (pos == 0) return 1; // file was empty

	// go to the start of the line, or start of the file
while (pos > 0) {

		// go to the start of the line
		long line_start = 0;
		for (long p = pos -1; p >= 0; p--) {
			fseek(fptr, p, SEEK_SET);
			int c = fgetc(fptr);

			if (c == '\n') {
				line_start = p + 1;
				break;
			}
		}

		if (line_start == 0) {
			fseek(fptr, 0, SEEK_SET);
		} else {
			fseek(fptr, line_start, SEEK_SET);
		}

		// check the key
		char tmp_key[MAX_KEY_LEN];
		size_t i = 0;
		int c;
		// read character until comma
		// store all characters in a char array to later compare with the key param to this func
		while ((c = fgetc(fptr)) != ',' && c != EOF && i < MAX_KEY_LEN - 1) {
			tmp_key[i++] = c;
		}

		tmp_key[i] = '\0';

		if (i == len && memcmp(key, tmp_key, len) == 0) {
			long value_start = ftell(fptr);
			long value_end = value_start;

			while ((c = fgetc(fptr)) != '\n' && c != EOF) {
				value_end++;
			}

			size_t value_len = value_end - value_start;

			char *buf = malloc(value_len);
			if (buf == NULL) return -1;

			fseek(fptr, value_start, SEEK_SET);
			fread(buf, 1, value_len, fptr); 
			*value_out = buf;
			*value_len_out = value_len;

			fclose(fptr);
			return 0;
		}

		pos = line_start - 1;

	}
	fclose(fptr);
	return -1;
}

int kv_set(void* key, size_t key_len, void* value, size_t value_len) {
	fptr = fopen(FILENAME, "a");
	size_t line_size = key_len + value_len;

	size_t total_size = key_len + strlen(",") + value_len;

	size_t val1 = fwrite(key, 1, key_len, fptr);
	if (val1 == 0) return -1;
	size_t val2 = fwrite(",", 1, strlen(","), fptr);
	if (val2 == 0) return -1;
	size_t val3 = fwrite(value, 1, value_len, fptr);
	if (val3 == 0) return -1;
	size_t val4 = fwrite("\n", 1, strlen("\n"), fptr);
	if (val4 == 0) return -1;

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
