// Spring 2019

#include "A3.hpp"
#include "scene_lua.hpp"
#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"

#include "GeometryNode.hpp"
#include "JointNode.hpp"
#include "Collider.hpp"
#include "BuildingManager.hpp"
#include "ParticleSystem.hpp"
#include "Orb.hpp"
#include "Block.hpp"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;
using namespace glm;

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;

//----------------------------------------------------------------------------------------
// Constructor
A3::A3(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  vao_skybox( 0 ),
	  vbo_skybox( 0 )
{
	srand( time( NULL ) );
}

//----------------------------------------------------------------------------------------
// Destructor
A3::~A3()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A3::init()
{
	reset();
	glClearColor(0.5, 0.5, 0.5, 1.0);
	createShaderProgram();

	glGenVertexArrays( 1, &m_vao_meshData );
	glGenVertexArrays( 1, &m_vao_normalMeshData );
	glGenVertexArrays( 1, &vao_skybox );
	glGenVertexArrays( 1, &vao_particles );
	enableVertexShaderInputSlots();

	processLuaSceneFiles();
	initPlayer();

	meshConsolidator.reset( new MeshConsolidator {
			getAssetFilePath( "cube.obj" ),
			getAssetFilePath( "sphere.obj" ),
	} );
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);
	initBuildingManager();

	initVortex();
	Block::fillTangentsBitangents( block_rootNode.get() );

	initBuildingManager();
	uploadVertexDataToVbos(*meshConsolidator);
	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();
	updateViewMatrix();
	initLightSources();

	initTextures();
	initFramebuffers();

	last_time = glfwGetTime();
}

void A3::initTextures() {
	cout << "init textures" << endl;
	tex_skybox = loadCubemap( {
		"Assets/Textures/nec_city/city_rt.TGA",	
		"Assets/Textures/nec_city/city_lf.TGA",	
		"Assets/Textures/nec_city/city_up.TGA",	
		"Assets/Textures/nec_city/city_dn.TGA",	
		"Assets/Textures/nec_city/city_bk.TGA",	
		"Assets/Textures/nec_city/city_ft.TGA"
	} );
	tex_building = loadTexture( "Assets/Textures/buildings/high_rise.jpg" );
	// tex_building = loadTexture( "Assets/Textures/buildings/window.jpg" );
	// tex_building = loadTexture( "Assets/Textures/nec_city/city_rt.TGA" );	
	// tex_diffuse = loadTexture( "Assets/Textures/normal/brickwall.jpg" );
	// tex_normal = loadTexture( "Assets/Textures/normal/brickwall_normal.jpg" );
	tex_diffuse = loadTexture( "Assets/Textures/normal/concrete.jpg" );
	tex_normal = loadTexture( "Assets/Textures/normal/concrete_normal.jpg" );
	tex_vortex = loadTexture( "Assets/Textures/vortex.jpg" );
	//tex_diffuse = loadTexture( "Assets/Textures/normal/concrete.jpg" );
	//tex_normal = loadTexture( "Assets/Textures/normal/concrete_normal.jpg" );
}

void A3::initFramebuffers() {
	cout << "init framebuffers" << endl;

	glGenFramebuffers( 1, &framebuffer );
	glBindFramebuffer( GL_FRAMEBUFFER, framebuffer );

	// colour
	glGenTextures( 1, &tex_colour );
	glBindTexture( GL_TEXTURE_2D, tex_colour );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, m_windowWidth, m_windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
	glGenerateMipmap( GL_TEXTURE_2D );

	// depth
	glGenTextures( 1, &tex_depth );
	glBindTexture( GL_TEXTURE_2D, tex_depth );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_windowWidth, m_windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0 ); 
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	// attach
	glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex_colour, 0 );
	glFramebufferTexture( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, tex_depth, 0 );

	GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
	if( status != GL_FRAMEBUFFER_COMPLETE ) cout << "framebuffer's fucked" << endl;
	glBindTexture( GL_TEXTURE_2D, 0 );
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

}

void A3::initBuildingManager() {
	cout << "init collider statics & building manager" << endl;
	Collider::init( &m_batchInfoMap, meshConsolidator.get() );
	
	buildingManager.reset( new BuildingManager( building_rootNode.get(), orb_rootNode.get(), block_rootNode.get(), half_building, gravity, player->getJumpSpeed(), aspect, fov ) );

	buildingManager->generateBuilding( player->getMoveSpeed() );
	buildingManager->generateBuilding( player->getMoveSpeed() );
	// buildingManager->generateBuilding( player->getMoveSpeed() );
}

