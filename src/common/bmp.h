#ifndef BMP_H
#define BMP_H

#include <string>
#include <vector>
#include <fstream>

#pragma pack(push, 1)

struct BMP_INFO_HEADER {
    unsigned int headerSize = sizeof(BMP_INFO_HEADER);
    int bmWidth = 0;
    int bmHeight = 0;
    unsigned short numPlanes = 1;
    unsigned short bitsPerPixel = 24;
    unsigned int compressionMethod = 0;
    unsigned int bitmapDataSize = 0;
    int pixPerMetreHoriz = 0;
    int pixPerMetreVert = 0;
    unsigned int numPaletteCols = 0;
    unsigned int nImportantColor = 0;
};

struct BMP_FILE_HEADER {
    char bm[2] = {'B', 'M'};
    unsigned int filesize = 0;
    unsigned short reserved0 = 0;
    unsigned short reserved1 = 0;
    unsigned int dataOffset = sizeof(BMP_FILE_HEADER) + sizeof(BMP_INFO_HEADER);
};

#pragma pack(pop)

class BMP {
public:
    static bool saveBmpRGB(std::string fname, const std::vector<double>& data, int width, int height, double scale) {
        auto byteData = mapToBytePerChannel(data, scale);
        if(byteData.size() < width*height*3) return false;
        return saveBmpRGB(fname, byteData.data(), width, height);
    }

    static bool saveBmpRGB(std::string fname, unsigned char* data, int width, int height) {
        static_assert (sizeof(BMP_FILE_HEADER) + sizeof(BMP_INFO_HEADER) == 54, "BMP HEADER SIZE != 54");
        std::ofstream out;
        out.open(fname, std::ios::binary | std::ios::out);
        if(!out.is_open()) return false;
        auto dataSize = width * height * 3;
        BMP_FILE_HEADER file_header;
        out.write((char*)&file_header, sizeof(BMP_FILE_HEADER));
        BMP_INFO_HEADER info_header;
        info_header.bitmapDataSize = dataSize;
        info_header.bmWidth = width;
        info_header.bmHeight = height;
        out.write((char*)&info_header, sizeof(BMP_INFO_HEADER));
        out.write((char*)data, dataSize);
        return true;
    }

    static std::vector<unsigned char> mapToBytePerChannel(const std::vector<double>& in, double scale) { // TODO: stride
        std::vector<unsigned char> result;
        result.resize(in.size());
        for(int i=0; i < in.size(); ++i) {
            auto val = in[i];
            val *= scale;
            if(val>255)    val = 255;
            else if(val<0) val = 0;
            result[i] = static_cast<unsigned char>(val);
        }
        return result;
    }
};

#endif // BMP_H
