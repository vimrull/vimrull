#pragma once
#undef UNICODE
#define UNICODE
#undef _WINSOCKAPI_
#define _WINSOCKAPI_

//void sha256(const char * data, BYTE ** hash, long * len);
void sha256(const char * data, char ** hash, long * len);