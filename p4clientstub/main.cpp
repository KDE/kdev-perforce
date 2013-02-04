/***************************************************************************
 *   Copyright 2013  Morten Danielsen Volden                               *
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include <iostream>
#include <QString>
#include <QByteArray>

bool validateNumberOfArguments(int argc, char** /*argv*/)
{
	if(argc < 3)
	{
		return false;
	}
		
	return true;
}


int fakeFstatOutput(QString const& filepath)
{
	std::string fileText = filepath.toUtf8().constData();
	/// This is the output of d:\projects\tools\test>p4 fstat new_file.txt
	std::cout << "... depotFile /" << fileText << std::endl;// /tools/test/new_file.txt" 
	std::cout << "... clientFile /home/projects" << fileText << std::endl;
	std::cout << "... isMapped" << std::endl;
	std::cout << "... headAction add" << std::endl;
	std::cout << "... headType text" << std::endl;
	std::cout << "... headTime 1285014691" << std::endl;
	std::cout << "... headRev 1" << std::endl;
	std::cout << "... headChange 759253" << std::endl;
	std::cout << "... headModTime 1285014680" << std::endl;
	std::cout << "... haveRev 1" << std::endl;
	return 0;
}

int fakeRevertOutput()
{
	return 0;
}

int fakeSyncOutput()
{
	return 0;
}

int fakeSubmitOutput()
{
	return 0;
}

int fakeDiff2Output()
{
	return 0;
}

int fakeDiffOutput()
{
	return 0;
}

int fakeFileLogOutput()
{
	return 0;
}

int fakeAnnotateOutput()
{
	return 0;
}

int fakeEditOutput()
{
	return 0;
}

int main(int argc, char** argv)
{
	if(!validateNumberOfArguments(argc, argv)) {
		std::cout << "Was not able to validate number of arguments: " << argc << std::endl;
		return -1;
	}

	if (qstrcmp(argv[1], "revert") == 0)  {
		return fakeRevertOutput();
	} else if(qstrcmp(argv[1], "sync") == 0) {
		return fakeSyncOutput();
	} else if(qstrcmp(argv[1], "submit") == 0) {
		return fakeSubmitOutput();
	} else if(qstrcmp(argv[1], "diff2") == 0) {
		return fakeDiff2Output();
	} else if(qstrcmp(argv[1], "diff") == 0) {
		return fakeDiffOutput();
	} else if(qstrcmp(argv[1], "filelog") == 0) {
		return fakeFileLogOutput();
	} else if(qstrcmp(argv[1], "annotate") == 0) {
		return fakeAnnotateOutput();
	} else if(qstrcmp(argv[1], "edit") == 0) {
		return fakeEditOutput();
	} else if(qstrcmp(argv[1], "fstat") == 0) {
		return fakeFstatOutput(QString(argv[2]));
	} 
 	return -1;
}