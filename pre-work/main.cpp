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
Defines of the main chunks we need to read datas
*/
#define MAIN_CHUNK      0x4D4D
#define EDITOR_CHUNK    0x3D3D
#define OBJECT_CHUNK    0x4000
#define MESH_CHUNK      0x4100
#define VERTICES_CHUNK  0x4110
#define FACES_CHUNK     0x4120

// https://baptiste-wicht.com/posts/2011/06/write-and-read-binary-files-in-c.html
template<typename T>
std::istream& binary_read(std::istream& stream, T& value)
{
    return stream.read(reinterpret_cast<char*>(&value), sizeof(T));
}

typedef struct Chunk {
    unsigned short id;
    unsigned int length;
} Chunk;

std::istream& operator >> (std::istream& is, Chunk& chunk)
{
    binary_read(is, chunk.id);
    binary_read(is, chunk.length);
    return is;
}

std::ostream& operator << (std::ostream& os, const Chunk& chunk)
{
    os << "CHUNK ID: " << std::hex << chunk.id;
    os << " | CHUNK LENGTH: " << std::dec << chunk.length;
    return os;
}

void read_object_name(std::istream& is, char name[20])
{
    unsigned short i = 0;
    char c;

    do {
        binary_read(is, c);
        name[i++] = c;
    } while( c != '\0' && i < 20 );
}

int main(int argc, char** argv)
{
    if( argc != 2 ){
        std::cerr << "Usage" << std::endl;
        return EXIT_FAILURE;
    }

    Chunk* chunk = new Chunk();

    char object_name[20];
    unsigned short quantity;
    unsigned short i;
    float value;

    // Open file in reading & binary mode
    std::ifstream file(argv[1], std::ios::binary | std::ios::ate );
    if( file.is_open() ){

        long fsize = file.tellg();
        file.seekg(0, std::ios::beg);

        // Process reading of 3DS File
        while( file.tellg() < fsize )
        {
            file >> *chunk;
            std::cerr << *chunk << std::endl;
            std::cerr << "Position: " << file.tellg() << std::endl;

            switch( chunk->id )
            {
                case MAIN_CHUNK: 
                    // nothing to do here, 0 bytes to read
                    std::cerr << "MAIN" << std::endl;
                break;

                case EDITOR_CHUNK: 
                    // nothing to do here, 0 bytes to read
                    std::cerr << "EDITOR" << std::endl;
                break;

                case OBJECT_CHUNK:
                    std::cerr << "OBJECT" << std::endl;
                    read_object_name(file, object_name);
                    std::cerr << object_name << std::endl;
                break;

                case MESH_CHUNK:
                    std::cerr << "MESH" << std::endl;
                break;

                case VERTICES_CHUNK:
                    std::cerr << "VERTICES" << std::endl;
                    binary_read(file, quantity);
                    for(i=0; i < quantity; ++i){
                        std::cerr << i << " ";
                        binary_read(file, value); std::cerr << " x: " << value;
                        binary_read(file, value); std::cerr << " y: " << value;
                        binary_read(file, value); std::cerr << " z: " << value << std::endl;
                    }
                break;

                /*
                case FACES_CHUNK:
                    std::cerr << "FACES" << std::endl;

                break;
                */

                default:
                    // Move forward from the current position by the current chunk length 
                    // minus the (2bytes ID + 4bytes length) already read.
                    file.seekg(chunk->length-6, std::ios::cur);
            }
        }

        file.close();
    }
    else {
        std::cerr << "Failed to read " << argv[1] << " file." << std::endl;
    }

    delete chunk;
    return EXIT_SUCCESS;
}