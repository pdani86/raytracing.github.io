
#ifndef MYUTILS_H
#define MYUTILS_H

#include <vector>
#include <fstream>

inline std::vector<unsigned char> mapToBytePerChannel(const std::vector<double>& in, double scale) { // TODO: stride
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

inline void writeToImage(int w, int h, const std::vector<double>& image, int samples_per_pixel) {
    auto imageBytes = mapToBytePerChannel(image, 255.0/samples_per_pixel);

    std::ofstream outFile;
    //outFile.open("image.raw", , std::ios::out | std::ios::binary);
    outFile.open("img600.bmp", std::ios::binary | std::ios::in | std::ios::out | std::ios::ate);
    outFile.seekp(18);
    outFile.write((char*)&w, 4);
    outFile.write((char*)&h, 4);
    outFile.seekp(54);
    outFile.write((char*)imageBytes.data(), imageBytes.size());
    outFile.close();
}

#endif
