
/* -------------------------------------------------------

This shader implements a spotlight per pixel using  the 
diffuse, specular, and ambient terms acoording to "Mathematics of Lighthing" 
as found in the book "OpenGL Programming Guide" (aka the Red Book)

Antonio Ramires Fernandes

--------------------------------------------------------- */

varying vec4 diffuse,ambientGlobal, ambient;
varying vec3 normal,lightDir,halfVector;
varying float dist;

uniform sampler2D color_texture;			//added to spotlight


void main()
{
	
	vec3 n,halfV;
	float NdotL,NdotHV;
	vec4 color = ambient; // texture2D( color_texture, gl_TexCoord[0].st); //ambientGlobal;
	float att,spotEffect;
	int get_light=0;

	/* a fragment shader can't write a verying variable, hence we need
	a new variable to store the normalized interpolated normal */
	n = normalize(normal);
	
	/* compute the dot product between normal and ldir */
	NdotL = max(dot(n,normalize(lightDir)),0.0);


	if (NdotL > 0.0) 
	{
		spotEffect = dot(normalize(gl_LightSource[0].spotDirection), normalize(-lightDir));
		
		float threshold = cos(gl_LightSource[0].spotCosCutoff); //spotCosCutoff is in radian

		if (spotEffect >threshold ) 
		{
			spotEffect = pow(spotEffect, gl_LightSource[0].spotExponent);
			att = spotEffect / (gl_LightSource[0].constantAttenuation +
			                    gl_LightSource[0].linearAttenuation * dist +
		                        gl_LightSource[0].quadraticAttenuation * dist * dist);


			color += att * (diffuse * NdotL);

			halfV = normalize(halfVector);
			NdotHV = max(dot(n,halfV),0.0);
			color += att * gl_FrontMaterial.specular * gl_LightSource[0].specular * pow(NdotHV,gl_FrontMaterial.shininess);
			get_light=1;
			color=color*texture2D( color_texture, gl_TexCoord[0].st);
		}
	}

	if(get_light==0)
	{		
		att = 0.1 / (gl_LightSource[0].constantAttenuation +
			         gl_LightSource[0].linearAttenuation * dist +
		             gl_LightSource[0].quadraticAttenuation * dist * dist);		
		color = att * ambient;
	}

	//gl_FragColor = texture2D( color_texture, gl_TexCoord[0].st)*color;
	gl_FragColor = color*1.15;		//ups intensity a bit
	
}
