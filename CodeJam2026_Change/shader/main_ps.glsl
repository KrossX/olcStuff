in float oFog;

void main()
{
	// We premultiply alpha here
	vec4 texColor = texture(pgeTexture0, oTex) * oCol;
	pixel = vec4(texColor.rgb * texColor.a, texColor.a);
	
	//if(oFog < 0) pixel   = vec4(1,0,0,1);
	//if(oFog > 10) pixel  = vec4(0,1,0,1);
	//if(oFog > 100) pixel = vec4(0,0,1,1);
	//if(oFog > 200) pixel = vec4(1,0,1,1);
	
	float nFog = (oFog - 100.0) / 200.0;
	nFog = smoothstep(0, 1, 1.0 - nFog);
	
	pixel.a = nFog;
	pixel.rgb *= pixel.a;
}

