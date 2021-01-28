/*******************************************************
* FileName: base64.cpp
*
************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * determine the value of a base64 encoding character
 *
 * @param base64char the character of which the value is searched
 * @return the value in case of success (0-63), -1 on failure
 */  
int _base64_char_value(char base64char)
 {
    if (base64char >= 'A' && base64char <= 'Z')
 return base64char-'A';
    if (base64char >= 'a' && base64char <= 'z')
 return base64char-'a'+26;
    if (base64char >= '0' && base64char <= '9')
 return base64char-'0'+2*26;
    if (base64char == '+')
 return 2*26+10;
    if (base64char == '/')
 return 2*26+11;
    return -1;
} 

/**
 * decode a 4 char base64 encoded byte triple
 *
 * @param quadruple the 4 characters that should be decoded
 * @param result the decoded data
 * @return lenth of the result (1, 2 or 3), 0 on failure
 */  
int _base64_decode_triple(char quadruple[4], unsigned char *result)
 {
    int i, triple_value, bytes_to_decode = 3, only_equals_yet = 1;
    int char_value[4];

    for (i=0; i<4; i++)
 char_value[i] = _base64_char_value(quadruple[i]);

    /* check if the characters are valid */
    for (i=3; i>=0; i--)
    {
 if (char_value[i]<0)
 {
     if (only_equals_yet && quadruple[i]=='=')
     {
  /* we will ignore this character anyway, make it something
   * that does not break our calculations */
  char_value[i]=0;
  bytes_to_decode--;
  continue;
     }
     return 0;
 }
 /* after we got a real character, no other '=' are allowed anymore */
 only_equals_yet = 0;
    }

    /* if we got "====" as input, bytes_to_decode is -1 */
    if (bytes_to_decode < 0)
 bytes_to_decode = 0;

    /* make one big value out of the partial values */
    triple_value = char_value[0];
    triple_value *= 64;
    triple_value += char_value[1];
    triple_value *= 64;
    triple_value += char_value[2];
    triple_value *= 64;
    triple_value += char_value[3];

    /* break the big value into bytes */
    for (i=bytes_to_decode; i<3; i++)
 triple_value /= 256;
    for (i=bytes_to_decode-1; i>=0; i--)
    {
 result[i] = triple_value%256;
 triple_value /= 256;
    }

    return bytes_to_decode;
} 

/**
 * decode base64 encoded data
 *
 * @param source the encoded data (zero terminated)
 * @param target pointer to the target buffer
 * @param targetlen length of the target buffer
 * @return length of converted data on success, -1 otherwise
 */  
int Decode64(char *source, unsigned char *target, size_t targetlen)
{
    char *src, *tmpptr;
    char quadruple[4], tmpresult[3];
    int i, tmplen = 3;
    size_t converted = 0;

    /* concatinate '===' to the source to handle unpadded base64 data */
    src = (char *)malloc(strlen(source)+5);
    if (src == NULL)
 return -1;
    strcpy(src, source);
    strcat(src, "====");
    tmpptr = src;

    /* convert as long as we get a full result */
    while (tmplen == 3)
    {
 /* get 4 characters to convert */
 for (i=0; i<4; i++)
 {
     /* skip invalid characters - we won't reach the end */
     while (*tmpptr != '=' && _base64_char_value(*tmpptr)<0)
  tmpptr++;

     quadruple[i] = *(tmpptr++);
 }

 /* convert the characters */
 tmplen = _base64_decode_triple(quadruple, (unsigned char *)tmpresult);

 /* check if the fit in the result buffer */
 if (targetlen < tmplen)
 {
     free(src);
     return -1;
 }

 /* put the partial result in the result buffer */
 memcpy(target, tmpresult, tmplen);
 target += tmplen;
 targetlen -= tmplen;
 converted += tmplen;
    }

    free(src);
    return converted;
}


/**************************************************************************************************************************************************************************************************
*Proc Name: Encode
*Input: character to be encoded
*Output: char that has encoded.
*processing: converted according to rule of base64 encoding.
*
*
*************************************************************************************************************************************************************************************************************/
#undef __FNNAME__
#define __FNNAME__	"Encode"

