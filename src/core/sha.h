#ifndef SHA_H
#define SHA_H

#include <string>
#include <sstream>
#include <openssl/sha.h>
#include <iomanip>

struct Sha {      
    void create(const char* src, uint size) {
	SHA256_CTX sha256;
	unsigned char hash[SHA256_DIGEST_LENGTH];
    
	SHA256_Init(&sha256);  	
	SHA256_Update(&sha256, src, size);    
	SHA256_Final(hash, &sha256);

	std::stringstream ss;

	for(int i = 0; i < SHA256_DIGEST_LENGTH; i++){
	    ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
	}
	
	sha = ss.str();
    }
    
    std::string sha = "";
};

#endif
