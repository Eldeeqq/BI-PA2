#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <cmath>
#include <cctype>
#include <climits>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <algorithm>
#include <functional>
#include <memory>

const uint16_t ENDIAN_LITTLE = 0x4949;
const uint16_t ENDIAN_BIG    = 0x4d4d;
#endif /* __PROGTEST__ */

/*
  inline uint16_t LB();
  Converts uint16_t between endians using bitwise operations.
*/
inline uint16_t LB(uint16_t a){
  return ((a & 0xFF)<<8|(a>>8));
}

/**
  # Header struct definition
    works as holder of image header info
**/
struct Header{
  uint16_t endian;
  uint16_t width;
  uint16_t height;
  uint16_t format;
  uint8_t bitsPerChannel;
  uint8_t chanelsPerPixel;
  size_t pixelInBits;
  size_t widthInBytes;

  /*
    bool set();
    Checks the validity of Image header.
    returns: true if it is ok
            false whenever endian value is wrong,
               size is zero or format is incorrect
  */
  bool set(const uint16_t & endian, const uint16_t & width,
            const uint16_t & height, const uint16_t & format);

  /*
    bool setFormat();
    Extracts bits from uint16_t format, checks validity and sets values
    returns: true if it is ok
            false if format is incorrect
  */
  bool setFormat(const uint16_t & format);

};
bool Header::setFormat(const uint16_t & format){
            // format check
            if((format & 0xFFE0) != 0) return false; // bits 8-15 must be 0
            if((format & 0x3) == 1) return false; // invalid channel

            uint16_t check = (format & 0x1C)>>2; // to check valid bits/channel
            if(check != 0 && check != 3 && check != 4) return false;

            // set channels/pixel
            switch ((format & 0x3)){
              case 0x0:
                chanelsPerPixel = 1; break;
              case 0x2:
                chanelsPerPixel = 3; break;
              case 0x3:
                chanelsPerPixel = 4; break;
              default: return false;
            }

            // set bits/channel
            switch (check){
              case 0:
                bitsPerChannel = 1; break;
              case 3:
                bitsPerChannel = 8; break;
              case 4:
                bitsPerChannel = 16; break;
              default: return false;
            }
          this->format = format;
          return true;
}
bool Header::set(const uint16_t & endian, const uint16_t & width, const uint16_t & height, const uint16_t & format){
  // endian check  + set
  if(endian != ENDIAN_BIG && endian != ENDIAN_LITTLE) return false;
  (endian == ENDIAN_LITTLE ?
              this->endian = ENDIAN_LITTLE : this->endian = ENDIAN_BIG );
  // width & height check + set
  if(width == 0 || height == 0) return false;
  (endian == ENDIAN_LITTLE ?
              this->width = width : this->width = LB(width));
  (endian == ENDIAN_LITTLE ?
              this->height = height : this->height = LB(height));

  if(endian==ENDIAN_LITTLE){
    if(!setFormat(format)) return false;
  }
  else {
    if(!setFormat(LB(format))) return false;
  }

  pixelInBits = chanelsPerPixel * bitsPerChannel;

  return true;
}

/**
  # Image class definition
    used for image manipulation
**/
class Image{
  std::ifstream inFile;
  std::ofstream outFile;
  Header header;
  uint8_t ** data;

  public:
  /*
    constructor and destructor using for file opening/closing
  */
  Image(const char * input, const char * output);
  ~Image(){inFile.close(); outFile.close();}
  /*
    bool loadHeader();
    loads image header, then saves it into struct
    also checks validity
    returns false if I/O error occurs, endian is wrong,
    size is 0 or format is incorrect, size of file is invalid (less/more of data)
    true - if everything went good and data were loaded
  */
  bool loadHeader();
  /*
    bool loadData();
    loads the image data as uint8_t (per byte) array
    returns false if I/O error occurs, true otherwise
  */
  bool loadData();
  /*
    bool horizontalFlip();
    flips image horizontally
    might have been void, because there is no chance of error,
    because i handled it in methods called before this one,
    so basically returns true
    - feels more natural to me (like true - work is done)
  */
  bool horizontalFlip();
  /*
    bool verticalFlip();
    flips image too, but vertically
    returns true
  */
  bool verticalFlip();
  /*
    bool saveHeader();
    saves header of img into a file.
    returns false if I/O error occured, true if everything went good
  */
  bool saveHeader();
  /*
    bool saveImage();
    saves the image data which might have been flipped
    returns false if I/O error occured, true if everything went good
  */
  bool saveImage();
  /*
    void freeMemory();
    deletes dynamically allocated memory
  */
  void freeMemory();
};