void A3::initPlayer() { 
	cout << "init player" << endl;
	player.reset( new Player{ puppet_rootNode->get_transform(),  gravity } ); 
}

void A3::initVortex() {
	vortex.reset( new Block{ block_rootNode.get(), vortex_init * 
	inverse( ( block_rootNode.get()->children.front() )->trans ) });
}

//----------------------------------------------------------------------------------------
void A3::processLuaSceneFiles() {
	puppet_rootNode = std::shared_ptr<SceneNode>( import_lua( "Assets/puppet.lua" ) );
	if ( !puppet_rootNode ) {
		std::cerr << "Could Not Open Assets/puppet.lua" << std::endl;
	}

	building_rootNode = std::shared_ptr<SceneNode>( import_lua( "Assets/building.lua" ) );
	if ( !building_rootNode ) {
		std::cerr << "Could Not Open Assets/building.lua" << std::endl;
	}

	orb_rootNode = shared_ptr<SceneNode>( import_lua( "Assets/orb.lua" ) );
	if( !orb_rootNode ) {
		cerr << "Could not open Assets/orb.lua" << endl;
	}

	block_rootNode = shared_ptr<SceneNode>( import_lua( "Assets/block.lua" ) );
	if( !block_rootNode ) {
		cerr << "Could not open Assets/block.lua" << endl;
	}
}

//----------------------------------------------------------------------------------------
void A3::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath( "VertexShader.vs" ).c_str() );
	m_shader.attachFragmentShader( getAssetFilePath( "FragmentShader.fs" ).c_str() );
	m_shader.link();

	skybox_shader.generateProgramObject();
	skybox_shader.attachVertexShader( getAssetFilePath( "skybox_VertexShader.vs" ).c_str() );
	skybox_shader.attachFragmentShader( getAssetFilePath( "skybox_FragmentShader.fs" ).c_str() );
	skybox_shader.link();

	particle_shader.generateProgramObject();
	particle_shader.attachVertexShader( getAssetFilePath( "particle_VertexShader.vs" ).c_str() );
	particle_shader.attachFragmentShader( getAssetFilePath( "particle_FragmentShader.fs" ).c_str() );
	particle_shader.link();

	normal_shader.generateProgramObject();
	normal_shader.attachVertexShader( getAssetFilePath( "normal_VertexShader.vs" ).c_str() );
	normal_shader.attachFragmentShader( getAssetFilePath( "normal_FragmentShader.fs" ).c_str() );
	normal_shader.link();
}

