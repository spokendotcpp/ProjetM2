#include <iostream>
#include <fstream>
#include <cstring>

/*
* Check if a filename ends by the 3DS extension.
* ~ INPUT  : a filename (can be an absolute/relative path).
* ~ RETURN : true if INPUT string ends by 3DS, else false.
*/
bool contains_3DS_extension(const char* filename)
{
    char extension[5];

    // 'o.3DS' exemple of min possible length (== 5)
    size_t fname_len = strlen(filename);
    if( fname_len < 5 ){
        std::cerr << "Minimum filename length is not respected" << std::endl;
        return false;
    }

    strncpy(extension, filename+(fname_len-4), 4);
    extension[4] = '\0';

    if( strcmp(extension, ".3DS") != 0 &&
        strcmp(extension, ".3ds") != 0 &&
        strcmp(extension, ".3Ds") != 0 &&
        strcmp(extension, ".3dS") != 0
    )   return false;

    return true;
}

/*
* Read a 3DS file into memory.
*/
char* read_3DS_file(const char* filename)
{
    if( !contains_3DS_extension(filename) ){
        std::cerr << "Will not read this file, it does not contain the 3DS file extension." << std::endl;
        return nullptr;
    }

    // Open file in reading mode + seek end directly.
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    std::streampos filesize = file.tellg();

    if( file.is_open() ){
        char* bytes = new char[filesize];
        file.seekg(0, std::ios::beg);
        file.read(bytes, filesize);
        file.close();
        return bytes;
    }
    else {
        std::cerr << "Unable to open file" << std::endl;
        return nullptr;
    }
}

int main(int argc, char** argv)
{
    std::ifstream file;
    std::streampos size;
    char* bytes = nullptr;
    const char* filename = nullptr;

    if( argc != 2 ){
        std::cerr << "Usage." << std::endl;
        return EXIT_FAILURE;
    }

    filename = argv[1];

    bytes = read_3DS_file(filename);

    // Now process data
    if( bytes != nullptr ){
        // https://en.wikipedia.org/wiki/.3ds
        // Check first Chunk

        char chunk[2];
        chunk[0] = bytes[0];
        chunk[1] = bytes[1];

        char chunk_len[4];
        chunk_len[0] = bytes[2];
        chunk_len[1] = bytes[3];
        chunk_len[2] = bytes[4];
        chunk_len[3] = bytes[5];

        if( strcmp(chunk, "0x4D4D") ){
            std::cerr << "YOUHOU" << std::endl;
        }

        long offset = strtol(chunk_len, NULL, 0);
        std::cerr << "Offset: " << offset << std::endl;


        char chunk_2[2];
        chunk_2[0] = bytes[6];
        chunk_2[1] = bytes[7];

        if( strcmp(chunk_2, "0x3D3D") ){
            std::cerr << "YOUHOU NUMBER 2" << std::endl;
        }

        chunk_len[0] = bytes[8];
        chunk_len[1] = bytes[9];
        chunk_len[2] = bytes[10];
        chunk_len[3] = bytes[11];

        std::cerr << "Offset n°2: " << strtol(chunk_len, NULL, 0) << std::endl;

        chunk[0] = bytes[12];
        chunk[1] = bytes[13];

        if( strcmp(chunk, "0x4000") ){
            std::cerr << "YOUHOU NUMBER 3" << std::endl;
        }
    }
    
    if( bytes != nullptr ){
        delete [] bytes;
        bytes = nullptr;
    }

    return EXIT_SUCCESS;
}