Image::Image(const char * input, const char * output){
  inFile.open(input,  std::ios::binary | std::ios::in);
  outFile.open(output,  std::ios::binary | std::ios::out);
}

bool Image::loadHeader(){
  uint16_t endian, width, height, format;

  //checks ifstream
  if(!inFile.is_open()) return false;

  // checks file for data
  inFile.seekg(0, std::ios::end);
  int size = inFile.tellg();
  if(size<=8) return false;
  inFile.seekg(0, std::ios::beg);

  // this block loads header data and checks ifstream
  inFile.read((char *) &endian, 2);
    if(!inFile.good()) return false;
  inFile.read((char *) &width, 2);
    if(!inFile.good()) return false;
  inFile.read((char *) &height, 2);
    if(!inFile.good()) return false;
  inFile.read((char *) &format, 2);
    if(!inFile.good()) return false;

  // call header setter which also validates it
  if(!header.set(endian, width, height, format)) return false;
  // check if size in bits is equal to size of bits of an image
  if(size*8 != (int)(header.pixelInBits * header.height * header.width + 64)){
    return false;
  }
  return true;
}

bool Image::loadData(){
 // create array
 size_t widthInBytes = header.width*header.pixelInBits;
  ((widthInBytes%8 == 0) ? widthInBytes/=8 : widthInBytes=1+(widthInBytes/8));
  header.widthInBytes = widthInBytes;
  data = new uint8_t * [header.height];
  for(size_t i = 0; i<header.height; i++){
    data[i] = new uint8_t[widthInBytes];
    for(size_t x = 0; x< widthInBytes; x++){
        inFile.read((char *) &data[i][x], 1);
    }
  }
  // test ifstream for failbit
  if(inFile.fail()) return false;
return true;
}

bool Image::verticalFlip(){
  // swaps rows
  for(size_t i = 0; i<header.height/2; i++ ){
    std::swap(data[i], data[header.height-1-i]);
  }
  return true;
}

bool Image::horizontalFlip(){
  uint8_t tmp;
  size_t pixelWidth = header.bitsPerChannel*header.chanelsPerPixel/8; // <-- finds out how many bytes there are for a pixel
  for(size_t y = 0; y<header.height; y++){
    for(size_t x = 0; x<header.width/2; x++){
      for(size_t len = 0; len<pixelWidth; len++){
        // swaps units
        tmp = data[y][x*pixelWidth+len];
        data[y][x*pixelWidth+len] = data[y][pixelWidth*(header.width-x-1)+len];
        data[y][pixelWidth*(header.width-x-1)+len] = tmp;
      }
    }
  }
  return true;
}

bool Image::saveHeader(){
    if(!outFile.is_open()) return false;

    outFile.write((const char *) &header.endian, sizeof(uint16_t));
    if(!outFile) return false;

    uint16_t tmp;
    (header.endian == ENDIAN_LITTLE ?
    outFile.write((const char *) &header.width, sizeof(uint16_t)):
    (tmp = LB(header.width), outFile.write((const char *)&tmp, sizeof(uint16_t))));
    if(!outFile) return false;

    (header.endian == ENDIAN_LITTLE ?
    outFile.write((const char *) &header.height, sizeof(uint16_t)):
    (tmp = LB(header.height), outFile.write((const char *)&tmp, sizeof(uint16_t))));
    if(!outFile) return false;

    (header.endian == ENDIAN_LITTLE ?
    outFile.write((const char *) &header.format, sizeof(uint16_t)):
    (tmp = LB(header.format), outFile.write((const char *)&tmp, sizeof(uint16_t))));
    if(!outFile) return false;
  return true;
}

bool Image::saveImage(){
  for(size_t i = 0; i<header.height; i++){
    for(size_t x = 0; x<header.widthInBytes; x++){
      outFile.write((const char *) &data[i][x], sizeof(uint8_t));
      if(!outFile) return false; // testing ofstream
    }
  }
  return true;
}

void Image::freeMemory(){
  for(size_t i = 0; i<header.height; i++){
    delete [] data[i];
  }
  delete [] data;
}

bool flipImage ( const char  * srcFileName,
                 const char  * dstFileName,
                 bool          flipHorizontal,
                 bool          flipVertical )
{
  Image m(srcFileName, dstFileName);

  if(!m.loadHeader()) return false;
  if(!m.loadData()){
    m.freeMemory();
    return false;
  }

  if(flipVertical){
      if(!m.verticalFlip()){
        m.freeMemory();
        return false;
      }
  }
  if(flipHorizontal){
      if(!m.horizontalFlip()){
        m.freeMemory();
        return false;
   }
  }

  if(!m.saveHeader()){
    m.freeMemory();
    return false;
  }
  if(!m.saveImage()){
    m.freeMemory();
    return false;
  }
  m.freeMemory();

return true;
}

