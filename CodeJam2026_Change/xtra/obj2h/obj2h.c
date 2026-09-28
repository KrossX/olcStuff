#include <stdio.h>
#include <string.h>

char buffer[1024];

struct pos { float x, y, z; };
struct uv { float u, v; };
struct face { struct pos pos[3]; struct uv uv[3]; };

struct pos  vbuff[1024];
struct uv   vtbuff[1024];
struct face fbuff[1024];

int v_count;
int vt_count;
int f_count;

char filename_out[1024];

int main(int argc, char *argv[])
{
	char *strptr;
	int i;
	FILE *file_in, *file_out;
	
	if(argc < 2) {
		printf("Usage: obj2h input.obj\n\n");
		return 0;
	}

	strptr = strrchr(argv[1], '.');         strptr[0] = '_';
	sprintf(filename_out, "%s.h", argv[1]); strptr[0] = '.';
	
	file_in = fopen(argv[1], "r");
	file_out = fopen(filename_out, "w");
	
	if(file_in == NULL || file_out == NULL) {
		printf("File error %s %s\n", file_in? "" : "INPUT", file_out? "" : "OUTPUT");
		return 0;
	}
	
	printf("%s --> %s\n", argv[1], filename_out);
	
	
	while(1) {
		char *ptr = fgets(buffer, 1024, file_in);
		if(!ptr) break;
		
		switch(buffer[0]) {
		case 'f': {
			int idx[6];
			sscanf(buffer, "f %d/%d %d/%d %d/%d", &idx[0], &idx[1], &idx[2], &idx[3], &idx[4], &idx[5]);

			for(i = 0; i < 6; i++) idx[i]--;
			fbuff[f_count].pos[0] = vbuff[idx[0]]; fbuff[f_count].uv[0] = vtbuff[idx[1]];
			fbuff[f_count].pos[1] = vbuff[idx[2]]; fbuff[f_count].uv[1] = vtbuff[idx[3]];
			fbuff[f_count].pos[2] = vbuff[idx[4]]; fbuff[f_count].uv[2] = vtbuff[idx[5]];
			f_count++;
			} break;

		case 'v': 
			if(buffer[1] == 't') {
				sscanf(buffer, "vt %f %f", &vtbuff[vt_count].u, &vtbuff[vt_count].v);
				vt_count++;
			} else {
				sscanf(buffer, "v %f %f %f", &vbuff[v_count].x, &vbuff[v_count].y, &vbuff[v_count].z);
				v_count++;
			} break;

		default: break;
		}
	}
	
	fclose(file_in);

	strptr[0] = '_';
	strptr = strrchr(argv[1], '\\');
	strptr = strptr ? strptr + 1 : argv[1];
	fprintf(file_out, "int %s_total =  %d;\n", strptr, f_count * 15);
	fprintf(file_out, "float %s[%d] = {\n", strptr, f_count * 15);
	
	for(i = 0; i < f_count; i++) {
		fprintf(file_out, "\t%ff,%ff,%ff,%ff,%ff,\n", fbuff[i].pos[0].x, fbuff[i].pos[0].y, fbuff[i].pos[0].z, fbuff[i].uv[0].u, fbuff[i].uv[0].v);
		fprintf(file_out, "\t%ff,%ff,%ff,%ff,%ff,\n", fbuff[i].pos[1].x, fbuff[i].pos[1].y, fbuff[i].pos[1].z, fbuff[i].uv[1].u, fbuff[i].uv[1].v);
		fprintf(file_out, "\t%ff,%ff,%ff,%ff,%ff", fbuff[i].pos[2].x, fbuff[i].pos[2].y, fbuff[i].pos[2].z, fbuff[i].uv[2].u, fbuff[i].uv[2].v);
		if(i < (f_count-1))	fprintf(file_out, ",\n");
	}

	fprintf(file_out, "\n};\n\n");
	fclose(file_out);
	
	printf("v: %d, vt: %d, f: %d\n", v_count, vt_count, f_count);
	
	return 0;
}