//----------------------------------------------------------------------------------------
void A3::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);
		m_positionAttribLocation = m_shader.getAttribLocation( "position" );
		glEnableVertexAttribArray(m_positionAttribLocation);
		m_normalAttribLocation = m_shader.getAttribLocation( "normal" );
		glEnableVertexAttribArray( m_normalAttribLocation );
		m_uvAttribLocation = m_shader.getAttribLocation("uv");
		glEnableVertexAttribArray( m_uvAttribLocation );
		CHECK_GL_ERRORS;
	}

	// skybox
	{
		glBindVertexArray( vao_skybox );
		skybox_positionAttribLocation = skybox_shader.getAttribLocation( "position" );
		glEnableVertexAttribArray( skybox_positionAttribLocation );
		CHECK_GL_ERRORS;
	}

	// particle
	{
		glBindVertexArray( vao_particles );
		particle_positionSizeAttribLocation = particle_shader.getAttribLocation( "pos_size" );
		glEnableVertexAttribArray( particle_positionSizeAttribLocation );
		particle_colourAttribLocation = particle_shader.getAttribLocation( "col" );
		glEnableVertexAttribArray( particle_colourAttribLocation );
		particle_squareVerticesAttribLocation = particle_shader.getAttribLocation( "square_vertices" );
		glEnableVertexAttribArray( particle_squareVerticesAttribLocation );

		glVertexAttribDivisor( particle_squareVerticesAttribLocation, 0 );
        glVertexAttribDivisor( particle_positionSizeAttribLocation, 1 );
        glVertexAttribDivisor( particle_colourAttribLocation, 1 );

		CHECK_GL_ERRORS;
	} 
	
	// normal
	{
		glBindVertexArray(m_vao_normalMeshData);
		m_normalPositionAttribLocation = normal_shader.getAttribLocation( "pos" );
		glEnableVertexAttribArray(m_normalPositionAttribLocation);
		m_normalNormalAttribLocation = normal_shader.getAttribLocation( "normal" );
		glEnableVertexAttribArray( m_normalAttribLocation );
		m_normalUVAttribLocation = normal_shader.getAttribLocation( "uv" );
		glEnableVertexAttribArray( m_normalUVAttribLocation );
		m_normalTangentAttribLocation = normal_shader.getAttribLocation( "tangent" );
		glEnableVertexAttribArray( m_normalTangentAttribLocation );
		//m_normalBitangentAttribLocation = normal_shader.getAttribLocation( "bitangent" );
		//glEnableVertexAttribArray( m_normalBitangentAttribLocation );
		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A3::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);
		CHECK_GL_ERRORS;

		
		glGenBuffers(1, &m_vbo_vertexNormals);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;

		glGenBuffers( 1, &m_vbo_vertexUVs );
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_vertexUVs );
		glBufferData( GL_ARRAY_BUFFER, meshConsolidator.getNumVertexUVBytes(), meshConsolidator.getVertexUVDataPtr(), GL_STATIC_DRAW );
		CHECK_GL_ERRORS;

		glGenBuffers(1, &m_vbo_vertexTangents);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexTangents);
		glBufferData(GL_ARRAY_BUFFER, Block::tangent_polys.size() * sizeof( vec3 ), &Block::tangent_polys[0].x, GL_STATIC_DRAW );
		CHECK_GL_ERRORS;
	}

	// Generate VBO for skybox
	{
		glGenBuffers( 1, &vbo_skybox );
		glBindBuffer( GL_ARRAY_BUFFER, vbo_skybox );
		glBufferData( GL_ARRAY_BUFFER, sizeof( skybox_vertices ), &skybox_vertices[0], GL_STATIC_DRAW );
		CHECK_GL_ERRORS;
	}

	// Generate VBOs for particles
	{
		glGenBuffers( 1, &particle_vertex_buf );
		glBindBuffer( GL_ARRAY_BUFFER, particle_vertex_buf );
		glBufferData( GL_ARRAY_BUFFER, sizeof( particle_vertices ), &particle_vertices[0], GL_STATIC_DRAW );
		CHECK_GL_ERRORS;

		glGenBuffers( 1, &particles_pos_buf );
		glBindBuffer( GL_ARRAY_BUFFER, particles_pos_buf );
		glBufferData( GL_ARRAY_BUFFER, Player::MAX_PARTICLES * 4 * sizeof( GLfloat ), NULL, GL_STREAM_DRAW );
		CHECK_GL_ERRORS;

		glGenBuffers( 1, &particles_col_buf );
		glBindBuffer( GL_ARRAY_BUFFER, particles_col_buf );
		glBufferData( GL_ARRAY_BUFFER, Player::MAX_PARTICLES * 4 * sizeof( GLfloat ), NULL, GL_STREAM_DRAW );
		CHECK_GL_ERRORS;
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//----------------------------------------------------------------------------------------
void A3::mapVboDataToVertexShaderInputLocations()
{
	/* main shader */
	glBindVertexArray(m_vao_meshData);
	// vertices
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	// normals
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	// texture(uv)
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexUVs);
	glVertexAttribPointer(m_uvAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	// unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	CHECK_GL_ERRORS;

	/* skybox shader */
	glBindVertexArray( vao_skybox );
	// vertices
	glBindBuffer( GL_ARRAY_BUFFER, vbo_skybox );
	glVertexAttribPointer( skybox_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr );
	// unbind
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindVertexArray( 0 );
	CHECK_GL_ERRORS;

	/* particle shader */
	glBindVertexArray( vao_particles );
	glBindBuffer( GL_ARRAY_BUFFER, particle_vertex_buf );
	glVertexAttribPointer( particle_squareVerticesAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindVertexArray( 0 );
	CHECK_GL_ERRORS;

	/* normal shader */
	glBindVertexArray( m_vao_normalMeshData );
	// pos
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_normalPositionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	// normals
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalNormalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	// texture(uv)
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexUVs);
	glVertexAttribPointer(m_normalUVAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	// tangents
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexTangents);
	glVertexAttribPointer(m_normalTangentAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindVertexArray( 0 );
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A3::initPerspectiveMatrix()
{
	aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perspective = glm::perspective( degreesToRadians( fov ), aspect, near, far );
}


//----------------------------------------------------------------------------------------
void A3::updateViewMatrix() {
	player->updateCameraStatus( delta_time );
	m_view = lookAt( player->getCameraPosition(), player->getCameraLookAt(), vec3(0.0f, 1.0f, 0.0f) );
    prev_viewProjection = viewProjection;
    viewProjection = m_view * m_perspective;
	
}

//----------------------------------------------------------------------------------------
void A3::initLightSources() {
	// World-space position
	m_light_dir = vec3{ 0.0f, 1.0f, 1.0f };
	m_light.rgbIntensity = vec3( 0.75f ); // light
}

//----------------------------------------------------------------------------------------
void A3::uploadCommonSceneUniforms() {
	m_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perspective));
		CHECK_GL_ERRORS;
	
		//-- Set LightSource uniform for the scene:
		{
			location = m_shader.getUniformLocation("light.rgbIntensity");
			glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
			CHECK_GL_ERRORS;
		}

		//-- Set background light ambient intensity
		{
			location = m_shader.getUniformLocation("ambientIntensity");
			vec3 ambientIntensity(0.1f);
			glUniform3fv(location, 1, value_ptr(ambientIntensity));
			CHECK_GL_ERRORS;
		}

		{
			location = m_shader.getUniformLocation("invViewProjection");
			glUniformMatrix4fv( location, 1, GL_FALSE, value_ptr( inverse( viewProjection ) ) );
			CHECK_GL_ERRORS;

			location = m_shader.getUniformLocation("prev_viewProjection");
			glUniformMatrix4fv( location, 1, GL_FALSE, value_ptr( prev_viewProjection ) );
			CHECK_GL_ERRORS;
		}

		location = m_shader.getUniformLocation( "tex" );
		glUniform1i( location, 0 );
		location = m_shader.getUniformLocation( "depthTexture" );
		glUniform1i( location, 1 );
		location = m_shader.getUniformLocation( "sceneSampler" );
		glUniform1i( location, 2 );
	}
	m_shader.disable();

	skybox_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = skybox_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perspective));
		CHECK_GL_ERRORS;
	}
	skybox_shader.disable();

	normal_shader.enable();
	{
		GLint location = normal_shader.getUniformLocation( "projection" );
		glUniformMatrix4fv( location, 1, GL_FALSE, value_ptr( m_perspective ) );
		location = normal_shader.getUniformLocation( "diffuseMap" );
		glUniform1i( location, 0 );
		location = normal_shader.getUniformLocation( "normalMap" );
		glUniform1i( location, 1 );
		CHECK_GL_ERRORS;
	}
	normal_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A3::appLogic()
{
	if( !toggle_skybox ) updateViewMatrix();

	if( player->getGameMode() ) player->updatePosition( buildingManager->getBuildingColliders(), buildingManager->getOrbColliders(), delta_time );
	else {
		if( left_pressed ) player->updatePosition( buildingManager->getBuildingColliders(), buildingManager->getOrbColliders(), 0, delta_time );
		if( up_pressed ) player->updatePosition( buildingManager->getBuildingColliders(), buildingManager->getOrbColliders(), 1, delta_time );
		if( right_pressed ) player->updatePosition( buildingManager->getBuildingColliders(), buildingManager->getOrbColliders(), 2, delta_time );
		if( down_pressed ) player->updatePosition( buildingManager->getBuildingColliders(), buildingManager->getOrbColliders(), 3, delta_time );
		if( r_pressed ) player->updatePosition( buildingManager->getBuildingColliders(), buildingManager->getOrbColliders(), 4, delta_time );
		if( f_pressed ) player->updatePosition( buildingManager->getBuildingColliders(), buildingManager->getOrbColliders(), 5, delta_time );
	}
	if( player->getMoveSpeed() >= BLUR_SPEED ) toggle_motion_blur = true;
	buildingManager->spawnIfPossible( player->getPosition().z, player->getMoveSpeed() );
	uploadCommonSceneUniforms();
	updateLightingUniforms();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A3::guiLogic()
{
	if( !show_gui ) {
		return;
	}

	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);


		// Add more gui elements here here ...


		// Create Button, and check if it was clicked:
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}

		ImGui::Text( "Orbs collected: %d", Orb::orbs_collected );
		ImGui::Text( "Score: %.1f", abs( player->getPosition().z ) );

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

