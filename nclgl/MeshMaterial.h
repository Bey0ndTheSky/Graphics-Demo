#pragma once
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <glad/glad.h>

using std::map;
using std::string;
using std::vector;

class MeshMaterialEntry {
public:
	std::map<string, string> entries;
	std::map<string, GLuint> textures;

	bool GetEntry(const string& name, const string** output) const {
		auto i = entries.find(name);
		if (i == entries.end()) {
			return false;
		}
		*output = &i->second;
		return true;
	}
};

class MeshMaterial
{
public:
	MeshMaterial(const std::string& filename);
	~MeshMaterial() {}
	MeshMaterialEntry* GetMaterialForLayer(int i);

	std::vector<MeshMaterialEntry>	materialLayers;
	std::vector<MeshMaterialEntry*> meshLayers;

protected:
	
};

