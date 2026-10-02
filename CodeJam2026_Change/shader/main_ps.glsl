in float oFog;

void main()
{
	// We premultiply alpha here
	vec4 texColor = texture(pgeTexture0, oTex) * oCol;

	float nFog = (oFog - 100.0) / 200.0;
	nFog = smoothstep(0.0, 1.0, 1.0 - nFog);

	texColor.a *= nFog;

	pixel = vec4(texColor.rgb * texColor.a, texColor.a);
}

