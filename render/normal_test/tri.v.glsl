attribute	vec4	v_position;
attribute vec2	UV_coords;

varying		vec3	L;
varying		vec2	f_UV_coord;

uniform		vec3	light;
uniform		vec3	viewPos;
uniform		mat4	model_view;
uniform		mat4	projection;

void main(void){
	vec3 angles	= radians( vec3( 0.0, 0.0, 0.0 ) );
	vec3 c			= cos( angles );
	vec3 s			= sin( angles );
	
	mat4 rx	= mat4(
		 1.0,  0.0,  0.0,  0.0,
		 0.0,  c.x, -s.x,  0.0,
		 0.0,  s.x,  c.x,  0.0,
		 0.0,  0.0,  0.0,  1.0
	);
	mat4 ry	= mat4(
		 c.y,  0.0,  s.y,  0.0,
		 0.0,  1.0,  0.0,  0.0,
		-s.y,  0.0,  c.y,  0.0,
		 0.0,  0.0,  0.0,  1.0
	);
	mat4 rz	= mat4(
		 c.z, -s.z,  0.0,  0.0,
		 s.z,  c.z,  0.0,  0.0,
		 0.0,  0.0,  1.0,  0.0,
		 0.0,  0.0,  0.0,  1.0
	);
	mat4 model = model_view * rz * ry * rx;

	//Dumb Diffuse
	//TODO:
	//	- Make light and diffuse product none static
	//	- Learn and construct normal matrix
	vec3	pos			= -( model * v_position ).xyz;
	//vec3	N				= normalize( ( model * v_normals ).xyz );
	//float	Kd			= max( dot( L, N ), 0.0 );
	//vec4	diffuse	= Kd * vec4( 1.0, 1.0, 1.0, 1.0 );
	
	//Final Assignments
	L	= normalize( light - pos );
	//light_intensity = Kd;
	f_UV_coord = UV_coords;
	gl_Position = projection * model * v_position;
}
