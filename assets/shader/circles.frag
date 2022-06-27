#version 330 core
uniform float iTime;
uniform vec2 iResolution;
out vec4 fragColor;

vec3 circle(vec3 bcol, vec3 col, in vec2 a, in vec2 b )
{
	float rr = 0.04;
	vec3 res = mix( bcol, col, 1.0 - smoothstep( rr-0.01, rr, length(a-b) ) );
	float f = smoothstep( rr-0.01, rr, length(a-b) ) - smoothstep( rr, rr+0.01, length(a-b) );
	return mix( res, vec3(0.0, 0.0, 0.0), f );  // border color
}

void main()
{
    vec2 uv = (gl_FragCoord.xy - iResolution.xy * 0.5) / min(iResolution.x, iResolution.y) * 2.0;    

    vec3 col = vec3(1.0, 1.0, 1.0);     // Background
    vec3 col2 = vec3(1.0, 0.8, 0.0);    // fill color

    col = circle( col, col2, uv, vec2(0.0, 0.0) );
    col = circle( col, col2, uv, vec2(1.0, 1.0) );
    col = circle( col, col2, uv, vec2(-1.0, -1.0) );

    fragColor = vec4( col, 1.0 );
}
