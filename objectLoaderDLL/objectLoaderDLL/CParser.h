#pragma once

#include "stdafx.h"

#include <Windows.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

#ifdef DLL_CLASS


#else

#define DLL_CLASS __declspec(dllimport)
#include <string>

#endif


class DLL_CLASS CParser
{
private:

	std::string m_file;

	int m_numVerticesIndex, m_numNormalsIndex, m_numUVIndex;

protected:
	unsigned int m_shaderProgramId;                                   // Shader program identifier generated by OpenGL
	unsigned int m_graphicsMemoryObjectId;                            // Vertex Array Object identifier generated by OpenGL
	unsigned int m_textureObjectId;									  // Texture object identifier generated by OpenGL

	unsigned short *m_vertexIndices;                                  // Dynamically-allocated array of vertex indices for this model
	unsigned short *m_normalIndices;                                  // Dynamically-allocated array of normal indices for this model
	unsigned short *m_UVindices;                                      // Dynamically-allocated array of UV coord indices for this model

	float *m_verticesRaw;                                             // Dynamically-allocated array of vertices (raw float values)
	float *m_normalsRaw;                                              // Dynamically-allocated array of normals (raw float values)
	float *m_uvCoordsRaw;                                             // Dynamically-allocated array of UV coords (raw float values)

	int m_numVertices, m_numNormals, m_numUVCoords, m_numFaces;       // Total number of vertices, normals, UV coords, and faces in this model

	bool m_Initialized;                                               // Does this C3DModel object contain valid data? (loaded from file)
	bool m_modelHasNormals;											  // Flag to determine if this model has normals
	bool m_modelHasUVs;												  // Flag to determine if this model has UVs
	bool m_modelHasTextures;										  // Flag to determine if this model has a valid texture filename
	char *m_modelTextureFilename;


	bool loadFromFile(const char* const filename);
	bool parser(std::string dataToFind, int &arraySize, float* &arrayDestination);
	bool parser(std::string dataToFind, int &arraySize, unsigned short* &arrayDestination);
	void normalIndicesParser(int arraySizeSource, int &arraySizeDestination, unsigned short* arraySource, unsigned short* &arrayDestination);

public:

	CParser();
	~CParser();


	void reset();
	bool loader(const char* const filename);
	int getNumNormalsIndices() { return m_numNormalsIndex; }
	int getNumVertexIndices() { return m_numVerticesIndex; }
	int getNumUVIndices() { return m_numUVIndex; }
};


