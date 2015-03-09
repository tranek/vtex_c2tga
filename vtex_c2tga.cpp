#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <Magick++.h>

using namespace Magick;

enum IMAGE_FORMATS {
	RGBA8888 = 0,
	ABGR8888 = 1,
	RBG888 = 2,
	BGR888 = 3,
	RBG565 = 4,
	DXT1 = 13,
	DXT3 = 14,
	DXT5 = 15
};

enum IMAGE_FLAGS {
	TSPEC_RENDER_TARGET  = 1,
	TSPEC_SUGGEST_CLAMP_S  = 16,
	TSPEC_SUGGEST_CLAMP_T  = 32,
	TSPEC_SUGGEST_CLAMP_U  = 64,
	TSPEC_NO_LOD     = 128,
	TSPEC_CUBE_TEXTURE  = 256,
	TSPEC_VOLUME_TEXTURE  = 512,
	TSPEC_TEXTURE_ARRAY  = 1024,
};

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cout << "Incorrect arguments." << std::endl;
		std::cout << "Arg 1 = /path/to/.vtex_c file." << std::endl;
		std::cout << "Arg 2 = /output/directory." << std::endl;
		exit(1);
	}

	std::streampos size;
	char * memblock;
	char header1[4];
	int i, j;

	std::ifstream file (argv[1], std::ios::in|std::ios::binary|std::ios::ate);
    if (!file.is_open())
	{
		std::cout << "Failed to load .vtex_c file." << std::endl;
        exit(1);
    }

	std::cout << "Loading: " << argv[1] << std::endl;

	size = file.tellg();
	memblock = new char [size];
	file.seekg (0, std::ios::beg);
	file.read (memblock, size);
	file.close();

	//std::string memblock_str = memblock;
	std::vector<char> memblock_vector;
	memblock_vector.assign(memblock, memblock + size);

	// Check file header for 0x0c
	char header2[4];
	header2[0] = 0x0c;
	header2[1] = 0x00;
	header2[2] = 0x00;
	header2[3] = 0x00;

	memcpy(header1, memblock+4, 4);
	if (strcmp(header1, header2) != 0) {
		printf("Incorrect header version: ");
		for(i=0; i<4; i++)
			printf("%02x ", header1[i]);
		exit(1);
	}

	// Get and print file info
	int start_of_DATA_header;
	char DATA_offset_array[4];
	int DATA_offset = 0;
	short width;
	char * width_char;
	short height;
	char * height_char;
	short depth;
	short format;
	std::string format_name = "";
	short mip_count;
	uint64_t flags;
	char * reflectivity;
	std::string render_target = "false";
	std::string suggest_clamp_s = "false";
	std::string suggest_clamp_t = "false";
	std::string suggest_clamp_u = "false";
	std::string no_lod = "false";
	std::string cube_texture = "false";
	std::string volume_texture = "false";
	std::string texture_array = "false";

	// Find the location of the DATA header
	char DATA[4] = {0x44, 0x41, 0x54, 0x41};
	
	std::vector<char>::iterator it;
	it = std::search (memblock_vector.begin(), memblock_vector.end(), DATA, DATA+4);
	if (it!=memblock_vector.end()) {
		//std::cout << "DATA found at position " << (it - memblock_vector.begin()) << '\n';
		start_of_DATA_header = it - memblock_vector.begin();
	} else {
		std::cout << "DATA not found\n";
		exit(1);
	}


	memcpy(DATA_offset_array, memblock + start_of_DATA_header + 4, 4);

	DATA_offset = *(int *)DATA_offset_array + start_of_DATA_header + 4;

	width_char = memblock + DATA_offset;
	width = *(short *)width_char;

	std::cout << "Width: " << width << std::endl;

	height_char = memblock + DATA_offset + 2;
	height = *(short *)height_char;
	std::cout << "Height: " << height << std::endl;

	depth = *(short *)(memblock + DATA_offset + 4);
	std::cout << "Depth: " << depth << std::endl;
	
	format = (short)*(memblock + DATA_offset + 6);
	switch(format) {
		case RGBA8888:
			format_name.append(" (RGBA8888)");
		case DXT1:
			format_name.append(" (DXT1)");
			break;
		case DXT5:
			format_name.append(" (DXT5)");
			break;
		default:
			printf("Invalid image format: %hd.", format);
			exit(1);
	}
	std::cout << "Format: " << format << format_name << std::endl;

	mip_count = (short)*(memblock + DATA_offset + 7);
	std::cout << "Number of Mips: " << mip_count << std::endl;

	flags = *(uint64_t *)(memblock + DATA_offset + 14);

	if (flags & TSPEC_RENDER_TARGET) {
		render_target = "true";
	}
	std::cout << "TSPEC_RENDER_TARGET: " << render_target << std::endl;

	if (flags & TSPEC_SUGGEST_CLAMP_S) {
		suggest_clamp_s = "true";
	}
	std::cout << "TSPEC_SUGGEST_CLAMP_S: " << suggest_clamp_s << std::endl;

	if (flags & TSPEC_SUGGEST_CLAMP_T) {
		suggest_clamp_t = "true";
	}	
	std::cout << "TSPEC_SUGGEST_CLAMP_T: " << suggest_clamp_t << std::endl;

	if (flags & TSPEC_SUGGEST_CLAMP_U) {
		suggest_clamp_u = "true";
	}
	std::cout << "TSPEC_SUGGEST_CLAMP_U: " << suggest_clamp_u << std::endl;
	
	if (flags & TSPEC_NO_LOD) {
		no_lod = "true";
	}
	std::cout << "TSPEC_NO_LOD: " << no_lod << std::endl;

	if (flags & TSPEC_CUBE_TEXTURE) {
		cube_texture = "true";
	}
	std::cout << "TSPEC_CUBE_TEXTURE: " << cube_texture << std::endl;

	if (flags & TSPEC_VOLUME_TEXTURE) {
		volume_texture = "false";
	}
	std::cout << "TSPEC_VOLUME_TEXTURE: " << volume_texture << std::endl;

	if (flags & TSPEC_TEXTURE_ARRAY) {
		texture_array = "false";
	}
	std::cout << "TSPEC_TEXTURE_ARRAY: " << texture_array << std::endl;

	reflectivity = memblock + DATA_offset + 22;
	std::cout << "Reflectivity: " << *(int *)reflectivity << std::endl;

	// Get the start location of mip 0
	char length_of_DATA_block[2];
	char * start_of_image_data;

	memcpy(length_of_DATA_block, memblock + start_of_DATA_header + 8, 2);

	start_of_image_data = memblock + DATA_offset + *(short *)length_of_DATA_block;

	// Compute size of each mip level except 0
	int offset_of_mip0 = 0;
	int mip_width = width;
	int mip_height = height;
	int mip_size = 0;
	for (i=1; i<mip_count; i++) {
		mip_size = 0;
		mip_width /= 2;
		mip_height /= 2;
		if (mip_width <= 1 || mip_height <= 1)
			break;
		switch (format) {
			case RGBA8888:
				mip_size = mip_width * mip_height * 4;
			case DXT1:
				mip_size = mip_width * mip_height / 2;
				break;
			case DXT5:
				mip_size = mip_width * mip_height;
				break;
			default:
				printf("Invalid image format: %hd.", format);
				exit(1);
		}
		offset_of_mip0 += mip_size;
	}

	// Calculate the number of images per mip level
	int num_images_per_mip_level = depth;
	if (cube_texture.compare("true") == 0) {
		num_images_per_mip_level *= 6;
	}
	offset_of_mip0 *= num_images_per_mip_level;

	// Loop for number of images per mip level
	for (i=0; i<num_images_per_mip_level; i++) {
		// Build a proper source file image format header (for DXT1 and DXT5)
		char * header;
		char dxt1_header[] = {0x44, 0x44, 0x53, 0x20, 0x7C, 0x00, 0x00, 0x00, 0x07, 0x10, 0x08, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x44, 0x58, 0x54, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
		char dxt5_header[] = {0x44, 0x44, 0x53, 0x20, 0x7C, 0x00, 0x00, 0x00, 0x07, 0x10, 0x08, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x44, 0x58, 0x54, 0x35, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
		int header_size = 0;
		int size_of_formatted_image = 0;
		std::string input_format = "RGBA";

		switch (format) {
			case RGBA8888: // no header
				size_of_formatted_image = width * height * 4;
				break;
			case DXT1: // 0x80 bytes long
				header_size = 128;
				header = new char[header_size];
				//bytes 13-16 are height
				dxt1_header[12] = height_char[0];
				dxt1_header[13] = height_char[1];

				//bytes 17-20 are width
				dxt1_header[16] = width_char[0];
				dxt1_header[17] = width_char[1];

				//bytes 23-26 are depth
				//bytes 27-30 are mip count
			
				memcpy(header, dxt1_header, header_size);

				size_of_formatted_image = width * height / 2;

				input_format = "DDS";
				break;
			case DXT5: // 0x80 bytes long
				header_size = 128;
				header = new char[header_size];
				//bytes 13-16 are height
				dxt5_header[12] = height_char[0];
				dxt5_header[13] = height_char[1];

				//bytes 17-20 are width
				dxt5_header[16] = width_char[0];
				dxt5_header[17] = width_char[1];

				//bytes 23-26 are depth
				//bytes 27-30 are mip count
			
				memcpy(header, dxt5_header, header_size);

				size_of_formatted_image = width * height;
				input_format = "DDS";
				break;
			default:
				printf("Invalid image format: %hd.", format);
				exit(1);
		}


		// Attach header to image data
		char * out_data = new char[size_of_formatted_image+header_size];
		if (header_size > 0) {
			memcpy(out_data, header, header_size);
		}
		memcpy(out_data+header_size, start_of_image_data + offset_of_mip0 + (size_of_formatted_image * i), size_of_formatted_image);

		// Run image data through Imagemagick and output a .tga
		Image image;
		image.type(::Magick::TrueColorType);
		image.modifyImage();

		std::string dimensions = std::to_string(width) + "x" + std::to_string(height);

		Blob blob(out_data, size_of_formatted_image+header_size);
		image.read(blob, dimensions.c_str(), input_format.c_str());

		std::string out_name(argv[2]);

		// Hopefully there won't be texture arrays of cubemaps!
		if (cube_texture.compare("true") == 0) {
			int ext_size = out_name.length() - 4;
			switch(i) {
				case 0:
					out_name.replace(ext_size, 4, "_rt.tga");
					break;
				case 1:
					out_name.replace(ext_size, 4, "_lf.tga");
					break;
				case 2:
					out_name.replace(ext_size, 4, "_bk.tga");
					break;
				case 3:
					out_name.replace(ext_size, 4, "_ft.tga");
					break;
				case 4:
					out_name.replace(ext_size, 4, "_up.tga");
					break;
				case 5:
					out_name.replace(ext_size, 4, "_dn.tga");
					break;
				default:
					std::cout << "More than 6 images with a cubemap! Halp!" << std::endl;
					break;
			}
		}

		std::cout << "Writing: " << out_name << std::endl;

		image.magick("TGA");
		image.write(out_name.c_str());
	}

	exit(0);
}