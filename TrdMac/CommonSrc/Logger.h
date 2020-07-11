#ifndef LOGGER_H_
#define LOGGER_H_


class CLogger
	{
public:	
	static TInt iLogIndent;
	
private:
	static BOOL iFirstTimeLoad;
	FILE*  iFile;
	
public:
	CLogger(const char* aFileName);

	CLogger();
	
	~CLogger();

	BOOL Open(const char* aFileName);
	void Log(const char* sLine);

	};

#endif