#ifndef __PROGTEST__
bool identicalFiles ( const char * fileName1, const char * fileName2 )
{
  std::ifstream file1, file2;
  uint16_t b1,b2;

  file1.open(fileName1,  std::ios::binary | std::ios::in);
  file2.open(fileName2,  std::ios::binary | std::ios::in);

  if(!file1.is_open() || !file2.is_open()){
    return false;
  }
  do {
    file1.read((char *) &b1, 2);
    file2.read((char *) &b2, 2);
    if(b1 != b2) {return 0;}
  } while(file1.good()&&file2.good());

  if(!file1.eof() || !file2.eof()) return false;

  return true;
}

int main ( void )
{
  //Image m("input_00.img", "output_00.img");
//flipImage ( "fail.bin", "out.bin", false, true );



  assert ( flipImage ( "input_00.img", "output_00.img", true, false )
           && identicalFiles ( "output_00.img", "ref_00.img" ) );

  assert ( flipImage ( "input_01.img", "output_01.img", false, true )
           && identicalFiles ( "output_01.img", "ref_01.img" ) );

  assert ( flipImage ( "input_02.img", "output_02.img", true, true )
           && identicalFiles ( "output_02.img", "ref_02.img" ) );

  assert ( flipImage ( "input_03.img", "output_03.img", false, false )
           && identicalFiles ( "output_03.img", "ref_03.img" ) );

  assert ( flipImage ( "input_04.img", "output_04.img", true, false )
           && identicalFiles ( "output_04.img", "ref_04.img" ) );

  assert ( flipImage ( "input_05.img", "output_05.img", true, true )
           && identicalFiles ( "output_05.img", "ref_05.img" ) );

  assert ( flipImage ( "input_06.img", "output_06.img", false, true )
           && identicalFiles ( "output_06.img", "ref_06.img" ) );

  assert ( flipImage ( "input_07.img", "output_07.img", true, false )
           && identicalFiles ( "output_07.img", "ref_07.img" ) );

  assert ( flipImage ( "input_08.img", "output_08.img", true, true )
           && identicalFiles ( "output_08.img", "ref_08.img" ) );

  assert ( ! flipImage ( "input_09.img", "output_09.img", true, false ) );

  // extra inputs (optional & bonus tests)
  assert ( flipImage ( "extra_input_00.img", "extra_out_00.img", true, false )
           && identicalFiles ( "extra_out_00.img", "extra_ref_00.img" ) );
  assert ( flipImage ( "extra_input_01.img", "extra_out_01.img", false, true )
           && identicalFiles ( "extra_out_01.img", "extra_ref_01.img" ) );
  assert ( flipImage ( "extra_input_02.img", "extra_out_02.img", true, false )
           && identicalFiles ( "extra_out_02.img", "extra_ref_02.img" ) );
  assert ( flipImage ( "extra_input_03.img", "extra_out_03.img", false, true )
           && identicalFiles ( "extra_out_03.img", "extra_ref_03.img" ) );
  assert ( flipImage ( "extra_input_04.img", "extra_out_04.img", true, false )
           && identicalFiles ( "extra_out_04.img", "extra_ref_04.img" ) );
  assert ( flipImage ( "extra_input_05.img", "extra_out_05.img", false, true )
           && identicalFiles ( "extra_out_05.img", "extra_ref_05.img" ) );
  assert ( flipImage ( "extra_input_06.img", "extra_out_06.img", true, false )
           && identicalFiles ( "extra_out_06.img", "extra_ref_06.img" ) );
  assert ( flipImage ( "extra_input_07.img", "extra_out_07.img", false, true )
           && identicalFiles ( "extra_out_07.img", "extra_ref_07.img" ) );
  assert ( flipImage ( "extra_input_08.img", "extra_out_08.img", true, false )
           && identicalFiles ( "extra_out_08.img", "extra_ref_08.img" ) );
  assert ( flipImage ( "extra_input_09.img", "extra_out_09.img", false, true )
           && identicalFiles ( "extra_out_09.img", "extra_ref_09.img" ) );
  assert ( flipImage ( "extra_input_10.img", "extra_out_10.img", true, false )
           && identicalFiles ( "extra_out_10.img", "extra_ref_10.img" ) );
  assert ( flipImage ( "extra_input_11.img", "extra_out_11.img", false, true )
           && identicalFiles ( "extra_out_11.img", "extra_ref_11.img" ) );
  return 0;
}
#endif /* __PROGTEST__ */
