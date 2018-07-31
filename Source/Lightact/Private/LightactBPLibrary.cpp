// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "LightactBPLibrary.h"
#include "Lightact.h"

#define RAPIDJSON_HAS_STDSTRING 1
#include "rapidjson/writer.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "StringConv.h"


//#include <Windows.h>
#include "Windows/MinWindows.h"
//#include <conio.h>
//#include <conio.h>



ULightactBPLibrary::ULightactBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

/* Reads a JSON string from shared memory file and outputs a map of strings to strings. */
void ULightactBPLibrary::BuildMap(const FString HandleName, const int HandleSize, TMap<FString, FString>& resultMap, bool& success) {

	resultMap.Empty();

	HANDLE hMapFile;
	LPCTSTR pBuf;

	hMapFile = OpenFileMapping(
		FILE_MAP_READ, // read access
		FALSE, // do not inherit the name
		(LPCWSTR)*HandleName);

	if (hMapFile != NULL) {

		pBuf = (LPTSTR)MapViewOfFile(hMapFile, // handle to map object
			FILE_MAP_READ, // read permission
			0, // offset hi
			0, // offset lo
			HandleSize);

		if (pBuf != NULL) {

			std::string mConvert((char*)pBuf);
			UnmapViewOfFile(pBuf);

			rapidjson::Document document;
			if (!document.Parse(mConvert.c_str()).HasParseError()) {

				for (rapidjson::Value::ConstMemberIterator itr = document.MemberBegin();
					itr != document.MemberEnd(); ++itr) {
					resultMap.Add(itr->name.GetString(), itr->value.GetString());
				}
				CloseHandle(hMapFile);
				success = true;

			} else {
				resultMap.Add("Error", "Has Parse Error");
				CloseHandle(hMapFile);
				success = false;
			}
		} else {
			resultMap.Add("Error", "Could not map view of file");
			CloseHandle(hMapFile);
			success = false;
		}
	} else {
		resultMap.Add("Error", "Could not open file mapping object");
		success = false;
	}
}

void ULightactBPLibrary::createMemHandle(const FString HandleName, const int HandleSize, FString& error, bool& success) {
	HANDLE hMapFile;

	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		HandleSize,              // maximum object size (low-order DWORD)
		(LPCWSTR)*HandleName);   // name of mapping object

	if (hMapFile != NULL) {
		success = true;
	}
	else {
		error = "Could not open file mapping object";
		success = false;
	}
}

void ULightactBPLibrary::writeSharedMemory(TMap<FString, FString> stringMap, const FString HandleName, const int HandleSize, FString& error, bool& success) {
	HANDLE hMapFile;
	LPCTSTR pBuf;
	
hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,		// read/write access
		FALSE,						// do not inherit the name
		(LPCWSTR)*HandleName);      // name of mapping object

	if (hMapFile == NULL)
	{
		error = "Could not open file mapping object.";
		success=false;
		return;
	}
	pBuf = (LPTSTR)MapViewOfFile(hMapFile,		// handle to map object
		FILE_MAP_ALL_ACCESS,					// read/write permission
		0,
		0,
		HandleSize);
	if (pBuf == NULL)
	{
		error = "Could not map view of file.";
		CloseHandle(hMapFile);
		success = false;
		return;
	}

	//Let's create a rapidjson document
	rapidjson::Document document;
	rapidjson::StringBuffer buffer;
	document.SetObject();
	std::string UEKey;
	std::string UEVal;

	for (auto& Elem : stringMap) { //we go through all the key/value pairs in the string map and add them to JSON document
		UEKey = std::string(TCHAR_TO_UTF8(*Elem.Key));
		UEVal = std::string(TCHAR_TO_UTF8(*Elem.Value));
		document.AddMember(
			rapidjson::Value(UEKey,document.GetAllocator()).Move(),
			rapidjson::Value(UEVal,document.GetAllocator()).Move(),
			document.GetAllocator());
	}

	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer); //we write them to JSON document
	document.Accept(writer);

	//some conversions
	std::string jsonString = buffer.GetString();
	//std::wstring wstr = std::wstring(jsonString.begin(), jsonString.end());
	//const wchar_t* widecstr = wstr.c_str();
	std::string wstr = std::string(jsonString.begin(), jsonString.end());
	memset((PVOID)pBuf,0, HandleSize);
	memcpy((PVOID)pBuf, wstr.c_str(), wstr.length());
	//CopyMemory((PVOID)pBuf, wstr.c_str(), (wstr.length* sizeof(char))); //and we write it to memory
	//_getch();
	UnmapViewOfFile(pBuf);
	success = true;
}

void ULightactBPLibrary::closeMemHandle(const FString HandleName, FString& error, bool& success) {

	HANDLE hMapFile;
	//LPCTSTR pBuf;
	
	hMapFile = OpenFileMapping(
		FILE_MAP_READ, // read access
		FALSE, // do not inherit the name
		(LPCWSTR)*HandleName);

	if (hMapFile != NULL) {
		CloseHandle(hMapFile);
		success = true;
	}
	else {
		error="Could not open file mapping object.";
		success = false;
	}
	
}

/* Splits string by delimiters and returns a composed 3-D space vector. */
void ULightactBPLibrary::StringExplode(const FString InputString, const FString Delimiters, FVector& Vector) {

	// add default values
	TArray<float> OutConverts;
	OutConverts.Add(0.f);
	OutConverts.Add(0.f);
	OutConverts.Add(0.f);

	TArray<FString> OutSplits;
	int len = InputString.ParseIntoArray(OutSplits, Delimiters.GetCharArray().GetData(), true);
	int minVal;
	if (len > 3)  minVal = 3;
	else  minVal = len;
	//min(len, 3)
	for (int i = 0; i < minVal; i++) {
		OutConverts[i] = FCString::Atof(OutSplits[i].GetCharArray().GetData());
	}
		
	Vector = FVector(OutConverts[0], OutConverts[1], OutConverts[2]);

}
