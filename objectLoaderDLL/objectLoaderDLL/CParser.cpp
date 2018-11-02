#include "stdafx.h"
#include "CParser.h"


using namespace std;

#define DLL_CLASS __declspec(dllexport)

DLL_CLASS::CParser::CParser()
{
	m_numVertices = 0;
	m_numNormals = 0;
	m_numUVCoords = 0;
	m_numFaces = 0;
}

DLL_CLASS::CParser::~CParser()
{
	reset();
}

void DLL_CLASS::CParser::reset()
{
	if (m_vertexIndices != NULL)
	{
		cout << "deleting vertex indices" << endl;
		delete[] m_vertexIndices;
		m_vertexIndices = NULL;
	}
	if (m_normalIndices != NULL)
	{
		cout << "deleting normal indices" << endl;
		delete[] m_normalIndices;
		m_normalIndices = NULL;
	}
	if (m_UVindices != NULL)
	{
		cout << "deleting UV indices" << endl;
		delete[] m_UVindices;
		m_UVindices = NULL;
	}
	if (m_verticesRaw != NULL)
	{
		cout << "deleting vertices (float)" << endl;
		delete[] m_verticesRaw;
		m_verticesRaw = NULL;
	}
	if (m_normalsRaw != NULL)
	{
		cout << "deleting normals (float)" << endl;
		delete[] m_normalsRaw;
		m_normalsRaw = NULL;
	}
	if (m_uvCoordsRaw != NULL)
	{
		cout << "deleting uvCoords (float)" << endl;
		delete[] m_uvCoordsRaw;
		m_uvCoordsRaw = NULL;
	}
	if (m_modelTextureFilename != nullptr)
	{
		delete[] m_modelTextureFilename;
		m_modelTextureFilename = nullptr;
	}

	m_numVertices = 0;
	m_numNormals = 0;
	m_numUVCoords = 0;
	m_numFaces = 0;

	m_Initialized = false;

	m_graphicsMemoryObjectId = 0;
	m_shaderProgramId = 0;
	m_textureObjectId = 0;
	m_modelHasNormals = false;
	m_modelHasUVs = false;
	m_modelHasTextures = false;
}

void DLL_CLASS::CParser::normalIndicesParser(int arraySizeSource, int &arraySizeDestination, unsigned short* arraySource, unsigned short* &arrayDestination)
{
	arraySizeDestination = arraySizeSource * 3;
	arrayDestination = new unsigned short[arraySizeDestination];


	size_t index = 0;

	for (size_t i = 0; i < arraySizeDestination; i += 3)
	{
		arrayDestination[i] = arraySource[index];
		arrayDestination[i + 1] = arraySource[index];
		arrayDestination[i + 2] = arraySource[index];

		index++;
	}
}

bool DLL_CLASS::CParser::loadFromFile(const char* const filename)
{
	reset();

	string file = filename;

	//Crea un Fstream que abre el archivo a leer en modo binario y busca el final del mismo
	std::ifstream reader;
	reader.open(file, std::ios::in | std::ios::binary | std::ios::ate);

	//Si el archivo no se lee correctamente, devuelve falso
	if (reader.fail())
	{
		return false;
	}

	//Retorna el tamaño del archivo
	size_t sizeOfFile = reader.tellg();

	//Posiciona al buffer en la primera direccion del archivo
	reader.seekg(std::ios::beg);

	//Crea un string del tamaño del archivo
	m_file = std::string(sizeOfFile, 0);

	//Lee todo el archivo y lo almacena en el string "textFile"
	reader.read(&m_file[0], sizeOfFile);

	return true;
}

bool DLL_CLASS::CParser::parser(std::string dataToFind, int &arraySize, float* &arrayDestination)
{

	size_t posIni = m_file.find(dataToFind + ": *") + dataToFind.length() + 3;
	size_t posFin = m_file.find(' ', posIni);

	arraySize = stoi(m_file.substr(posIni, posFin - posIni));

	if (arrayDestination == nullptr)
	{
		arrayDestination = new float[arraySize * sizeof(unsigned short)];
	}

	else
	{
		return false;
	}

	posIni = m_file.find("a: ", posFin) + 3;
	posFin = m_file.find('}', posFin);


	std::string dataToConvert = m_file.substr(posIni, posFin - posIni);
	dataToConvert.erase(std::remove(dataToConvert.begin(), dataToConvert.end(), '\r'), dataToConvert.end());
	dataToConvert.erase(std::remove(dataToConvert.begin(), dataToConvert.end(), '\n'), dataToConvert.end());
	dataToConvert.erase(std::remove(dataToConvert.begin(), dataToConvert.end(), '\t'), dataToConvert.end());
	dataToConvert.erase(std::remove(dataToConvert.begin(), dataToConvert.end(), ' '), dataToConvert.end());

	posIni = 0;
	posFin = dataToConvert.find(',');
	int index = 0;

	while (posFin < dataToConvert.length())
	{
		posFin = dataToConvert.find(',', posIni);
		arrayDestination[index] = stof(dataToConvert.substr(posIni, posFin - posIni));
		posIni = posFin + 1;

		index++;
	}

	return true;
}

bool DLL_CLASS::CParser::parser(std::string dataToFind, int &arraySize, unsigned short* &arrayDestination)
{
	size_t posIni = m_file.find(dataToFind + ": *") + dataToFind.length() + 3;
	size_t posFin = m_file.find(' ', posIni);

	arraySize = stoi(m_file.substr(posIni, posFin - posIni));
	arrayDestination = new unsigned short[arraySize * sizeof(float)];

	posIni = m_file.find("a: ", posFin) + 3;
	posFin = m_file.find('}', posFin);


	std::string dataToConvert = m_file.substr(posIni, posFin - posIni);

	dataToConvert.erase(std::remove(dataToConvert.begin(), dataToConvert.end(), '\r'), dataToConvert.end());
	dataToConvert.erase(std::remove(dataToConvert.begin(), dataToConvert.end(), '\n'), dataToConvert.end());
	dataToConvert.erase(std::remove(dataToConvert.begin(), dataToConvert.end(), '\t'), dataToConvert.end());
	dataToConvert.erase(std::remove(dataToConvert.begin(), dataToConvert.end(), ' '), dataToConvert.end());

	posIni = 0;
	posFin = dataToConvert.find(',');

	int index = 0;
	int parsedNumber = 0;

	while (posFin < dataToConvert.length())
	{
		posFin = dataToConvert.find(',', posIni);

		parsedNumber = (stoi(dataToConvert.substr(posIni, posFin - posIni)));

		if (dataToFind == "PolygonVertexIndex" && parsedNumber < 0)
		{
			parsedNumber = (parsedNumber * -1) - 1;
		}

		arrayDestination[index] = parsedNumber;
		posIni = posFin + 1;

		index++;
	}

	return true;
}

bool DLL_CLASS::CParser::loader(const char* const filename)
{
	if (!loadFromFile(filename))
	{
		return false;
	}

	parser("Vertices", m_numVertices, m_verticesRaw);
	parser("PolygonVertexIndex", m_numVerticesIndex, m_vertexIndices);

	parser("Normals", m_numNormals, m_normalsRaw);
	normalIndicesParser(m_numVerticesIndex, m_numNormalsIndex, m_vertexIndices, m_normalIndices);

	parser("UV", m_numUVCoords, m_uvCoordsRaw);
	parser("UVIndex", m_numUVIndex, m_UVindices);

	return true;
}