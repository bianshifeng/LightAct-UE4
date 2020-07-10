// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "LightActBPLibrary.h"
#include "LightAct.h"

#define RAPIDJSON_HAS_STDSTRING 1
#include "../rapidjson/writer.h"
#include "../rapidjson/rapidjson.h"
#include "../rapidjson/document.h"
#include "../rapidjson/stringbuffer.h"

//#define WIN32_LEAN_AND_MEAN
#include "Windows/MinWindows.h"
#include "time.h"
#include "Containers/StringConv.h"
#include <string>

#include "Framework/Application/SlateApplication.h"
#include "DesktopPlatform/Public/DesktopPlatformModule.h"


ULightActBPLibrary::ULightActBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

/* Reads a JSON string from shared memory file and outputs a map of strings to strings. */
void ULightActBPLibrary::BuildMap(const FString HandleName, const int HandleSize, TMap<FString, FString>& resultMap, bool& success) {

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

			}
			else {
				resultMap.Add("Error", "Has Parse Error");
				CloseHandle(hMapFile);
				success = false;
			}
		}
		else {
			resultMap.Add("Error", "Could not map view of file");
			CloseHandle(hMapFile);
			success = false;
		}
	}
	else {
		resultMap.Add("Error", "Could not open file mapping object");
		success = false;
	}
}

void ULightActBPLibrary::createMemHandle(const FString HandleName, const int HandleSize, FString& error, bool& success) {
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

void ULightActBPLibrary::writeSharedMemory(TMap<FString, FString> stringMap, const FString HandleName, const int HandleSize, FString& error, bool& success) {
	HANDLE hMapFile;
	LPCTSTR pBuf;

	hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,		// read/write access
		FALSE,						// do not inherit the name
		(LPCWSTR)*HandleName);      // name of mapping object

	if (hMapFile == NULL)
	{
		error = "Could not open file mapping object.";
		success = false;
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
			rapidjson::Value(UEKey, document.GetAllocator()).Move(),
			rapidjson::Value(UEVal, document.GetAllocator()).Move(),
			document.GetAllocator());
	}

	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer); //we write them to JSON document
	document.Accept(writer);

	//some conversions
	std::string jsonString = buffer.GetString();
	//std::wstring wstr = std::wstring(jsonString.begin(), jsonString.end());
	//const wchar_t* widecstr = wstr.c_str();
	std::string wstr = std::string(jsonString.begin(), jsonString.end());
	memset((PVOID)pBuf, 0, HandleSize);
	memcpy((PVOID)pBuf, wstr.c_str(), wstr.length());
	//CopyMemory((PVOID)pBuf, wstr.c_str(), (wstr.length* sizeof(char))); //and we write it to memory
	//_getch();
	UnmapViewOfFile(pBuf);
	success = true;
}

void ULightActBPLibrary::closeMemHandle(const FString HandleName, FString& error, bool& success) {

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
		error = "Could not open file mapping object.";
		success = false;
	}

}

