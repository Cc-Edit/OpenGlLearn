//
//  loadImage.cpp
//  Texture
//
//  Created by 文强 on 2023/11/26.
//
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdio.h>

int width, height, nrChannels;
unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
