shader_type canvas_item;

void fragment() {
	vec4 c = texture(TEXTURE, UV);
	
	// Linear -> sRGB
	vec3 a = vec3(0.055);
	c.rgb = mix((vec3(1.0) + a) * pow(c.rgb, vec3(1.0 / 2.4)) - a, 12.92 * c.rgb, lessThan(c.rgb, vec3(0.0031308)));
	
	COLOR = c;
}