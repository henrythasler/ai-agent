#version 330 core
out vec4 fragColor;

uniform sampler2D image;

uniform int iFrame;
uniform float iTime;
uniform vec2 iResolution;

uniform vec2 iViewportCenter;
uniform float iZoom;

layout (std140) uniform Population
{
                            // base alignment   // aligned offset
    int popCount;           // 4                // 0
    vec3 genomeColor[1024]; // 16               // 4
                                                // 20
                                                // ...
    vec2 position[1024];    // 16               // 16388
                                                // 16404
                                                // ...
    // Total: 32772 Bytes
}; 

// based on https://www.shadertoy.com/view/wsByzt by nickcody
// coord      - pixel to test
// center     - center of circle
// radius     - radius of circle
// strokeWidth - thickness of circle line
// pixelWidth - blendiness
// backgroundColor - color to draw over
// strokeColor - circle line color
// fillColor - circle fill color
vec3 circle(vec2 coord, vec2 center, float radius, float strokeWidth, float pixelWidth, vec3 backgroundColor, vec3 strokeColor, vec3 fillColor) {
    float dist_to_center = sqrt(pow(coord.x - center.x, 2.) + pow(coord.y - center.y, 2.));
    float delta = dist_to_center-radius;

    float blend = smoothstep(0., pixelWidth, abs(delta) - strokeWidth);
    
    if (delta  < 0.) {
    	// inside  edge
        return mix(strokeColor, fillColor, blend);
    } else if (delta  > 0.) {
    	// outside edge

        // from https://www.shadertoy.com/view/ltBGzt
        // const float amount = 1000.0;// bigger number = more accurate / crisper falloff.
        // float blend = 1 - clamp(1.0 / (clamp(delta, 1.0/amount, 1.0)*amount), 0.,1.);

        return mix(strokeColor, backgroundColor, blend);
    } else {
        return strokeColor;
    }       
}

// https://www.shadertoy.com/view/XlGcRh
vec2 hashwithoutsine21(float p)
{
	vec3 p3 = fract(vec3(p,p,p) * vec3(.1031, .1030, .0973));
	p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.xx+p3.yz)*p3.zy);
}

void main()
{
    vec2 uv = ((gl_FragCoord.xy + iViewportCenter) - iResolution.xy * 0.5) / min(iResolution.x, iResolution.y) * iZoom;

    vec3 backgroundColor = vec3(1.0, 1.0, 1.0);
    vec3 strokeColor = vec3(0.0, 0.0, 0.0);
    vec3 fillColor = vec3(1.0, 0.8, 0.0);

    float blur = (2. * iZoom)/iResolution.y;
    float strokeWidth = 0.0001;
    float radius = 0.03;

    // vec3 col = backgroundColor;
    vec3 col = texture(image, uv).rgb;

    int nCircles = popCount;

    for(int i = 0; i < nCircles; i++)
    {
        vec2 position = vec2(cos(float(i)/10. + iTime/15.) * i / nCircles, sin(float(i)/10. + iTime/15.) * i / nCircles);

        fillColor = 0.5 + 0.5 * cos(i + vec3(0,2,4));
        // fillColor = genomeColor[i];
        col = circle(uv, position, radius, strokeWidth, blur, col, strokeColor, fillColor);
    }

    fragColor = vec4( col, 1.0 );
}