void A3::updateNormalShaderUniforms( const GeometryNode& geoNode, const mat4& view ) {
	normal_shader.enable();
	{
		GLint location = normal_shader.getUniformLocation( "model" );
		glUniformMatrix4fv( location, 1, GL_FALSE, value_ptr( inverse( m_view ) * view * geoNode.trans ) );
		CHECK_GL_ERRORS;

		location = normal_shader.getUniformLocation( "view" );
		glUniformMatrix4fv( location, 1, GL_FALSE, value_ptr( m_view ) );
		CHECK_GL_ERRORS;

		//location = normal_shader.getUniformLocation( "lightPos" );
		//glUniform3f( location, 100.0, 0.0, -100.0 );
		//CHECK_GL_ERRORS;

		location = normal_shader.getUniformLocation( "viewPos" );
		glUniform3f( location, player->getCameraPosition().x, player->getCameraPosition().y, player->getCameraPosition().z );	
		glUniform3f( location, 0, 0, -100 );	
		CHECK_GL_ERRORS;

		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, tex_diffuse );
		glGenerateMipmap( GL_TEXTURE_2D );
		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D, tex_normal );
		
	}
	normal_shader.disable();
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniforms(
		const ShaderProgram & shader,
		const GeometryNode & node,
		const glm::mat4 & viewMatrix
) {
	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		mat4 modelView = viewMatrix * node.trans;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;

		//-- Set NormMatrix:
		location = shader.getUniformLocation("NormalMatrix");
		mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
		glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
		CHECK_GL_ERRORS;

		//-- Set Material values:
		location = shader.getUniformLocation("material.kd");
		vec3 kd = node.material.kd;
		glUniform3fv(location, 1, value_ptr(kd));
		CHECK_GL_ERRORS;
		location = shader.getUniformLocation("material.ks");
		vec3 ks = node.material.ks;
		glUniform3fv(location, 1, value_ptr(ks));
		CHECK_GL_ERRORS;
		location = shader.getUniformLocation("material.shininess");
		glUniform1f(location, node.material.shininess);
		CHECK_GL_ERRORS;
	}
	shader.disable();
}

