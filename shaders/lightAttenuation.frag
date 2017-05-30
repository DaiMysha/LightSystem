uniform vec2 center;
uniform float radius;
uniform vec4 color;
uniform float bleed;
uniform float linearFactor;
uniform float intensity;

void main() {
	vec2 pixel = gl_FragCoord.xy;
	
	float dist = length(center - pixel);
	
	float distFromFalloff = radius - dist;
	
	float attenuation = 0.0;
	attenuation = distFromFalloff * (bleed / (dist*dist) + linearFactor / radius);
			
	attenuation = clamp(attenuation, 0.0, 1.0);
	
	vec4 color = vec4(1, 1, 1, attenuation * intensity);

	gl_FragColor = color;
}