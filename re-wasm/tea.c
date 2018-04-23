#include "tea.h"
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include <string.h>
static void EncryptBlock(
    uint32_t *v,
    uint32_t *k
);

static uint32_t LoadUint32(
    unsigned char b[4]
);

static void StoreUint32(
    unsigned char dst[4],
    uint32_t n
);


static void EncryptBlock(
    uint32_t *v,
    uint32_t *k
) {
    /* item 22 */
    uint32_t delta = 0x9e3779b9, sum = 0, i;
    /* item 180001 */
    i = 0;
    
    
    while (i < 32) {
        /* item 19 */
        sum += delta;
        /* item 21 */
        v[0] += ((v[1]<<3) ^ k[0]) ^ (v[1] + sum) ^ ((v[1]>>5) + k[1]);
        /* item 34 */
        v[1] += ((v[0]<<3) ^ k[2]) ^ (v[0] + sum) ^ ((v[0]>>5) + k[3]);
        /* item 180003 */
        i++;
        
    } 
        return;
    
    
}

static uint32_t LoadUint32(
    unsigned char b[4]
) {
    /* item 45 */
    return (uint32_t)b[0] | (uint32_t)b[1]<<8 | (uint32_t)b[2]<<16 | (uint32_t)b[3]<<24;
    
}

static void StoreUint32(
    unsigned char dst[4],
    uint32_t n
) {
    /* item 51 */
    dst[0] = n;
    dst[1] = n >> 8;
    dst[2] = n >> 16;
    dst[3] = n >> 24;
    return;
    
}



void EncryptCBC(
    unsigned char *dst,
    unsigned char *src,
    int len,

    unsigned char key[16]
) {
    /* item 109 */
    unsigned char padded[8];
    uint32_t v[2], k[4];
    int i;

    /* item 65 */
    k[0] = LoadUint32(&key[0]);
    k[1] = LoadUint32(&key[4]);
    k[2] = LoadUint32(&key[8]);
    k[3] = LoadUint32(&key[12]);
    
  
    while (len >= 8) {
        /* item 63 */
        v[0] = LoadUint32(&src[0]);
        v[1] = LoadUint32(&src[4]);
        
        EncryptBlock(v, k);
        
        StoreUint32(&dst[0], v[0]);
        StoreUint32(&dst[4], v[1]);
        
        src += 8;
        dst += 8;
        len -= 8;
        
    }
    
}



int check(char *m) {
    /* item 117 */
    
    
    if(strlen(m)!=24){

	return 0;}
    unsigned char key[16] = "webasmintersting";

    unsigned char c[24];
    
    int i;
    
    /* item 197 */
    EncryptCBC(c, m, sizeof(m),  key);
    /* item 210 */
   
    /* item 1980001 */
    i = 0;
    
        
    int s[]={153, 119, 2, 189, 47, 108, 135, 53, 85, 34, 121, 29, 246, 72, 44, 140, 185, 214, 19, 147, 203, 216, 49, 227};
    i=0;
    int j;
    for(;i<3;i++){
	for(j=0;j<8;j++){
		if(c[i*8+j]!=s[i*8+7-j]){
				
			return 0;
		}
	
	}
    }

    return 1;		
   
    
    

}

