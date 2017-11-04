#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<iostream>
#define YY_SIZE 1024
#define BUF_SIZE 100

int dfa_O(const char* iptr) {
	if (*iptr >= '0'&&*iptr <= '7') return 1;
	else return 0;
}

int dfa_D(const char* iptr) {
	if (*iptr >= '0'&&*iptr <= '9') return 1;
	else return 0;
}

int dfa_NZ(const char* iptr) {
	if (*iptr >= '1'&&*iptr <= '9') return 1;
	else return 0;
}

int dfa_L(const char* iptr) {
	if ((*iptr >= 'a'&&*iptr <= 'z') || (*iptr >= 'A'&&*iptr <= 'Z') || *iptr == '_') return 1;
	else return 0;
}

int dfa_A(const char* iptr) {
	if ((*iptr >= '0'&&*iptr <= '9') || (*iptr >= 'a'&&*iptr <= 'z') || (*iptr >= 'A'&&*iptr <= 'Z') || *iptr == '_') return 1;
	else return 0;
}

int dfa_H(const char* iptr) {
	if ((*iptr >= '0'&&*iptr <= '9') || (*iptr >= 'a'&&*iptr <= 'f') || (*iptr >= 'A'&&*iptr <= 'F')) return 1;
	else return 0;
}

int dfa_HP(const char* iptr) {
	int state = 1, len = 0;
	while (1) {
		switch (state) {
		case 1:
			if (*iptr == '0') state = 2;
			else return 0;
			break;
		case 2:
			if (*iptr == 'x') state = 3;
			else if (*iptr == 'X') state = 3;
			else return len;
			break;
		case 3:
			return len;
		}
		iptr++;
		len++;
	}
}

int dfa_E(const char* iptr) {
	int state = 1, len = 0;
	while (1) {
		switch (state) {
		case 1:
			if (*iptr == 'E') state = 2;
			else if (*iptr == 'e') state = 2;
			else return 0;
			break;
		case 2:
			if (*iptr == '+') state = 3;
			else if (*iptr == '-') state = 3;
			else if (dfa_D(iptr)) state = 4;
			else return 0;
			break;
		case 3:
			if (dfa_D(iptr)) state = 4;
			else return 0;
			break;
		case 4:
			if (dfa_D(iptr)) state = 4;
			else return len;
		}
		iptr++;
		len++;
	}
}

int dfa_FS(const char* iptr) {
	if (*iptr == 'f'|| *iptr == 'F' || *iptr == 'l' || *iptr == 'L') return 1;
	else return 0;
}

int dfa_IS(const char* iptr) {
	int state = 1, len = 0;
	while (1) {
		switch (state) {
		case 1:
			if (*iptr == 'u' || *iptr == 'U') state = 2;
			else if (*iptr == 'l' || *iptr == 'L') state = 5;
			else return 0;
			break;
		case 2:
			if (*iptr == 'l' || *iptr == 'L') state = 3;
			else return len;
			break;
		case 3:
			if (*iptr == 'l' || *iptr == 'L') state = 4;
			else return len;
			break;
		case 4:
			return len;
		case 5:
			if (*iptr == 'l' || *iptr == 'L') state = 6;
			else return len;
			break;
		case 6:
			if (*iptr == 'u' || *iptr == 'U') state = 7;
			else return len;
			break;
		case 7:
			return len;
		}
		iptr++;
		len++;
	}
}

int dfa_CP(const char* iptr) {
	if (*iptr == 'u' || *iptr == 'U' || *iptr == 'L') return 1;
	else return 0;
}

int dfa_SP(const char* iptr) {
	int state = 1, len = 0;
	while (1) {
		switch (state) {
		case 1:
			if (*iptr == 'u' || *iptr == 'U' || *iptr == 'L') state = 2;
			else return 0;
			break;
		case 2:
			if (*iptr == '8') state = 3;
			else return len;
			break;
		case 3:
			return len;
		}
		iptr++;
		len++;
	}
}

int dfa_ES(const char* iptr) {
	int state = 1, len = 0;
	while (1) {
		switch (state) {
		case 1:
			if (*iptr == '\\') state = 2;
			else return 0;
			break;
		case 2:
			if (strchr("'\"?\\abfnrtv", *iptr)) state = 3;
			else if (dfa_O(iptr)) state = 4;
			else if (*iptr == 'x') state = 7;
			else return 0;
			break;
		case 3:
			return len;
		case 4:
			if (dfa_O(iptr)) state = 5;
			else return len;
			break;
		case 5:
			if (dfa_O(iptr)) state = 6;
			else return len;
			break;
		case 6:
			return len;
		case 7:
			if (dfa_H(iptr)) state = 8;
			else return len;
			break;
		case 8:
			if (dfa_H(iptr)) state = 8;
			else return len;
			break;
		}
		iptr++;
		len++;
	}
}

int dfa_WS(const char* iptr) {
	if (strchr(" \t\v\n\f", *iptr)) return 1;
	else return 0;
}

int dfa_RESERVED(const char* iptr) {
	int len = 0;
	if (!strncmp(iptr, "break", 5)) len = strlen("break");
	/*Print token*/
	return len;
}

int dfa_IDENTIFIER(const char* iptr) {
	int state = 1, len = 0, tlen = 0, run = 1;
	while (run) {
		switch (state) {
		case 1:
			if (tlen = dfa_L(iptr)) {
				len += tlen;
				state = 2;
			}
			else {
				len = 0;
				run = 0;
			}
			break;
		case 2:
			if (tlen = dfa_L(iptr)) {
				len += tlen;
				state = 2;
			}
			else if (tlen = dfa_D(iptr)) {
				len += tlen;
				state = 2;
			}
			else {
				run = 0;
			}
			break;
		}
		iptr++;
	}
	/*print token*/
	return len;
}

