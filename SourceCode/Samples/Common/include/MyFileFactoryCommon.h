#if defined(WIN32)
#include <conio.h>
#else
#include "../common/conio.h"
#endif

#include <string.h>
#include <stdio.h>
#include "irrKlang.h"
using namespace irrklang;

#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

// To start, we need to implement the class IFileFactory, which irrKlang uses
// to open files. The interface consists only of one single method named 
// createFileReader(const ik_c8* filename). In this method, we create return 
// our own file access class and return it:

// a class implementing the IFileFactory interface to override irrklang file access
class CMyFileFactory : public IFileFactory
{
public:

	//! Opens a file for read access. Simply return 0 if file not found.
	virtual IFileReader* createFileReader(const ik_c8* filename)
	{
		printf("MyFileFactory: open file %s\n", filename);

		FILE* file = fopen(filename, "rb");
		if (!file)
			return 0;

		return new CMyReadFile(file, filename);
	}

protected:

	// To write our own file access methods returned in the method above,
	// we only need to implement the IFileReader interface, which has some
	// standard methods like read(), seek(), getPos() etc. In this example
	// we simply use fopen, fread, fseek etc and print to the console 
	// when we are reading or seeking:

	// an own implementation of IReadFile to overwrite read access to files 
	class CMyReadFile : public IFileReader
	{
	public:

		// constructor, store size of file and filename
		CMyReadFile(FILE* openedFile, const ik_c8* filename)
		{
			File = openedFile;
			strcpy(Filename, filename);

			// get file size
			fseek(File, 0, SEEK_END);
			FileSize = ftell(File);
			fseek(File, 0, SEEK_SET);
		}

		~CMyReadFile()
		{
			fclose(File);
		}

		//! reads data, returns how much was read
		ik_s32 read(void* buffer, ik_u32 sizeToRead)
		{
			printf("CMyReadFile: read %d bytes\n", sizeToRead);
			return (ik_s32)fread(buffer, 1, sizeToRead, File);
		}

		//! changes position in file, returns true if successful
		bool seek(ik_s32 finalPos, bool relativeMovement)
		{
			printf("CMyReadFile: seek to position %d\n", finalPos);
			return fseek(File, finalPos, relativeMovement ? SEEK_CUR : SEEK_SET) == 0;
		}

		//! returns size of file
		ik_s32 getSize()
		{
			return FileSize;
		}

		//! returns where in the file we are.
		ik_s32 getPos()
		{
			return ftell(File);
		}

		//! returns name of file
		const ik_c8* getFileName()
		{
			return Filename;
		}

		FILE* File;
		char Filename[1024];
		ik_s32 FileSize;

	}; // end class CMyReadFile

}; // end class CMyFileFactory