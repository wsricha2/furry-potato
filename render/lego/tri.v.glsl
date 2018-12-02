attribute	vec4	v_position;
attribute	vec4	v_normals;

varying		float	light_intensity;

uniform		vec3	theta;
uniform		mat4	model_view;
uniform		mat4	projection;

//diffuse_product :=	effectively vec4(1.0, 1.0, 1.0, 1.0);
//Light Position	:=	vec3( 2.0, 2.0, 2.0 );
void main(void){
	vec3 angles	= radians( theta );
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
	vec3	L				= normalize( vec3( 2.0, 2.0, -4.0 ) - pos );
	vec3	N				= normalize( ( model * v_normals ).xyz );
	float	Kd			= max( dot( L, N ), 0.0 );
	//vec4	diffuse	= Kd * vec4( 1.0, 1.0, 1.0, 1.0 );
	
	//Final Assignments
	light_intensity = Kd;
	gl_Position = projection * model * v_position;
}
