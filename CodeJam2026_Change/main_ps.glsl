R"(
uniform vec2 bg_offset;
uniform vec2 bg_region;

vec4 col_blue  = vec4(0,0,1,1);
vec4 col_green = vec4(0,1,0,1);
vec4 col_brown = vec4(0.5,0.25,0,1);
vec4 col_white = vec4(1,1,1,1);


void main()
{
	vec2 pos = oTex * bg_region / 20.0 + bg_offset;
	
	float factor = texture(pgeTexture0, pos).r;
	pixel = factor < 0.33? col_blue : factor < 0.66? col_green : factor < 0.85? col_brown : col_white;
}
)";