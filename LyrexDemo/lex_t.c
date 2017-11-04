#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#define BUF_SIZE 100

/*define*/
/*methods*/

int main() {
	char buf[BUF_SIZE];
	FILE* in_file;
	while (1) {
		printf_s("Enter file name:\n");
		gets_s(buf, BUF_SIZE);
		errno_t err = fopen_s(&in_file, buf, "r");
		if (err == 0) break;
		else printf_s("Fail to open file.\n");
	}
	fseek(in_file, 0, SEEK_END);
	int size = ftell(in_file);
	char* input_buf = (char*)malloc(size);
	fseek(in_file, 0, SEEK_SET);
	fread_s(input_buf, size, sizeof(char), size, in_file);
	for (int i = size - 1; input_buf[i] < 0; i--) input_buf[i] = '\0';

	char* ptr = input_buf;
	while (1) {
		/*attempts*/
		if (*ptr == 0) break;
	}
}