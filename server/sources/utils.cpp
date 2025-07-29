#include "utils.h"


void Utils::EncryptYourString(std::string & str,
const std::string & key, Encode encodeMethod){
    switch (encodeMethod)
    {
    case Encode::Simple:
        str = (*key.begin()) + str + (*key.rbegin());
        break;
    case Encode::Normal:
        break;
    case Encode::Hard:
        break;
    case Encode::Ultra:
        break;
    default:
        break;
    }
}


void Utils::DecodeYourString(std::string & str,
const std::string & key, Encode decodeMethod){
    switch (decodeMethod)
    {
    case Encode::Simple:
        if(str.size() > 2){
            str.erase(str.size() - 1 , 1);
            str.erase(0,1);
        }else str.clear();
        break;
    case Encode::Normal:
        break;
    case Encode::Hard:
        break;
    case Encode::Ultra:
        break;
    default:
        break;
    }
}