/* Splits string by delimiters and returns a composed 3-D space vector. */
void ULightActBPLibrary::stringToVector(const FString InputString, const FString Delimiters, FVector& Vector) {

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

/* Splits string by delimiters and returns an array of 3-D space vectors. */
void ULightActBPLibrary::stringToVectorArray(const FString InputString, const FString ComponentDelimiter, const FString VectorDelimiter, TArray<FVector>& VArray) {

	// add default values
	TArray<float> OutConverts;
	OutConverts.Add(0.f);
	OutConverts.Add(0.f);
	OutConverts.Add(0.f);

	TArray<FString> VecSplits;
	int len = InputString.ParseIntoArray(VecSplits, VectorDelimiter.GetCharArray().GetData(), true);
	for (int i = 0; i < len; i++) {
		TArray<FString> CompSplits;
		int len2 = VecSplits[i].ParseIntoArray(CompSplits, ComponentDelimiter.GetCharArray().GetData(), true);
		int minVal;
		if (len2 > 3)  minVal = 3;
		else  minVal = len2;
		//min(len, 3)
		for (int j = 0; j < minVal; j++) {
			OutConverts[j] = FCString::Atof(CompSplits[j].GetCharArray().GetData());
		}
		VArray.Add(FVector(OutConverts[0], OutConverts[1], OutConverts[2]));
	}
}

/* Splits string by delimiters and returns an array of integers. */
void ULightActBPLibrary::stringToIntArray(const FString InputString, const FString Delimiter, TArray<int>& IntArr) {

	if (Delimiter == "")
		return;

	// add default values
	int temp = 0;

	TArray<FString> StringArr;
	int len = InputString.ParseIntoArray(StringArr, Delimiter.GetCharArray().GetData(), true);
	for (int i = 0; i < len; i++) {
		temp = FCString::Atoi(StringArr[i].GetCharArray().GetData());
		IntArr.Add(temp);
	}
}

/* Splits string by delimiters and returns an array of floats. */
void ULightActBPLibrary::stringToFloatArray(const FString InputString, const FString Delimiter, TArray<float>& FloatArr) {

	if (Delimiter == "")
		return;

	// add default values
	float temp = 0.f;


	TArray<FString> StringArr;
	int len = InputString.ParseIntoArray(StringArr, Delimiter.GetCharArray().GetData(), true);
	for (int i = 0; i < len; i++) {
		temp = FCString::Atof(StringArr[i].GetCharArray().GetData());
		FloatArr.Add(temp);
	}
}

/*Extrudes contours*/
void ULightActBPLibrary::extrudeContours(TArray<FVector> Contours, float height, float ScaleX, float ScaleY, TArray<FVector>& Vertices, TArray<int32>& Triangles) {

	float currCont = 0;
	Vertices.Empty(); //here we store all vertices
	Triangles.Empty();//here we store all triangles
	int lastVert = 0;
	FVector contCoG = FVector(0.0f, 0.0f, 0.0f); //contour Center of Gravity
	TArray<FVector> upperVertices; //upper plane of extruded contour
	TArray<FVector> Vertices1Cont; //vertices of one contour
	TArray<int32> Triangles1Cont; //triangles of one contour

	for (int32 ic = 0; ic != Contours.Num(); ++ic) {

		if (Contours[ic][0] == currCont) { //if we are still in the same contour (we start with contour 0)
			Vertices1Cont.Add(FVector(Contours[ic].Y * ScaleX, Contours[ic].Z * ScaleY, 0.0f));
			contCoG += Vertices1Cont.Last();
			upperVertices.Add(Vertices1Cont.Last() + FVector(0.0f, 0.0f, height));
		}
		if (Contours[ic][0] != currCont || ic >= Contours.Num() - 1) { //if we encounter the next contour or reach the end
			Vertices1Cont.Add(contCoG / Vertices1Cont.Num());
			upperVertices.Add(Vertices1Cont.Last() + FVector(0.0f, 0.0f, height));
			Vertices1Cont.Append(upperVertices); //append upper vertices to lower
			upperVertices.Empty(); //and empty upperVertices so its ready for next iteration


			int contPoints = Vertices1Cont.Num() / 2; //number of points in the contour

			// we initialize the Triangles1Cont array
			// it holds all triangles of one contour
			Triangles1Cont.Empty();
			Triangles1Cont.SetNum((contPoints - 1) * 12); //lots of triangles for one contour
			int k = 0;
			int j = 0;

			for (int32 i = 0; i != (contPoints - 1) * 3 - 1; ++i) { // we put a condition here just in case

				// if true we do the last 2 vertices per plane and break
				if ((i) == (contPoints - 1) * 3 - 2) {
					Triangles1Cont[i] = 0 + lastVert;
					Triangles1Cont[i + 1] = (contPoints - 1) + lastVert;
					Triangles1Cont[i + (contPoints - 1) * 3] = 2 * contPoints - 2 + lastVert;
					Triangles1Cont[i + (contPoints - 1) * 3 + 1] = 2 * contPoints - 1 + lastVert;
					break;
				}

				//the last point of all triangles on top and bottom plane is CoG
				if ((i + 1) % 3 == 0 && (i - 1) != 0) {
					Triangles1Cont[i] = contPoints - 1 + lastVert;
					Triangles1Cont[i + (contPoints - 1) * 3] = 2 * contPoints - 1 + lastVert;
					++k;
				}
				else {
					Triangles1Cont[i] = i - 2 * k + lastVert;
					Triangles1Cont[i + (contPoints - 1) * 3] = contPoints * 2 - 2 - i + 2 * k + lastVert;


					if (i % 3 == 0) { //here we do the triangles on the sides
						Triangles1Cont[(contPoints - 1 + j) * 6] = i - 2 * j + lastVert;
						if (j < contPoints - 2) {
							Triangles1Cont[(contPoints - 1 + j) * 6 + 1] = i - 2 * j + 1 + contPoints + lastVert;
							Triangles1Cont[(contPoints - 1 + j) * 6 + 2] = i - 2 * j + 1 + lastVert;

							Triangles1Cont[(contPoints - 1 + j) * 6 + 4] = i - 2 * j + contPoints + lastVert;
							Triangles1Cont[(contPoints - 1 + j) * 6 + 5] = i - 2 * j + 1 + contPoints + lastVert;
						}
						else {
							Triangles1Cont[(contPoints - 1 + j) * 6 + 1] = contPoints + lastVert;
							Triangles1Cont[(contPoints - 1 + j) * 6 + 2] = 0 + lastVert;

							Triangles1Cont[(contPoints - 1 + j) * 6 + 4] = i - 2 * j + contPoints + lastVert;
							Triangles1Cont[(contPoints - 1 + j) * 6 + 5] = contPoints + lastVert;
						}

						Triangles1Cont[(contPoints - 1 + j) * 6 + 3] = i - 2 * j + lastVert;
						j++;
					}
				}
			}

			/*Reset things and either break or loop back*/
			contCoG = FVector(0.0f, 0.0f, 0.0f); // reset CoG
			lastVert += 2 * contPoints; //lastVertice from current contour
			currCont = Contours[ic][0];
			Vertices.Append(Vertices1Cont);
			Vertices1Cont.Empty();
			Triangles.Append(Triangles1Cont);
			if (ic >= Contours.Num() - 1)
				break;
			else
				--ic;
		}
	}
}

/* Construct a simple string for Lightact heartbeat signal. It uses current system time. */
void ULightActBPLibrary::lightactProcessTick(int& Value) {

	struct tm timeinfo;
	time_t currTime;
	time(&currTime);

	//timeinfo = localtime(&currTime);
	localtime_s(&timeinfo, &currTime);
	Value = timeinfo.tm_sec + 60 * timeinfo.tm_min + 60 * 60 * timeinfo.tm_hour;

}

void ULightActBPLibrary::openFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& FileTypes, TArray<FString>& OutFileNames)
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (DesktopPlatform)
	{
		// Opening the file picker!
		// A value of 0 represents single file selection while a value of 1 represents multiple file selection

		const void* ParentWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);

		uint32 SelectionFlag = 0;

		DesktopPlatform->OpenFileDialog(ParentWindowHandle, DialogTitle, DefaultPath, FString(""), FileTypes, SelectionFlag, OutFileNames);
	}
}


