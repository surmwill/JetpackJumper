// Spring 2019

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "SceneNode.hpp"
#include "Player.hpp"
#include "Block.hpp"

#include <glm/glm.hpp>
#include <memory>

#include <string>
#include <vector>
#include <array>

class GeometryNode;
class JointNode;
class Collider;
class BuildingManager;

struct LightSource {
	glm::vec3 position;
	glm::vec3 rgbIntensity;
};


class A3 : public CS488Window {
public:
	A3(const std::string & luaSceneFile);
	virtual ~A3();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	//-- Virtual callback methods
	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	//-- One time initialization methods:
	void processLuaSceneFiles();
	void createShaderProgram();
	void enableVertexShaderInputSlots();
	void uploadVertexDataToVbos(const MeshConsolidator & meshConsolidator);
	void mapVboDataToVertexShaderInputLocations();
	void updateViewMatrix();
	void initLightSources();

	void initPerspectiveMatrix();
	void uploadCommonSceneUniforms();
	void uploadMotionBlurUniforms();
	void renderSceneGraph( const SceneNode& node, const glm::mat4& trans );
	void renderArcCircle();

	glm::mat4 m_perspective;
	glm::mat4 m_view;

	LightSource m_light;

	//-- GL resources for mesh geometry data:
	GLuint m_vao_meshData;
	GLuint m_vbo_vertexPositions;
	GLuint m_vbo_vertexNormals;
	GLuint m_vbo_vertexUVs;
	GLint m_positionAttribLocation;
	GLint m_normalAttribLocation;
	GLint m_uvAttribLocation;
	ShaderProgram m_shader;

	GLuint vao_skybox;
	GLuint vbo_skybox;
	GLint skybox_positionAttribLocation;
	ShaderProgram skybox_shader;

	GLuint m_vao_normalMeshData;
	ShaderProgram normal_shader;
	GLuint m_vbo_vertexTangents;
	GLuint m_vbo_vertexBitangents;
	GLint m_normalPositionAttribLocation;
	GLint m_normalNormalAttribLocation;
	GLint m_normalUVAttribLocation;
	GLint m_normalTangentAttribLocation;
	GLint m_normalBitangentAttribLocation;

	ShaderProgram particle_shader;
	GLuint vao_particles;
	GLint particle_positionSizeAttribLocation;
	GLint particle_colourAttribLocation;
	GLint particle_squareVerticesAttribLocation;
	GLuint particles_pos_buf, particles_col_buf, particle_vertex_buf;

	// BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
	// object. Each BatchInfo object contains an index offset and the number of indices
	// required to render the mesh with identifier MeshId.
	BatchInfoMap m_batchInfoMap;

	std::string m_luaSceneFile;

	std::shared_ptr<SceneNode> puppet_rootNode;
	std::shared_ptr<SceneNode> building_rootNode;
	std::shared_ptr<SceneNode> orb_rootNode;
	std::shared_ptr<SceneNode> block_rootNode;

private:
	const int DEFAULT_SHADER = 0, NORMAL_SHADER = 1;
	const float LIGHT_DIR_SCALE = 100.0f;
	const float BLUR_SPEED = 55.0f;
	int which_shader = DEFAULT_SHADER;

	const float gravity = -9.81f * 2.0f;

	void renderGeometryNode( const GeometryNode& geoNode, glm::mat4 view );
	void renderJointNode( const JointNode& jointNode, glm::mat4 view );
	void drawSkybox();
	void drawParticles();
	void updateNormalShaderUniforms( const GeometryNode& geoNode, const glm::mat4& view );
	void enableMotionBlur( bool enabled );

	void reset();
	void initTextures();
	void initBuildingManager();
	void initPlayer();
	void initVortex();
	void initFramebuffers();

	unsigned int loadTexture( const char* path );
	unsigned int loadCubemap( std::vector<std::string> faces );

	void quit();
	void checkForTextures( std::string name );
	void drawScene( bool toScreen );
	void updateLightingUniforms();

	double delta_time = 0.0f, last_time;
	float half_building = 25.0f;
	float fov = 60.0f, near = 0.1f, far = 1000.0f, aspect;
	glm::vec3 m_light_dir{ 0.0f, 1.0f, 1.0f };
	
	unsigned int tex_skybox, tex_building, tex_diffuse, tex_normal, tex_vortex, tex_colour, tex_depth;
	unsigned int tex_id_len = 3;
	unsigned int framebuffer;
	std::unique_ptr<Player> player;
	std::unique_ptr<MeshConsolidator> meshConsolidator;
	std::unique_ptr<BuildingManager> buildingManager;
	std::unique_ptr<Block> vortex;
	bool up_pressed, left_pressed, down_pressed, right_pressed, r_pressed, f_pressed, n_pressed, m_pressed;
	bool toggle_normal_shader = true, toggle_texture = true, toggle_vortex = true, toggle_skybox = false, toggle_bb = false;
	bool toggle_motion_blur = false, no_motion_blur = false;
	glm::vec3 camera_pos, particle_camera_pos{ 0.0f, 30.0f, 30.0f }; // I dont know if this is important
	glm::mat4 vortex_init = glm::translate( glm::scale( glm::mat4{ 1.0f }, glm::vec3( 1500, 1, 1500 ) ),
								glm::vec3( 0.0f, -500.0f, 0.0 ) );
	glm::mat4 vortex_trans{ 1.0f }, vortex_rot{ 1.0f };
	glm::mat4 viewProjection{ 1.0f }, prev_viewProjection{ 1.0f };

	std::array<float, 12> particle_vertices { {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.0f
	} };

	std::array<float, 108> skybox_vertices{ {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    } };
};
