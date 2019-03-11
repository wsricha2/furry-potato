attribute	vec2	v_position;
void main(void){
	gl_Position = vec4( v_position.x, v_position.y, 0.0, 1.0 );
}
