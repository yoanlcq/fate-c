/*
- Do not use wchar_t or std::wstring in any place other than adjacent 
point to APIs accepting UTF-16.
- Do not use _T("") or L"" literals in any place other than parameters to 
APIs accepting UTF-16.
- Do not use types, functions, or their derivatives that are sensitive to the 
UNICODE constant, such as LPTSTR, CreateWindow() or the _T()macro. Instead, use LPWSTR, 
CreateWindowW() and explicit L"" literals.
- Yet, UNICODE and _UNICODE are always defined, to avoid passing narrow UTF-8 
strings to ANSI WinAPI getting silently compiled. This can be done by VS project 
settings, under code name Use Unicode character set.
- std::string and char* variables are considered UTF-8, anywhere in the program.
If you have the privilege of writing in C++, the narrow()/widen() conversion functions 
below can be handy for inline conversion syntax. Of course, any other UTF-8/UTF-16 
coversion code would do.
- Only use Win32 functions that accept widechars (LPWSTR), never those which accept 
LPTSTR or LPSTR. Pass parameters this way:
	SetWindowTextW(widen(someStdString or "string litteral").c_str())
	
- Always produce text output files in UTF-8.
- Using fopen() should anyway be avoided for RAII/OOD reasons.
However, if necessary, use _wfopen() and WinAPI conventions as described above.
- Never pass std::string or const char* filename arguments to the fstream family. 
MSVC CRT does not support UTF-8 arguments, but it has a non-standard extension 
which should be used as follows:
- Convert std::string arguments to std::wstring with widen:
	std::ifstream ifs(widen("hello"), std::ios_base::binary);
We will have to manually remove the conversion, when MSVC’s attitude to fstream 
changes.
- This code is not multi-platform and may have to be changed manually in the future.
- Alternatively use a set of wrappers that hide the conversions.
*/

#include <windows.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
/*
int _tmain(int argc, char *argv[]) {
	int char_cnt_plus_null 
		= MultiByteToWideChar(CP_UTF8, 0, "Śộмẽ şặოрĺẹ ΆŚÇĬĪ-ťēχŧ\n", -1, NULL, 0);
	
	uint16_t *wstr = malloc(char_cnt_plus_null*2);
	//SetConsoleOutputCP(CP_UTF8);
	free(wstr);
	//WideCharToMultiByte()
}
*/
/*
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
               LPSTR lpCmdLine, int CmdShow) {
*/
void test_console(void) {
	HANDLE std = GetStdHandle(STD_OUTPUT_HANDLE);    
	WriteConsoleW(std, L"Śộмẽ şặოрĺẹ ΆŚÇĬĪ-ťēχŧ\n", 
		wcslen(L"Śộмẽ şặოрĺẹ ΆŚÇĬĪ-ťēχŧ\n"), NULL, NULL);
	CloseHandle(std);
}

void test_messagebox(void) {
    MessageBoxW(NULL, L"Śộмẽ şặოрĺẹ ΆŚÇĬĪ-ťēχŧ\n", L"Title", MB_OK);
	
	int char_cnt_plus_null 
		= MultiByteToWideChar(CP_UTF8, 0, u8"Śộмẽ şặოрĺẹ ΆŚÇĬĪ-ťēχŧ\n", -1, NULL, 0);
	uint16_t *wstr = malloc(char_cnt_plus_null*2);
	MultiByteToWideChar(CP_UTF8, 0, u8"Śộмẽ şặოрĺẹ ΆŚÇĬĪ-ťēχŧ\n", -1, 
		wstr, char_cnt_plus_null);
	MessageBoxW(NULL, wstr, L"Title", MB_OK);
	free(wstr);
}

void test_file(void) {
	FILE *f = _wfopen(L"Śộмẽ şặოрĺẹ ΆŚÇĬĪ-ťēχŧ.txt", L"w");
	fwrite(u8"Śộмẽ şặოрĺẹ ΆŚÇĬĪ-ťēχŧ\n", 1, strlen(u8"Śộмẽ şặოрĺẹ ΆŚÇĬĪ-ťēχŧ\n"), f);
	fclose(f);
}

int main(void) {			   	
	test_messagebox();
    return 0;
}