#include "Skybox.h"

namespace Minecraft
{
	Skybox::Skybox() : m_VBO(GL_ARRAY_BUFFER)
	{
        GLfloat SkyboxVertices[] =
        {
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
        };

        m_CubeMap.CreateCubeTextureMap({
            "Core\\Resources\\Skybox\\right.png",
            "Core\\Resources\\Skybox\\left.png",
            "Core\\Resources\\Skybox\\top.png",
            "Core\\Resources\\Skybox\\bottom.png",
            "Core\\Resources\\Skybox\\front.png",
            "Core\\Resources\\Skybox\\back.png"
            });

        m_SkyboxShader.CreateShaderProgramFromFile("Core\\Shaders\\SkyboxVertex.glsl", "Core\\Shaders\\SkyboxFrag.glsl");
        m_SkyboxShader.CompileShaders();

        m_VAO.Bind();
        m_VBO.BufferData(sizeof(SkyboxVertices), SkyboxVertices, GL_STATIC_DRAW);
        m_VBO.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        m_VAO.Unbind();
	}

    void Skybox::RenderSkybox(FPSCamera* camera)
    {
        glDepthMask(GL_FALSE);
        m_SkyboxShader.Use();

        m_SkyboxShader.SetMatrix4("u_ViewProjection", camera->GetViewProjection());
        m_SkyboxShader.SetInteger("u_Skybox", 0);

        m_VAO.Bind();
        glActiveTexture(GL_TEXTURE0);
        DebugGLFunction(glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeMap.GetID()));
        DebugGLFunction(glDrawArrays(GL_TRIANGLES, 0, 36));
        glDepthMask(GL_TRUE);

        m_VAO.Unbind();
    }
}