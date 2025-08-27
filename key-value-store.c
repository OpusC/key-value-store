#include <stddef.h>
#include <stdio.h>
#include <string.h>

void assert_equals_int(int expected, int actual) {
		if (expected != actual) {
				printf("Assertion Failed: expected %d but got %d\n", expected, actual);
		}
}

int kv_get(void *key, size_t len) {
		return 1; // stub
}

int kv_set(void* key, size_t key_len, void* value, size_t value_len) {
		return 1; // stub
}

int main(int argc, char *argv[]) {
		char key[] = "test1";
		int value = 3;

		kv_set(key, strlen(key), &value, sizeof(value));

		int retrieve_value = kv_get(key, strlen(key));

		assert_equals_int(value, retrieve_value);

		return 0;
}