void A3::updateLightingUniforms() {
	if( n_pressed ) m_light_dir = rotate( m_light_dir, radians( (float)delta_time * LIGHT_DIR_SCALE ), vec3( 0, 1, 0 ) );
	if( m_pressed ) m_light_dir = rotate( m_light_dir, radians( (float)-delta_time * LIGHT_DIR_SCALE ), vec3( 0, 1, 0 ) );

	m_shader.enable();
	{
		GLint location = m_shader.getUniformLocation("light.direction");
		glUniform3fv(location, 1, value_ptr( m_light_dir ));
	}
	m_shader.disable();

	normal_shader.enable();
	{
		GLint location = normal_shader.getUniformLocation("lightDirection");
		glUniform3fv(location, 1, value_ptr( m_light_dir ));
	}
	normal_shader.disable();
}

void A3::checkForTextures( string name ) {
	m_shader.enable();
	{
		bool enabled = false;
		if( name.length() != tex_id_len ) 
			cout << "checkForTextures requires an id of length: " << tex_id_len << endl;
		else {
			if( name == "tx0" ) { // building in building.lua
				glActiveTexture( GL_TEXTURE0 );
				glBindTexture( GL_TEXTURE_2D, tex_building ); 
				GLint location = m_shader.getUniformLocation( "uv_scaling" );
				glUniform2f( location, 3.0f, 50.0f );
				enabled = true;
			}
			if( name == "tx1" && toggle_normal_shader ) {
				glActiveTexture( GL_TEXTURE0 );
				glBindTexture( GL_TEXTURE_2D, tex_vortex );
				GLint location = m_shader.getUniformLocation( "uv_scaling" );
				glUniform2f( location, 1.0f, 1.0f );
				enabled = true;
			}
			if( name == "tx1" && !toggle_normal_shader ) {
				glActiveTexture( GL_TEXTURE0 );
				glBindTexture( GL_TEXTURE_2D, tex_diffuse );
				GLint location = m_shader.getUniformLocation( "uv_scaling" );
				glUniform2f( location, 1.0f, 1.0f );
				enabled = true;
			}
		}

		if( !toggle_texture ) enabled = false;
		GLint location = m_shader.getUniformLocation( "use_texture" );
		glUniform1i( location, enabled );
	}
	m_shader.disable();
}

