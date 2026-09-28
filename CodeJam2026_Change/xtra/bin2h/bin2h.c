#include <windows.h>
#include <stdio.h>

char line_buffer[1024];
char filename_out[MAX_PATH], filename_in[MAX_PATH];
char *file_buffer;

int str_length(char *str) {
	int i = 0;
	while(*str++) { i++; }
	return i;
}

void str_copy(char *dst, char *src) {
	while(*dst++ = *src++) {}
}

int find_last_of(char *str, char c) {
	int i, j = -1;
	for(i = 0; str[i]; i++) {
		if(str[i] == c) j = i;
	}
	return j;
}

char* get_filename(char *str) {

	int i = find_last_of(str, '\\');
	return &str[i < 0? 0 : i+1];
}

int main(int argc, char *argv[])
{
	HANDLE file_in, file_out;
	DWORD file_size, bytescount, i = 0;
	
	if(argc < 2) {
		printf("Usage: bin2h input [output]\n");
		printf("\t\toutput is optional.\n\n");
		return 0;
	}
	
	str_copy(filename_in, argv[1]);
	CharLower(filename_in);
	file_in = CreateFile(filename_in, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	
	if(file_in == INVALID_HANDLE_VALUE) {
		printf("Error opening input file\n");
		return 0;
	}
	
	if(argc < 3) {
		int ext = find_last_of(filename_in, '.');
		str_copy(filename_out, filename_in);
		if(ext) {
			filename_out[ext+1] = 'h';
			filename_out[ext+2] = 0;
		} else {
			ext = str_length(filename_out);
			filename_out[ext+0] = '.';
			filename_out[ext+1] = 'h';
			filename_out[ext+2] = 0;
		}
	} else {
		str_copy(filename_out, argv[2]);
	}
		
	CharLower(filename_out);
	file_out = CreateFile(filename_out, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	
	if(file_out == INVALID_HANDLE_VALUE) {
		printf("Error opening output file\n");
		return 0;
	}
	
	file_size = GetFileSize(file_in, NULL);
	file_buffer = VirtualAlloc(NULL, file_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	
	if(!file_buffer) {
		printf("Could not allocate buffer.\n");
		return 0;
	}
	
	if(!ReadFile(file_in, file_buffer, file_size, &bytescount, NULL)) {
		printf("Error reading input file\n");
		return 0;
	}

	CloseHandle(file_in);
	
	i = find_last_of(filename_in, '.');
	filename_in[i] = '_';
	i = wsprintf(line_buffer, "unsigned char %s[] = {\n\t", get_filename(filename_in));
	WriteFile(file_out, line_buffer, i, &bytescount, NULL);
	
	for(i = 0; i < file_size;) {
		int cnt = wsprintf(line_buffer, "0x%02X, ", (BYTE)file_buffer[i++]);

		if((i&0xF) == 0) { line_buffer[5] = '\n'; line_buffer[6] = '\t'; cnt++; } 
		if(i == file_size) { line_buffer[4] = '\n'; cnt--; }
	
		WriteFile(file_out, line_buffer, cnt, &bytescount, NULL);
	}
	
	i = wsprintf(line_buffer, "};\n");
	WriteFile(file_out, line_buffer, i, &bytescount, NULL);

	CloseHandle(file_out);
	
	return 0;
}
