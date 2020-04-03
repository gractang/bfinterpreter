#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "time.h"
#include "unistd.h"

#define MAX_BAG_SIZE 256
#define LINE_LENGTH 50000

#define ADD 43
#define SUBTRACT 45
#define LEFT 60
#define RIGHT 62
#define OPEN 40
#define CLOSE 41
#define SHOUT 42
#define VALIDS "+-<>()*"

#define FLAG "-d"

#define OPEN_S "while (line[p]) {\n"
#define CLOSE_S "}\n"
#define SHOUT_S "putchar(line[p]);\n"
#define CLEAR_S "line[p] = 0;\n"

#define WRITE_FILE "bfcode.c"

//checks if the token is valid
bool is_valid(char c) {
	for (int i = 0; i < strlen(VALIDS); i++) {
		if (VALIDS[i] == c) return true;
	}
	return false;
}

//parses the bf file
char* parse(char* file_name) {
	FILE* fp = fopen(file_name, "r");
	if (fp == NULL) {
		printf("%s\n", "aiyah");
		exit(1);
	}
	int c;
	char* code;

	fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
 	code = malloc(file_size);
 	int n = 0;

 	int num_open = 0;
 	int num_close = 0;

	while ((c = fgetc(fp)) != EOF) {
		if (is_valid((char)(c))) {
			if (c == OPEN) num_open++;
			else if (c == CLOSE) num_close++;

			if (num_close > num_open) {
				printf("%s\n", "invalid program");
				exit(1);
			}
			code[n] = (char)(c);
			n++;
		}
	}
	
	if (num_open != num_close) {
		printf("%s\n", "invalid program");
		exit(1);
	}
	fclose(fp);
	return code;
}

//outputs code to add to the bag, then returns the end index
int add(char* code, int i, FILE* fp, char current_c) {
	int num = 0;
	while (current_c == ADD) {
		num++;
		current_c = code[i++];
	}
	fputs("line[p] += ", fp);
	char str[5];
	sprintf(str, "%d", num-1);
	fputs(str, fp);
	fputs(";\n", fp);
	return i-2;
}

//outputs code to subtract from the bag, then returns the end index
int subtract(char* code, int i, FILE* fp, char current_c) {
	int num = 0;
	while (current_c == SUBTRACT) {
		num++;
		current_c = code[i++];
	}
	fputs("line[p] -= ", fp);
	char str[5];
	sprintf(str, "%d", num-1);
	fputs(str, fp);
	fputs(";\n", fp);
	return i-2;
}

//outputs code to move the pointer left, then returns the end index
int left(char* code, int i, FILE* fp, char current_c) {
	int num = 0;
	while (current_c == LEFT) {
		num++;
		current_c = code[i++];
	}
	fputs("p -= ", fp);
	char str[5];
	sprintf(str, "%d", num-1);
	fputs(str, fp);
	fputs(";\n", fp);
	fputs("if (p < 0) {printf(\"%s\", \"left oopsie -- undesirable wrapping\\n\"); exit(1);}\n", fp);
	return i-2;
}

//outputs code to move the pointer right, then returns the end index
int right(char* code, int i, FILE* fp, char current_c) {
	int num = 0;
	while (current_c == RIGHT) {
		num++;
		current_c = code[i++];
	}
	fputs("p += ", fp);
	char str[5];
	sprintf(str, "%d", num-1);
	fputs(str, fp);
	fputs(";\n", fp);
	fputs("if (p >= LINE_LENGTH) {printf(\"%s\", \"right oopsie -- undesirable wrapping\\n\"); exit(1);} \n", fp);
	return i-2;
}

//calls the other methods to convert the bf file to a c file
void to_C(char* code, char* file_name) {
	FILE* fp = fopen(file_name, "w+");
	if (fp == NULL) {
		printf("%s\n", "aiyah");
		exit(1);
	}

	//formatting stuff
	fputs("#include \"stdio.h\"\n#include \"stdint.h\"\n#include \"stdlib.h\"\n#include \"time.h\"\n", fp);
	fputs("#define LINE_LENGTH 50000\n\n", fp);
	fputs("int main(int argc, char** argv) {\n", fp);
	fputs("clock_t begin = clock();\n", fp);
	fputs("char line[LINE_LENGTH];\n", fp);
	fputs("int p = 0;\n", fp);

	int len = strlen(code);

	for (int i = 0; i < len; i++) {
		char current_c = code[i];
		switch (current_c) {
			case ADD: i = add(code, i, fp, current_c);
				break;
			case SUBTRACT: i = subtract(code, i, fp, current_c);
				break;
			case LEFT: i = left(code, i, fp, current_c); 
				break;
			case RIGHT: i = right(code, i, fp, current_c);
				break;
			case OPEN: 
				if (code[i+1] == SUBTRACT && code[i+2] == CLOSE) {
					fputs(CLEAR_S, fp);
					i += 2;
				}
				else 
					fputs(OPEN_S, fp); 
				break;
			case CLOSE: fputs(CLOSE_S, fp); 
				break;
			case SHOUT: fputs(SHOUT_S, fp); 
				break;
			default: break;
		}
	}
	fputs("clock_t end = clock();\n", fp);
	fputs("double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;\n", fp);
	fputs("printf(\"time spent: %f\", time_spent);\n", fp);
	fputs("\nreturn 0;\n}", fp);

	fclose(fp);
}

int main(int argc, char** argv) {
	//start clock to keep track of time
	clock_t begin = clock();

	char* file_name = argv[1];

	int line[LINE_LENGTH];

	char* bflol = parse(file_name);

	to_C(bflol, WRITE_FILE);

	free(bflol);

	//find total time spent (in secs)
	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("time spent: %f\ns", time_spent);

	//compile new code
	system("gcc bfcode.c -o bfcode");
	// system("bfcode"); //for windows

	char *idfk[]={"./bfcode",NULL}; 
    execv(idfk[0],idfk);
	//system("clang bfcode.c -o bfcode && ./bfcode"); //for mac

	return 0;
}