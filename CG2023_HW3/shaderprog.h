#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "headers.h"

// ShaderProg Declarations.
class ShaderProg
{
public:
	// ShaderProg Public Methods.
	ShaderProg();
	~ShaderProg();

	bool LoadFromFiles(const std::string vsFilePath, const std::string fsFilePath);
	void Bind() { glUseProgram(shaderProgId); };
	void UnBind() { glUseProgram(0); };

	GLint GetLocMVP() const { return locMVP; }

protected:
	// ShaderProg Protected Methods.
	virtual void GetUniformVariableLocation();

	// ShaderProg Protected Data.
	GLuint shaderProgId;

private:
	// ShaderProg Private Methods.
	GLuint AddShader(const std::string& sourceText, GLenum shaderType);
	static bool LoadShaderTextFromFile(const std::string filePath, std::string& sourceText);

	// ShaderProg Private Data.
	GLint locMVP;
};

// ------------------------------------------------------------------------------------------------

// FillColorShaderProg Declarations.
class FillColorShaderProg : public ShaderProg
{
public:
	// FillColorShaderProg Public Methods.
	FillColorShaderProg();
	~FillColorShaderProg();

	GLint GetLocFillColor() const { return locFillColor; }

protected:
	// FillColorShaderProg Protected Methods.
	void GetUniformVariableLocation();

private:
	// FillColorShaderProg Private Data.
	GLint locFillColor;
};

// ------------------------------------------------------------------------------------------------

// PhongShadingDemoShaderProg Declarations.
class PhongShadingDemoShaderProg : public ShaderProg
{
public:
	// PhongShadingDemoShaderProg Public Methods.
	PhongShadingDemoShaderProg();
	~PhongShadingDemoShaderProg();

	GLint GetLocM() const { return locM; }
	GLint GetLocNM() const { return locNM; }
	GLint GetLocCameraPos() const { return locCameraPos; }
	GLint GetLocKa() const { return locKa; }
	GLint GetLocKd() const { return locKd; }
	GLint GetLocKs() const { return locKs; }
	GLint GetLocNs() const { return locNs; }
	GLint GetLocAmbientLight() const { return locAmbientLight; }
	GLint GetLocDirLightDir() const { return locDirLightDir; }
	GLint GetLocDirLightRadiance() const { return locDirLightRadiance; }
	GLint GetLocPointLightPos() const { return locPointLightPos; }
	GLint GetLocPointLightIntensity() const { return locPointLightIntensity; }
	GLint GetLocSpotLightPosition() const { return locSpotLightPosition; }
	GLint GetLocSpotLightDirection() const { return locSpotLightDirection; }
	GLint GetLocSpotLightIntensity() const { return locSpotLightIntensity; }
	GLint GetLocSpotLightCutoffStartInDegree() const { return locSpotLightCutoffStartInDegree; }
	GLint GetLocSpotLightTotalWidthInDegree() const { return locSpotLightTotalWidthInDegree; }
	GLint GetLocMapKd() const{ return locMapKd; } //new
	GLint GetLocTime() const { return locTime; }
	GLint GetLocSideToSide() const { return locSideToSide; }
	GLint GetLocTimeScale() const { return locTimeScale; }
	GLint GetLocPivot() const { return locPivot; }
	GLint GetLocWave() const { return locWave; }
	GLint GetLocTwist() const { return locTwist; }
	GLint GetLocMaskBlack() const { return locMaskBlack; }
	GLint GetLocMaskWhite() const { return locMaskWhite; }

protected:
	// PhongShadingDemoShaderProg Protected Methods.
	void GetUniformVariableLocation();

private:
	// PhongShadingDemoShaderProg Public Data.
	// Transformation matrix.
	GLint locM;
	GLint locNM;
	GLint locCameraPos;

	// Material properties.
	GLint locKa;
	GLint locKd;
	GLint locKs;
	GLint locNs;

	// AmbientLight data.
	GLint locAmbientLight;

	// DirectionalLight data
	GLint locDirLightDir;
	GLint locDirLightRadiance;

	// Pointlight data
	GLint locPointLightPos;
	GLint locPointLightIntensity;

	// SpotLight data
	GLint locSpotLightPosition;
	GLint locSpotLightDirection;
	GLint locSpotLightIntensity;
	GLint locSpotLightCutoffStartInDegree;
	GLint locSpotLightTotalWidthInDegree;

	// Map data
	GLint locMapKd; //new

	//Animation
	GLint locTime;
	GLint locSideToSide;
	GLint locTimeScale;
	GLint locPivot;
	GLint locWave;
	GLint locTwist;
	GLint locMaskBlack;
	GLint locMaskWhite;
};

// ------------------------------------------------------------------------------------------------

// SkyboxShaderProg Declarations.
class SkyboxShaderProg : public ShaderProg
{
public:
	// SkyboxShaderProg Public Methods.
	SkyboxShaderProg();
	~SkyboxShaderProg();

	GLint GetLocMapKd() const { return locMapKd; }

protected:
	// PhongShadingDemoShaderProg Protected Methods.
	void GetUniformVariableLocation();

private:
	// SkyboxShaderProg Public Data.
	GLint locMapKd;
};


// LineShaderProg Declarations.
class LineShaderProg : public ShaderProg
{

public:
	// LineShaderProg Public Methods.
	LineShaderProg();
	~LineShaderProg();

	GLint GetLocMapKd() const { return locMapKd; }


protected:
	// LineShaderProg Protected Methods.
	void GetUniformVariableLocation();


private:
	// LineShaderProg Public Data.
	GLint locMapKd;


};



// BubbleShaderProg Declarations.
class BubbleShaderProg : public ShaderProg
{

public:
	// BubbleShaderProg Public Methods.
	BubbleShaderProg();
	~BubbleShaderProg();

	GLint GetLocMapKd() const { return locMapKd; }

protected:
	// BubbleShaderProg Protected Methods.
	void GetUniformVariableLocation();


private:
	// BubbleShaderProg Public Data.
	GLint locMapKd;

};

#endif