void A3::drawSkybox() {
	mat4 view = mat4( mat3( m_view ) );

	glDepthMask( GL_FALSE );
	skybox_shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = skybox_shader.getUniformLocation( "View" );
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr( view ) );
		CHECK_GL_ERRORS;

		glDepthMask( GL_FALSE );
		glBindVertexArray( vao_skybox );
		glBindTexture( GL_TEXTURE_CUBE_MAP, tex_skybox );
		glDrawArrays( GL_TRIANGLES, 0, 36 );
		glBindVertexArray( 0 );
	}
	skybox_shader.disable();
	glDepthMask( GL_TRUE );
}

void A3::drawParticles() {
	ParticleSystem& lf_ps = player->getLeftFootParticleSystem();
	const int particle_count = lf_ps.updateParticles( delta_time, particle_camera_pos );

	// orphaning
	glBindBuffer( GL_ARRAY_BUFFER, particles_pos_buf );
	glBufferData( GL_ARRAY_BUFFER, Player::MAX_PARTICLES * 4 * sizeof( GLfloat ), NULL, GL_STREAM_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, particle_count * sizeof( GLfloat ) * 4, lf_ps.getParticlePosSizeData() );

	glBindBuffer( GL_ARRAY_BUFFER, particles_col_buf );
	glBufferData( GL_ARRAY_BUFFER, Player::MAX_PARTICLES * 4 * sizeof( GLfloat ), NULL, GL_STREAM_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, particle_count * sizeof( GLfloat ) * 4, lf_ps.getParticleColData() );

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	particle_shader.enable();
	{
		// uniforms	
		GLint location = particle_shader.getUniformLocation( "ViewProjection" );
        glUniformMatrix4fv( location, 1, GL_FALSE, value_ptr( m_perspective * m_view ) );
		location = particle_shader.getUniformLocation( "camera_right_ws" );
        glUniform3f( location, m_view[0][0], m_view[1][0], m_view[2][0] );
		location = particle_shader.getUniformLocation( "camera_up_ws" );
        glUniform3f( location, m_view[0][1], m_view[1][1], m_view[2][1] );

		glBindVertexArray( vao_particles );

		glBindBuffer( GL_ARRAY_BUFFER, particles_pos_buf );
		glVertexAttribPointer( particle_positionSizeAttribLocation, 4, GL_FLOAT, GL_FALSE, 0, nullptr );

		glBindBuffer( GL_ARRAY_BUFFER, particles_col_buf );
		glVertexAttribPointer( particle_colourAttribLocation, 4, GL_FLOAT, GL_FALSE, 0, nullptr );

		glDrawArraysInstanced( GL_TRIANGLE_STRIP, 0, 4, particle_count );

		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindVertexArray( 0 );
	}
	particle_shader.disable();

	glDisable( GL_BLEND );
	
}

void A3::enableMotionBlur( bool enabled ) {
	m_shader.enable();
	GLint location = m_shader.getUniformLocation( "use_motion_blur" );
	glUniform1i( location, enabled );
	m_shader.disable();
}

void A3::uploadMotionBlurUniforms() {
	m_shader.enable();
	glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, tex_depth );
	glActiveTexture( GL_TEXTURE2 );
	glBindTexture( GL_TEXTURE_2D, tex_colour );
	glGenerateMipmap( GL_TEXTURE_2D );
	GLint location = m_shader.getUniformLocation( "width" );
	glUniform1f( location, m_windowWidth );
	location = m_shader.getUniformLocation( "height" );
	glUniform1f( location, m_windowHeight );
	m_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A3::draw() {
	double current_time = glfwGetTime();
	delta_time = current_time - last_time;
	last_time = current_time;
	if( delta_time < 0.03 ) delta_time = 0.03;

	//glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	//glDisable( GL_DEPTH_TEST );
	//drawSkybox();

	if( toggle_skybox ) {
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		glDisable( GL_DEPTH_TEST );
		drawSkybox();

		if( up_pressed ) m_view = lookAt( vec3( 0, 0, 0 ), vec3( 0, 0, -1 ), vec3( 0, 1, 0 ) ); 
		if( left_pressed ) m_view = lookAt( vec3( 0, 0, 0 ), vec3( -1, 0, 0 ), vec3( 0, 1, 0 ) ); 
		if( right_pressed ) m_view = lookAt( vec3( 0, 0, 0 ), vec3( 1, 0, 0 ), vec3( 0, 1, 0 ) ); 
		if( down_pressed ) m_view = lookAt( vec3( 0, 0, 0 ), vec3( 0, 0, 1 ), vec3( 0, 1, 0 ) ); 
		if( r_pressed ) m_view = lookAt( vec3( 0, 0, 0 ), vec3( 0, 1, 0 ), vec3( 0, 0, 1 ) ); 
		if( f_pressed ) m_view = lookAt( vec3( 0, 0, 0 ), vec3( 0, -1, 0 ), vec3( 0, 0, -1 ) ); 
	}
	else {
		glBindFramebuffer( GL_FRAMEBUFFER, framebuffer );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		enableMotionBlur( false );
		drawScene( false );

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		uploadMotionBlurUniforms();
		if( !no_motion_blur ) enableMotionBlur( toggle_motion_blur );
		drawScene( true );
	}
}

