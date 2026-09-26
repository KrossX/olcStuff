R"(
uniform vec2 bg_offset;
uniform vec2 bg_region;

vec4 col_blue  = vec4(0,0,1,1);
vec4 col_green = vec4(0,1,0,1);
vec4 col_brown = vec4(0.5,0.25,0,1);
vec4 col_white = vec4(1,1,1,1);


void main()
{
	vec2 pos = oTex;
	pos.x -= 0.5;
	pos /= 1.0 - pos.y * 2.0 - 0.5;
	pos.x += 0.5;

	if(oTex.y < 0.25) {
		float factor = oTex.y * 4;
		pixel = mix(col_blue + 0.5, col_white, factor * factor);
	} else {
		float factor = texture(pgeTexture0, pos * bg_region / 40.0 + bg_offset).r;
		pixel = factor < 0.33? col_blue : factor < 0.66? col_green : factor < 0.85? col_brown : col_white;

		factor = 1 - (oTex.y - 0.25) * 4.0/3.0;
		pixel = mix(pixel, col_white, factor * factor);
	}
}
)";
