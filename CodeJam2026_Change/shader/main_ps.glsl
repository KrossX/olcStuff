uniform vec2 uv_offset;
uniform vec4 bg_white;

in float oFog;

void main()
{
	// We premultiply alpha here
	vec4 texColor = texture(pgeTexture0, oTex + uv_offset) * oCol;

	float nFog = (oFog - 100.0) / 200.0;
	nFog = smoothstep(0.0, 1.0, nFog);

	texColor.rgb = mix(texColor.rgb, bg_white.rgb, nFog);

	pixel = vec4(texColor.rgb * texColor.a, texColor.a);
}

