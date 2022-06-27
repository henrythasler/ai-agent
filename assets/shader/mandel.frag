#version 330 core

uniform float time;
out vec4 fragColor;

int fmandel(vec2 uv)
{
    float zoom = 200000;
    float f_cx = -0.746076465, f_cy = -0.1245476771, f_sx = 1.0/zoom, f_sy=1.0/zoom, f_z = 1.0/zoom;
    int iterations = 400;
    float radius = 4.0;

  vec2 c = vec2(f_cx, f_cy) + uv.xy*f_z + vec2(f_sx,f_sy);
  vec2 z=c;

  for(int n=0; n<iterations; n++)
	{
	z = vec2(z.x*z.x - z.y*z.y, 2.0*z.x*z.y) + c;
	if((z.x*z.x + z.y*z.y) > radius) return n;
	}
  return 0;
}

void main()
{
    vec2 uv = gl_FragCoord.xy;
    int n = fmandel(uv); 
    if(n==0) {
      fragColor = vec4(0);
    }
    else {
    fragColor = vec4( (-cos(0.025*float(n) + time)+1.0)/2.0, 
					  (-cos(0.08*float(n) + time)+1.0)/2.0, 
					  (-cos(0.12*float(n) + time)+1.0)/2.0, 
				       1.0);    
    }
}
