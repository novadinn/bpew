#ifndef TEXTURE_H
#define TEXTURE_H

#include "../core/platform.h"
#include <map>
#include <vector>
#include <string>

enum class TextureType {
	IMAGE,
};
	
struct Texture2D {
  void createFromFile(const char *p);
  void destroy();

  void bind();
  void unbind();

  inline uint getID() const { return id; }
  inline uint getWidth() const { return width; }
  inline uint getHeight() const { return height; }	
  
	static Texture2D *getTexture(std::string &path);
	static Texture2D *getTexture(uint index);
	static std::string getTexturePath(uint index);
  static bool textureLoaded(std::string &path);	
  static void addTexture(Texture2D texture);
	static void loadTexture(std::string path, Texture2D &texture);	
	static bool validTextureIndex(uint index);
	
	// NOTE: All textures should be stored at here
	static std::vector<Texture2D> textures;
	// NOTE: All loaded textures should store name (map<texture path, texture index>)
  static std::map<std::string, uint> loaded_textures;	

	std::string name;
private:
  uint id;
  uint width, height;
	TextureType type;
};

#endif
