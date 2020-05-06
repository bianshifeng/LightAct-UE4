// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "LightActBPLibrary.generated.h"

/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/
UCLASS()
class ULightActBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

		UFUNCTION(BlueprintCallable, meta = (DisplayName = "Shared memory JSON to string map", Keywords = "LightAct shared memory JSON to string map", ToolTip = "Reads a JSON string from shared memory file and outputs a map of strings to strings."), Category = "LightAct")
		static void BuildMap(const FString HandleName, const int HandleSize, TMap<FString, FString>& ResultMap, bool& Success);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Open shared memory", Keywords = "Open shared memory handle JSON", ToolTip = "Opens shared memory handle. Usually connected to OnBeginPlay. Handle has to be closed on EndPlay."), Category = "LightAct")
		static void createMemHandle(const FString HandleName, const int HandleSize, FString& error, bool& Success);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Write to shared memory", Keywords = "Write string map to LightAct shared memory JSON", ToolTip = "Converts a string map to JSON and writes it to shared memory"), Category = "LightAct")
		static void writeSharedMemory(TMap<FString, FString> stringMap, const FString HandleName, const int HandleSize, FString& error, bool& success);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Close shared memory", Keywords = "Close shared memory handle JSON", ToolTip = "Closes shared memory handle. Usually connected to OnEndPlay. Handle has to be opened before."), Category = "LightAct")
		static void closeMemHandle(const FString HandleName, FString& error, bool& Success);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "String to Vector", Keywords = "LightAct string explode to vector", ToolTip = "Splits string by delimiters and returns a composed 3-D space vector."), Category = "LightAct")
		static void stringToVector(const FString InputString, const FString Delimiters, FVector& Vector);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "String to Vector Array", Keywords = "LightAct string explode to vector array", ToolTip = "Splits string by delimiters and returns an array of 3-D space vectors."), Category = "LightAct")
		static void stringToVectorArray(const FString InputString, const FString ComponentDelimiter, const FString VectorDelimiter, TArray<FVector>& Vector);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "String to Integer Array", Keywords = "LightAct string explode to integer array", ToolTip = "Splits string by delimiters and returns an array of integers."), Category = "LightAct")
		static void stringToIntArray(const FString InputString, const FString Delimiter, TArray<int>& IntArr);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "String to Float Array", Keywords = "LightAct string explode to float array", ToolTip = "Splits string by delimiters and returns an array of floats."), Category = "LightAct")
		static void stringToFloatArray(const FString InputString, const FString Delimiter, TArray<float>& FloatArr);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Extrude Contours", Keywords = "LightAct extrude contours vector array", ToolTip = "Extrudes contours and creates array of vertices and triangles. Connect these to Create Mesh Section."), Category = "LightAct")
		static void extrudeContours(TArray<FVector> Contours, float height, float ScaleX, float ScaleY, TArray<FVector>& Vertices, TArray<int32>& Triangles);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "LightAct Tick", Keywords = "LightAct process heartbeat", ToolTip = "Creates a LightAct tick heartbeat value."), Category = "LightAct")
		static void lightactProcessTick(int& Heartbeat);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "openFileDialog", Keywords = "Open File Dialog", ToolTip = "Opens a file dialog and returns pointer to it"), Category = "FileDialog")
		static void openFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& FileTypes, TArray<FString>& OutFileNames);

};