int dfa_HEXINT(const char* iptr) {
	int state = 1, len = 0, tlen = 0, run = 1;
	while (run) {
		switch (state) {
		case 1:
			if (tlen = dfa_HP(iptr)) {
				len += tlen;
				state = 2;
			}
			else {
				len = 0;
				run = 0;
			}
			break;
		case 2:
			if (tlen = dfa_H(iptr)) {
				len += tlen;
				state = 3;
			}
			else {
				len = 0;
				run = 0;
			}
			break;
		case 3:
			if (tlen = dfa_H(iptr)) {
				len += tlen;
				state = 3;
			}
			else if (tlen = dfa_IS(iptr)) {
				len += tlen;
				state = 4;
			}
			else {
				run = 0;
			}
			break;
		case 4:
			run = 0;
			break;
		}
		iptr++;
	}
	/*print token*/
	return len;
}

int dfa_DECINT(const char* iptr) {
	int state = 1, len = 0, tlen = 0, run = 1;
	while (run) {
		switch (state) {
		case 1:
			if (tlen = dfa_NZ(iptr)) {
				len += tlen;
				state = 2;
			}
			else {
				len = 0;
				run = 0;
			}
			break;
		case 2:
			if (tlen = dfa_D(iptr)) {
				len += tlen;
				state = 2;
			}
			else if (tlen = dfa_IS(iptr)) {
				len += tlen;
				state = 3;
			}
			else {
				run = 0;
			}
			break;
		case 3:
			run = 0;
			break;
		}
		iptr++;
	}
	/*print token*/
	return len;
}

int dfa_OCTINT(const char* iptr) {
	int state = 1, len = 0, tlen = 0, run = 1;
	while (run) {
		switch (state) {
		case 1:
			if (tlen = (strncmp(iptr, "0", 1) ? 0 : strlen("0"))) {
				len += tlen;
				state = 2;
			}
			else {
				len = 0;
				run = 0;
			}
			break;
		case 2:
			if (tlen = dfa_O(iptr)) {
				len += tlen;
				state = 2;
			}
			else if (tlen = dfa_IS(iptr)) {
				len += tlen;
				state = 3;
			}
			else {
				run = 0;
			}
			break;
		case 3:
			run = 0;
			break;
		}
		iptr++;
	}
	/*print token*/
	return len;
}

int dfa_CHRINT(const char* iptr) {
	int state = 1, len = 0, tlen = 0, run = 1;
	while (run) {
		switch (state) {
		case 1:
			if (tlen = dfa_CP(iptr)) {
				len += tlen;
				state = 2;
			}
			else if (tlen = (strncmp(iptr, "'", 1) ? 0 : strlen("'"))) {
				len += tlen;
				state = 3;
			}
			else {
				len = 0;
				run = 0;
			}
			break;
		case 2:
			if (tlen = (strncmp(iptr, "'", 1) ? 0 : strlen("'"))) {
				len += tlen;
				state = 3;
			}
			else {
				len = 0;
				run = 0;
			}
			break;
		case 3:
			if (tlen = dfa_ES(iptr)) {
				len += tlen;
				state = 4;
			}
			else if (tlen = !strchr("'\\\n", *iptr)) {
				len += tlen;
				state = 4;
			}
			else {
				len = 0;
				run = 0;
			}
			break;
		case 4:
			if (tlen = dfa_ES(iptr)) {
				len += tlen;
				state = 4;
			}
			else if (tlen = !strchr("'\\\n", *iptr)) {
				len += tlen;
				state = 4;
			}
			else if(tlen = (strncmp(iptr, "'", 1) ? 0 : strlen("'"))) {
				len += tlen;
				state = 5;
			}
			else {
				len = 0;
				run = 0;
			}
			break;
		case 5:
			run = 0;
			break;
		}

		iptr++;
	}
	/*print token*/
	return len;
}

int dfa_IEXPFLOAT(const char* iptr) {
	int state = 1, len = 0, tlen = 0, run = 1;
	while (run) {
		switch (state) {
		case 1:
			if (tlen = dfa_D(iptr)) {
				len += tlen;
				state = 2;
			}
			else {
				len = 0;
				run = 0;
			}
			break;
		case 2:
			if (tlen = dfa_D(iptr)) {
				len += tlen;
				state = 2;
			}
			else if (tlen = dfa_E(iptr)) {
				len += tlen;
				state = 3;
			}
			else {
				len = 0;
				run = 0;
			}
			break;
		case 3:
			if (tlen = dfa_FS(iptr)) {
				len += tlen;
				state = 4;
			}
			else {
				run = 0;
			}
			break;
		case 4:
			run = 0;
			break;
		}
		iptr++;
	}
	/*print token*/
	return len;
}

int analyze() {
	char buf[BUF_SIZE];
	FILE* in_file = NULL;
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
	return 0;
}

char yy_text[YY_SIZE];

int run(char* is) {
	int state = 0, ptr = 0;
	while (*is) {
		switch (state) {
		case 0:
			//End of input
			if (*is == '\0') return 0;
			//Exception
			else return 1;
		case 1:
			switch (*is) {
			case '.':
				state = 2;
				yy_text[ptr++] = *is;
				break; 
			case 'b':
				
			default:
				//Exception
				return 1;
			}
			break;
		case 25:
			switch (*is) {
			case '.':
				state = 2;
				yy_text[ptr++] = *is;
				break;
			default:
				token_25();
				state = 0;
				memset(yy_text, '\0', YY_SIZE);
				ptr = 0;
				is--;
			}

			break;
		}
		if (!*is) break;
		is++;
	}
	return 1;
}