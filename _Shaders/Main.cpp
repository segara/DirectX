#include <stdio.h>
#include <string>
#include <assert.h>
#include <d3dcommon.h>
#include <d3d10effect.h>
#include <d3dcompiler.h>
using namespace std;

#pragma comment(lib, "d3dcompiler.lib")

int Compile(string src, string dest)
{
	DWORD fileValue = GetFileAttributesA(src.c_str());
	if (fileValue >= 0xFFFFFFFF)
	{
		printf("%s 파일 찾을 수 없음 - 실패\n", src.c_str());
		
		return 0;
	}
		

	ID3DBlob* error;
	INT flag = D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY | D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;

	wstring temp = L"";
	temp.assign(src.begin(), src.end());

	ID3DBlob* result;
	HRESULT hr = D3DCompileFromFile(temp.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, NULL, "fx_5_0", flag, NULL, &result, &error);
	if (FAILED(hr))
	{
		if (error != NULL)
			printf("%s", (const char *)error->GetBufferPointer());

		return 0;
	}


	HANDLE fileHandle = CreateFileA(dest.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	bool bChecked = fileHandle != INVALID_HANDLE_VALUE;
	if (bChecked == false)
	{
		printf("%s 파일 생성 - 실패\n", dest.c_str());

		return 0;
	}

	DWORD size = 0;
	WriteFile(fileHandle, result->GetBufferPointer(), result->GetBufferSize(), &size, NULL);

	CloseHandle(fileHandle);
	fileHandle = NULL;
	
	return 1;
}

//0 - 실행파일 경로
//1 - 옵션(/F : 파일, /D : 폴더)
//2 - 폴더 경로
//3 - 파일이름
//4 - 파일확장자
int main(int count, char* args[])
{
	string option = args[1];

	if (count < 2 || count > 5)
	{
		printf("명령줄 인수 잘못됨 - 실패\n");

		return 0;
	}

	if (option == "/F" || option == "/f")
	{
		if (count != 5)
		{
			printf("/F 옵션의 명령줄 인수 잘못됨 - 실패\n");

			return 0;
		}

		if (string(args[4]) != ".fx")
		{
			printf("%s%s%s 이펙트 파일 아님 - 실패\n", args[2], args[3], args[4]);

			return 0;
		}

		string src = string(args[2]) + string(args[3]) + string(args[4]);
		string dest = string(args[2]) + string(args[3]) + ".fxo";
		
		printf("---> 컴파일 시작 : 파일: %s \n", src.c_str());


		int result  = Compile(src, dest);
		if (result == 1)
		{
			printf("1> %s%s -> %s\n", args[3], args[4], dest.c_str());
			printf("==================== 컴파일: 성공 1, 실패 0 ====================\n");

			return 0;
		}
		else
		{
			printf("==================== 컴파일: 성공 0, 실패 1 ====================\n");
		}

		return 0;

	}
	else if (option == "/D" || option == "/d")
	{
		string folder = string(args[2]) + "\\*.*";

		WIN32_FIND_DATAA findData;
		HANDLE folderHandle = FindFirstFileA(folder.c_str(), &findData);

		if (folderHandle == INVALID_HANDLE_VALUE)
		{
			printf("%s 경로가 잘못됨 - 실패\n", args[2]);

			return 0;
		}


		UINT succeed = 0;
		UINT failed = 0;

		while (true)
		{
			string file = findData.cFileName;
			string src = "";
			string dest = "";

			UINT index = file.find_last_of('.');
			string ext = file.substr(index + 1, file.length());

			if (file.length() > 0 && file != "." && file != ".." && ext == "fx")
			{
				src = string(args[2]) + "\\" + string(findData.cFileName);
				dest = src + "o";

				int result = Compile(src, dest);
				
				result ? ++succeed : ++failed;
			}

			if (FindNextFileA(folderHandle, &findData) == FALSE)
				break;
		}

		printf("==================== 컴파일: 성공 %d, 실패 %d ====================\n", succeed, failed);
	}
	else
	{
		printf("옵션은 /D나 /F를 사용해야 합니다 : %s\n", option.c_str());

		return 0;
	}

	return 0;
}