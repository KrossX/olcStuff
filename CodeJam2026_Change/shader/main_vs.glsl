out float oFog;

void main()
{
	if (pgeDrawType == 2) // 3D																																  
	{
		gl_Position = pgeMVP * vec4(aPos.x, aPos.y, aPos.z, 1.0);
		oTex = aTex;
	}

	else if (pgeDrawType == 1) // 2D Line																																		  
	{
		float p = 1.0 / aPos.z;
		gl_Position = p * vec4(vec2(2.0 * (floor(aPos.xy) + 0.5) * pgeInverseTargetSizeInPixels - 1.0), 0.0, 1.0);
		oTex = aTex;
	}

	else if (pgeDrawType == 3) // Unconstrained 2D Line																																		  
	{
		float p = 1.0 / aPos.z;
		gl_Position = p * vec4(vec2(2.0 * (aPos.xy) * pgeInverseTargetSizeInPixels - 1.0), 0.0, 1.0);
		oTex = aTex;
	}

	else if (pgeDrawType == 4) // Unconstrained 2D Polygon																																		  
	{
		float p = 1.0 / aPos.z;
		gl_Position = p * vec4(vec2(2.0 * (aPos.xy) * pgeInverseTargetSizeInPixels - 1.0), 0.0, 1.0);
		oTex = p * vec2(aTex.x, aTex.y);
	}

	else if (pgeDrawType == 0) // 2D Polygon																																		  
	{
		float p = 1.0 / aPos.z; 
		gl_Position = p * vec4(vec2(2.0 * (aPos.xy + 0.25) * pgeInverseTargetSizeInPixels - 1.0), 0.0, 1.0);
		oTex = p * vec2(aTex.x, aTex.y);
	}

	else  // Balanced default
	{
		gl_Position = aPos;
		oTex = aTex;
	}

	oFog = gl_Position.z;
	oCol = aCol * pgeGlobalTint;
}
