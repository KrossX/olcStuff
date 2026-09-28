uniform vec2 bg_offset;
uniform vec2 bg_region;

vec4 col_blue0  = texture(pgeTexture1, vec2(37,16)/64.0);
vec4 col_blue1  = texture(pgeTexture1, vec2(37,46)/64.0);
vec4 col_blue2  = texture(pgeTexture1, vec2(45,46)/64.0);
vec4 col_green0 = texture(pgeTexture1, vec2(26,16)/64.0);
vec4 col_green1 = texture(pgeTexture1, vec2(26,46)/64.0);
vec4 col_brown0 = texture(pgeTexture1, vec2(13,16)/64.0);
vec4 col_brown1 = texture(pgeTexture1, vec2(13,46)/64.0);
vec4 col_white  = texture(pgeTexture1, vec2(49,46)/64.0);


void main()
{
	vec2 pos = oTex;
	pos.x -= 0.5;
	pos /= 1.0 - pos.y * 2.0 - 0.5;
	pos.x += 0.5;

	if(oTex.y < 0.25) {
		float factor = oTex.y * 4;
		pixel = mix(col_blue2, col_white, factor * factor);
	} else {
		float factor = texture(pgeTexture0, pos * bg_region / 40.0 + bg_offset).r;
		pixel = factor < 0.33? mix(col_blue0,  col_blue1,  factor * 3.0) : 
				factor < 0.66? mix(col_green1, col_green0, (factor - 0.66) * 3.0) : 
				factor < 0.85? mix(col_brown0, col_brown1, (factor - 0.85) * 6.67) : col_white;

		factor = 1 - (oTex.y - 0.25) * 4.0/3.0;
		pixel = mix(pixel, col_white, factor * factor);
	}
}
