var canvas;
var gl;
var program;
var v_Loc_Position;
var uv_i_Loc_Position;
var UV_Loc_Position;
var uni_u_coords = [ 0.32, 0.5, 0.96 ];
var uni_v_coords = [ 0.05, 0.1, 0.86 ];
var uni_u_Loc;
var uni_v_Loc;

var vertex_coords = [
	vec2( -0.8, -0.8 ),
	vec2( -0.8,  0.8 ),
	vec2(  0.8,  0.8 ),
];
var UV_coords = [
	vec2(  0.0,  0.0 ),
	vec2(  0.5,  0.0 ),
	vec2(  1.0,  1.0 ),
];
var uv_index = [ 0.0, 1.0, 2.0 ];
var index_array = [0, 1, 2];

window.onload = function init(){
//--------------------------------------------------------------------
	canvas = document.getElementById( "gl-canvas" );
	gl = WebGLUtils.setupWebGL( canvas );
	if ( !gl ) { alert( "WebGL isn't available" ); }
	gl.viewport( 0, 0, canvas.width, canvas.height );
	gl.clearColor( 0.5, 0.5, 0.5, 1.0 );
	program = initShaders( gl, "vertex-shader", "fragment-shader" );
	gl.useProgram( program );
	gl.enable( gl.DEPTH_TEST );
	gl.blendFunc( gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA );
	gl.enable( gl.BLEND );
//--------------------------------------------------------------------
	
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	var UINT_surface = new Uint8Array( index_array );
	var i_buffer		= gl.createBuffer();
	var v_buffer		= gl.createBuffer();
	var uv_i_buffer	= gl.createBuffer();
	var UV_buffer		= gl.createBuffer();
	
	gl.bindBuffer( gl.ELEMENT_ARRAY_BUFFER, i_buffer );
	gl.bufferData( gl.ELEMENT_ARRAY_BUFFER, UINT_surface, gl.STATIC_DRAW );
	
	gl.bindBuffer( gl.ARRAY_BUFFER, v_buffer );
	gl.bufferData( gl.ARRAY_BUFFER, flatten( vertex_coords ), gl.STATIC_DRAW );
	v_Loc_Position = gl.getAttribLocation( program, "vertex_coords" );
	gl.vertexAttribPointer( v_Loc_Position, 2, gl.FLOAT, false, 0, 0 );
	gl.enableVertexAttribArray( v_Loc_Position );
	
	gl.bindBuffer( gl.ARRAY_BUFFER, uv_i_buffer );
	gl.bufferData( gl.ARRAY_BUFFER, flatten( uv_index ), gl.STATIC_DRAW );
	uv_i_Loc_Position = gl.getAttribLocation( program, "uv_index" );
	gl.vertexAttribPointer( uv_i_Loc_Position, 1, gl.FLOAT, false, 0, 0 );
	gl.enableVertexAttribArray( uv_i_Loc_Position );
	
	gl.bindBuffer( gl.ARRAY_BUFFER, UV_buffer );
	gl.bufferData( gl.ARRAY_BUFFER, flatten( UV_coords ), gl.STATIC_DRAW );
	UV_Loc_Position = gl.getAttribLocation( program, "UV_coords" );
	gl.vertexAttribPointer( UV_Loc_Position, 2, gl.FLOAT, false, 0, 0 );
	gl.enableVertexAttribArray( UV_Loc_Position );
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	
//====================================================================
	document.getElementById( "p0_u" ).onchange = function( event ){
		uni_u_coords[ 0 ] = document.getElementById( "p0_u" ).value;
		document.getElementById( "p0_u_output" ).innerHTML = uni_u_coords[ 0 ];
	};
	document.getElementById( "p0_v" ).onchange = function( event ){
		uni_v_coords[ 0 ] = document.getElementById( "p0_v" ).value;
		document.getElementById( "p0_v_output" ).innerHTML = uni_v_coords[ 0 ];
	};
	document.getElementById( "p1_u" ).onchange = function( event ){
		uni_u_coords[ 1 ] = document.getElementById( "p1_u" ).value;
		document.getElementById( "p1_u_output" ).innerHTML = uni_u_coords[ 1 ];
	};
	document.getElementById( "p1_v" ).onchange = function( event ){
		uni_v_coords[ 1 ] = document.getElementById( "p1_v" ).value;
		document.getElementById( "p1_v_output" ).innerHTML = uni_v_coords[ 1 ];
	};
	document.getElementById( "p2_u" ).onchange = function( event ){
		uni_u_coords[ 2 ] = document.getElementById( "p2_u" ).value;
		document.getElementById( "p2_u_output" ).innerHTML = uni_u_coords[ 2 ];
	};
	document.getElementById( "p2_v" ).onchange = function( event ){
		uni_v_coords[ 2 ] = document.getElementById( "p2_v" ).value;
		document.getElementById( "p2_v_output" ).innerHTML = uni_v_coords[ 2 ];
	};
//====================================================================
	
	render();
};

function render(){
	gl.clear( gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT );
	uni_u_Loc = gl.getUniformLocation( program, "u_coords" );
	uni_v_Loc = gl.getUniformLocation( program, "v_coords" );
	gl.uniform3fv( uni_u_Loc, flatten( uni_u_coords ) );
	gl.uniform3fv( uni_v_Loc, flatten( uni_v_coords ) );
	
	gl.drawElements( gl.TRIANGLES, 3, gl.UNSIGNED_BYTE, 0 );
	requestAnimFrame( render );
}