void A3::drawScene( bool toScreen ) {
	glDisable( GL_DEPTH_TEST );
	drawSkybox();

	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LESS );

	which_shader = DEFAULT_SHADER;
	{
		if( toScreen ) {
			enableMotionBlur( false );
			renderSceneGraph( *puppet_rootNode, player->getModelMatrix() );
			vortex_trans = translate( mat4{ 1.0f }, vec3( player->getPosition().x, 0, player->getPosition().z ) );
			vortex_rot = rotate( vortex_rot, (float)delta_time, vec3( 0, 1, 0 ) );
			if( toggle_vortex ) renderSceneGraph( *vortex->getRoot(), vortex_trans * vortex_rot * vortex->getTrans() );
			if( !no_motion_blur ) enableMotionBlur( toggle_motion_blur );
		}

		for( const Building& building : buildingManager->getBuildings() ) {
			renderSceneGraph( *building.getRoot(), building.getTrans() );
		}
		for( const Orb& orb : buildingManager->getOrbs() ) {
			if( orb.checkForCollision() ) renderSceneGraph( *orb.getRoot(), orb.getTrans()  );
		}
	}

	if( toggle_normal_shader ) which_shader = NORMAL_SHADER;
	{
		for( const Block& block : buildingManager->getBlocks() ) {
			renderSceneGraph( *block.getRoot(), block.getTrans() );
		}
	}
	
	if( toScreen ) drawParticles();
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A3::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A3::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A3::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A3::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A3::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A3::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	initPerspectiveMatrix();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A3::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if( action == GLFW_PRESS ) {
		switch( key ) {
			case GLFW_KEY_Q: quit(); break;

			case GLFW_KEY_W: player->faceForward(); break;
			case GLFW_KEY_A: player->faceLeft(); break;
			case GLFW_KEY_D: player->faceRight(); break;
			case GLFW_KEY_SPACE: player->jump(); break;

			//
			case GLFW_KEY_UP: up_pressed = true; break;
			case GLFW_KEY_LEFT: left_pressed = true; break;
			case GLFW_KEY_DOWN: down_pressed = true; break;
			case GLFW_KEY_RIGHT: right_pressed = true; break;
			case GLFW_KEY_C: player->toggleGameMode(); break;
			case GLFW_KEY_R: r_pressed = true; break;
			case GLFW_KEY_F: f_pressed = true; break;
			case GLFW_KEY_M: m_pressed = true; break;
			case GLFW_KEY_N: n_pressed = true; break;
			case GLFW_KEY_1: m_light_dir = vec3( 0.0, 1.0, 1.0 ); break;
			case GLFW_KEY_2: m_light_dir = vec3( 0.0, 0.0, 1.0 ); break;
			case GLFW_KEY_3: toggle_normal_shader = !toggle_normal_shader; break;
			case GLFW_KEY_4: toggle_texture = !toggle_texture; break;
			case GLFW_KEY_5: toggle_vortex = !toggle_vortex; break;
			case GLFW_KEY_6: toggle_skybox = !toggle_skybox; break;
			case GLFW_KEY_7: toggle_motion_blur = !toggle_motion_blur; break;
			case GLFW_KEY_8: no_motion_blur = !no_motion_blur; break;
			case GLFW_KEY_9: player->toggle_always_wind = !player->toggle_always_wind; break;
			case GLFW_KEY_0: toggle_bb = !toggle_bb; break;
		}
	}
	else if ( action == GLFW_RELEASE ) {
		switch( key ) {
			case GLFW_KEY_UP: up_pressed = false; break;
			case GLFW_KEY_LEFT: left_pressed = false; break;
			case GLFW_KEY_DOWN: down_pressed = false; break;
			case GLFW_KEY_RIGHT: right_pressed = false; break;
			case GLFW_KEY_R: r_pressed = false; break;
			case GLFW_KEY_F: f_pressed = false; break;
			case GLFW_KEY_M: m_pressed = false; break;
			case GLFW_KEY_N: n_pressed = false; break;
		}
	}

	return eventHandled;
}

