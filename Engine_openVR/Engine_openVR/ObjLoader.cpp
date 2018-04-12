#include "ObjLoader.h"






void ObjLoader::HandleVertexData(std::string type, float x, float y, float z)
{
	switch (type[1])
	{
	case 't':
		uvs.push_back(x);
		uvs.push_back(y);
		break;
	case ' ':
		vertices.push_back(x);
		vertices.push_back(y);
		vertices.push_back(z);
		break;
	}
}

void ObjLoader::HandleFaceData(std::string triplet)
{
	float v, vt, vn;
	std::istringstream stringStream(triplet);
	std::string token;
	
	std::getline(stringStream, token, '/');
	v = std::stoul(token, 0, 10);

	std::getline(stringStream, token, '/');
	vt = std::stoul(token, 0, 10);

	std::getline(stringStream, token, '/');
	vn = std::stoul(token, 0, 10);

	v -= 1;
	vt -= 1;
	vn -= 1; 

	std::string elementString = std::to_string(v) + std::to_string(vt) + std::to_string(vn);
	auto element = elementsCache.find(elementString);

	if (element != elementsCache.end())
	{
		
		elements.emplace_back(element->second);
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			finalData.emplace_back(vertices[v * 3 + i]);
		}

		finalData.emplace_back(1);//Color
		finalData.emplace_back(1);
		finalData.emplace_back(1);

		for (int i = 0; i < 2; i++)
		{
			finalData.emplace_back(vertices[vt * 2 + i]);
		}

	}
}

ObjLoader::ObjLoader(Shader * shader, std::string fileName): Drawable()
{
	std::ifstream file;
	std::stringstream dataStream;

	


	unsigned int elementID = 0;
	file.open(fileName, std::ios::in);
	if (file)
	{
		std::string line;
		while (std::getline(file, line))
		{
			if(line != "")
			{ 
			std::istringstream stringStream(line.substr(1));
			switch (line[0])
			{
			case '#':
				break;
			case 'v':
				float x, y, z;
				stringStream >> x >> y >> z;
				HandleVertexData(line, x, y, z);
				break;
			case 'f':
				std::string triplet;
				while (stringStream >> triplet)
				{
					std::cout << triplet << std::endl;
					HandleFaceData(triplet);
				}
				break;
			}
			}
		}
		bindVAO();
		loadVertices(finalData);
		loadElements(element);
		
		positionAttribute = shader->getAttribute("position");
		colorAttribute = shader->getAttribute("color");
		uvAttribute = shader->getAttribute("uv");

		glEnableVertexAttribArray(positionAttribute);
		glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
		glEnableVertexAttribArray(colorAttribute);
		glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(uvAttribute);
		glVertexAttribPointer(uvAttribute, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

		unbindVAO();
		vertices.clear();
		uvs.clear();
		finalData.clear();
		elementsCache.clear();
		dataStream.clear();
		file.close();
	}

}

ObjLoader::~ObjLoader()
{
}
