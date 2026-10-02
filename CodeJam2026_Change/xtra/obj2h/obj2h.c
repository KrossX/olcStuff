#include <stdio.h>
#include <string.h>

char buffer[1024];

struct pos { float x, y, z; };
struct uv { float u, v; };
struct face { struct pos pos[3]; struct uv uv[3]; };

struct pos  vbuff[1024];
struct uv   vtbuff[1024];

struct obj_info {
	struct face fbuff[1024];

	char name[64];

	int v_count;
	int vt_count;
	int f_count;	
};

int v_total, vt_total, f_total;
struct obj_info obj[16];
struct obj_info *obj_cur = obj;
int obj_count;

void obj_pos_append(struct obj_info *o, float x, float y, float z)
{
	struct pos *p = &vbuff[v_total++];

	o->v_count++;
	
	p->x = x;
	p->y = y;
	p->z = z;
}

void obj_uv_append(struct obj_info *o, float u, float v)
{
	struct uv *uv = &vtbuff[vt_total++];
	
	o->vt_count++;
	
	uv->u = u;
	uv->v = v;
}

void obj_face_append(struct obj_info *o, int *idx)
{
	struct face *f = &o->fbuff[o->f_count++];
	
	f_total++;
	
	f->pos[0] = vbuff[idx[0]-1];
	f->uv[0] = vtbuff[idx[1]-1];
	f->pos[1] = vbuff[idx[2]-1];
	f->uv[1] = vtbuff[idx[3]-1];
	f->pos[2] = vbuff[idx[4]-1];
	f->uv[2] = vtbuff[idx[5]-1];
}

char filename_out[1024];

int main(int argc, char *argv[])
{
	char *strptr;
	int i, j;
	FILE *file_in, *file_out;
	
	if(argc < 2) {
		printf("Usage: obj2h input.obj\n\n");
		return 0;
	}
	
	if(argc < 3) {
		strptr = strrchr(argv[1], '.');         strptr[0] = '_';
		sprintf(filename_out, "%s.h", argv[1]); strptr[0] = '.';
	} else {
		strcpy(filename_out, argv[2]);
	}

	file_in = fopen(argv[1], "r");
	file_out = fopen(filename_out, "w");
	
	if(file_in == NULL || file_out == NULL) {
		printf("File error %s %s\n", file_in? "" : "INPUT", file_out? "" : "OUTPUT");
		return 0;
	}
	
	while(1) {
		char *ptr = fgets(buffer, 1024, file_in);
		if(!ptr) break;
		
		switch(buffer[0]) {
		case 'o': {
			obj_cur =  &obj[obj_count++];
			sscanf(buffer, "o %s", obj_cur->name);
			} break;
			
		case 'f': {
			int idx[6];
			sscanf(buffer, "f %d/%d %d/%d %d/%d", &idx[0], &idx[1], &idx[2], &idx[3], &idx[4], &idx[5]);
			obj_face_append(obj_cur, idx);
			} break;

		case 'v': 
			if(buffer[1] == 't') {
				float u, v;
				sscanf(buffer, "vt %f %f", &u, &v);
				obj_uv_append(obj_cur, u, v);
			} else {
				float x, y, z;
				sscanf(buffer, "v %f %f %f", &x, &y, &z);
				obj_pos_append(obj_cur, x, y, z);
			} break;

		default: break;
		}
	}
	
	fclose(file_in);
	
	fprintf(file_out, "namespace obj\n{\n");
	for(i = 0; i < obj_count; i++) {
		struct obj_info *o = &obj[i];
		fprintf(file_out, "\tint %s_cnt =  %d;\n", o->name, o->f_count * 15);
		fprintf(file_out, "\tfloat %s[%d] = {\n", o->name, o->f_count * 15);

		for(j = 0; j < o->f_count;) {
			struct face *f = &o->fbuff[j++];
			
			fprintf(file_out, "\t\t%9.6ff,%9.6ff,%9.6ff,%9.6ff,%9.6ff,\n", f->pos[0].x, f->pos[0].y, f->pos[0].z, f->uv[0].u, f->uv[0].v);
			fprintf(file_out, "\t\t%9.6ff,%9.6ff,%9.6ff,%9.6ff,%9.6ff,\n", f->pos[1].x, f->pos[1].y, f->pos[1].z, f->uv[1].u, f->uv[1].v);
			fprintf(file_out, "\t\t%9.6ff,%9.6ff,%9.6ff,%9.6ff,%9.6ff",    f->pos[2].x, f->pos[2].y, f->pos[2].z, f->uv[2].u, f->uv[2].v);
			fprintf(file_out, j == o->f_count ? "};\n\n" : ",\n");
		}
		
		printf("%s = v: %d, vt: %d, f: %d\n", o->name, o->v_count, o->vt_count, o->f_count);
	}

	fprintf(file_out, "};\n");
	fclose(file_out);
	
	printf("TOTAL = v: %d, vt: %d, f: %d\n", v_total, vt_total, f_total);
	
	return 0;
}