char Encode(unsigned char ch)			//input is the char that has to be encoded.
{
	if(ch < 26)										//if char is less than 26 encode it to alphabet Uppercase.
		return 'A'+ch;
	if(ch <52 )										//encode to lower case alphabet.
		return 'a'+ (ch-26);
	if(ch < 62)										//encode to digit
		return '0'+ (ch-52);
	if(ch==62)
		return '+';
	return '/';
}


int Encode64(char* pszbuffer, int iSz, char *pszOut)
{
	int i,j=0;													//i will be used as incrementer from file ,while j will be used as incrementer in buffer.
	unsigned char byte1,byte2,byte3;							//temp storage for three bytes that we will read 
	unsigned char tempbyte;										//used to hold byte after encoding
	int length = iSz; //strlen(pszbuffer);
	
	
	
	for(i=0; i <length-2 ;i=i+3)								//read until one,zero,two bytes are remaining
	{
//		if((i%3000==0)&& (i!=0))								//replace 3000 with 57			//if we did not want to send packet of 76 char then it can be removed	
//		{
//			//strcat(buffer,"\r\n");								//append \r\n
//			WriteToFile(buffer);		
//			::ZeroMemory(buffer,sizeof(buffer));				//reinitialise buffer.
//			j=0;												//reinitialise j(buffer incrementor)
//		}
		
		byte1=*(pszbuffer+i);
		byte2=*(pszbuffer+i+1);
		byte3=*(pszbuffer+i+2);

		tempbyte=byte1 >> 2;									//prepare first byte
		tempbyte=Encode(tempbyte);								//encode it
		pszOut[j]=tempbyte;										//copy it to buffer
		j++;		
		tempbyte=((byte1 & 0x3)<<4)|(byte2 >> 4);				//prepare second byte
		tempbyte=Encode(tempbyte);								//encode it
		pszOut[j]=tempbyte;										//copy it to buffer
		j++;
		tempbyte=((byte2& 0xf ) << 2 )|(byte3 >> 6);			//prepare third byte
		tempbyte=Encode(tempbyte);								//encode it.
		pszOut[j]=tempbyte;										//copy it to buffer
		j++;
		tempbyte= byte3 & 0x3f;									//prepare fourth byte.
		tempbyte=Encode(tempbyte);								//encode it.
		pszOut[j]=tempbyte;										//put it to buffer
		j++;
	}
	
	pszOut[j] = '\0';
	if((length-i)==0)											//case 1: all bytes of file has been read.
	{
	
		return  0;
	}
	else if(length-i==1)										//case 2:one more byte is reamined to read
	{
							
		byte1=*(pszbuffer+i);									//reads remaing byte.
		tempbyte=byte1>>2;										//prepare first byte
		tempbyte=Encode(tempbyte);	
		pszOut[j]=tempbyte;
		
		tempbyte=((byte1 & 0x3)<<4);
		tempbyte=Encode(tempbyte);
		pszOut[j+1]=tempbyte;
		
		pszOut[j+2]= '=';										//append '='at last
		
		pszOut[j+3]= '=';										//append '='at last
		
		pszOut[j+4] = '\0';
	}
	else														//case 3: two bytes are remaining to read,length-i==1
	{
		byte1=*(pszbuffer+i);
		byte2=*(pszbuffer+i+1);
		
		tempbyte=byte1>>2;										//prepare first byte
		tempbyte=Encode(tempbyte);
		pszOut[j]=tempbyte;	
		
		tempbyte=((byte1 & 0x3)<<4)|(byte2 >> 4);				//prepare second byte
		tempbyte=Encode(tempbyte);
		pszOut[j+1]=tempbyte;

		tempbyte=((byte2& 0xf ) << 2 );
		tempbyte=Encode(tempbyte);
		pszOut[j+2]=tempbyte;
				
		pszOut[j+3]='=';										//append '=' at last as padding byte.
		pszOut[j+4]='\0';										//append '=' at last as padding byte.
	}

	return strlen(pszOut);
}

/*************** end ******************************/ 