void A3::quit() { glfwSetWindowShouldClose( m_window, GL_TRUE ); }

void A3::renderSceneGraph( const SceneNode & root, const mat4& trans ) {
	if( which_shader == DEFAULT_SHADER ) glBindVertexArray( m_vao_meshData );   
	else glBindVertexArray( m_vao_normalMeshData );

    for (const SceneNode* node : root.children) {
        switch( node->m_nodeType ) {
            case NodeType::GeometryNode :
                renderGeometryNode( *static_cast<const GeometryNode*>(node), m_view * trans * root.trans );
                break;
            case NodeType::JointNode :
                renderJointNode( *static_cast<const JointNode*>(node), m_view * trans * root.trans );
                break;
        }
    }
    glBindVertexArray(0);
    CHECK_GL_ERRORS;
}

void A3::renderGeometryNode( const GeometryNode& geoNode, mat4 view ) {
    for( const SceneNode* node : geoNode.children ) {
        switch( node->m_nodeType ) {
            case NodeType::GeometryNode :
                renderGeometryNode( *static_cast<const GeometryNode*>(node), view );
                break;
            case NodeType::JointNode :
                renderJointNode( *static_cast<const JointNode*>(node), view );
                break;
        }
    }

	if( !toggle_bb && geoNode.m_name == "bbb" ) return;

    // draw
	if( which_shader == DEFAULT_SHADER ) {
		updateShaderUniforms( m_shader, geoNode, view );
		checkForTextures( geoNode.m_name.substr( 0, 3 ) );
		m_shader.enable();
	}
	else {
		updateNormalShaderUniforms( geoNode, view );
		normal_shader.enable();
	}

    BatchInfo batchInfo = m_batchInfoMap[geoNode.meshId];
    glDrawArrays( GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices );
	
	if( which_shader == DEFAULT_SHADER ) m_shader.disable();
	else normal_shader.disable();

}

void A3::renderJointNode( const JointNode& jointNode, mat4 view ) {
    mat4 about_joint = jointNode.get_transform() * jointNode.getRotationMatrix() *  jointNode.get_inverse();

    for( const SceneNode* node : jointNode.children ) {
        switch( node->m_nodeType ) {
            case NodeType::GeometryNode :
                renderGeometryNode( *static_cast<const GeometryNode*>(node), view * about_joint );
                break;
            case NodeType::JointNode :
                renderJointNode( *static_cast<const JointNode*>(node), view * about_joint );
                break;
        }
    }
}

unsigned int A3::loadTexture( const char* path ) {
	unsigned int tex_id;
	glGenTextures( 1, &tex_id );
	int w, h, n_components;
	stbi_set_flip_vertically_on_load( true );
	unsigned char* data = stbi_load( path, &w, &h, &n_components, 0 );
	if( data ) {
		GLenum format;
		if( n_components == 1 ) format = GL_RED;
		else if( n_components == 3 ) format = GL_RGB;
		else if( n_components == 4 ) format = GL_RGBA;

		glBindTexture( GL_TEXTURE_2D, tex_id );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data );
		glGenerateMipmap( GL_TEXTURE_2D );

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}
	else cerr << "failed to load texture: " << path << endl;

	return tex_id;
}

// r,l,t,bot,f,back
unsigned int A3::loadCubemap( vector<string> faces ) {
	unsigned int tex_id;
	glGenTextures( 1, &tex_id );
	glBindTexture( GL_TEXTURE_CUBE_MAP, tex_id );
	int w, h, n_components;
	for( unsigned int i = 0; i < faces.size(); i++ ) {
		unsigned char* data = stbi_load( faces[i].c_str(), &w, &h, &n_components, 0 );
		if( data ) glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data );
		else cerr << "failed to load texture: " << faces[i] << endl;
		stbi_image_free( data );
	}
	
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	
	return tex_id;
}

void A3::reset() {
	up_pressed = false;
	down_pressed = false;
	left_pressed = false;
	right_pressed = false;
	r_pressed = false;
	f_pressed = false;
	n_pressed = false;
	m_pressed = false;
}

