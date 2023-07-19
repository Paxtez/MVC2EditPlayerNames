//////////////////////////////////////////////////////
//
// Marvel Vs Capcom 2 Player Name Edit Tool v0.8
//
// Edits the saved Player Names inside a modded
// US Dreamcast Version of Marvel vs Capcom 2
// that was edited by the MVC2 Modding tool
// [ https://www.paxtez.zachd.com ]
//
// Edits 1ST_READ.BIN, .CDIs, .GDI, or .BIN files
// or any other format that keeps the file intact
//
// USAGE:
//       EditNames.exe
// Edits a 1ST_READ.BIN file in the same path as EditNames.exe
//
// USAGE:
//       EditNames.exe "MVC2.CDI"
// Edits any file dragged on to the program .exe
//
// My C/C++ code is very ugly, I learned this bastard
// version of it many years ago, and never unlearned
// it. Beware.
//
// PLAYER NAME DATA FORMAT
// In a 1ST_READ.BIN this may be at 0x1BAA80, but it may
// change.
//
//1234567890ABCDEF
// NEW CHALLANGER.     // Default name
//[ PLAYER NAMES ]     // Target string
//  PLAYER NAME 1.     // Names Can be up to 14 characters
//          NAME2.     // The first character must be a space
//          NAME3.     // The lastt character must be a null
//................     // Names can be skipped
//            NM5.     // Name needs to be right justified
//            NM6.     // with spaces
//     . . .           // Omitted name
// PLAYER NAME 16.     //
//           WINS.     // Win counter for Default Name
//xxxxxxxxxxxxxxxx     // Color data, each byte is for each of the 16 player names, if the color is FF that name will be skipped during the selection
//yzzzzzzzzzzzzzzz     // y and z are space offsets, this is a hex int (0xB = 11). y is for the "WINS" name (0xB)
//z                    // z is for the 16 names in order
//1234567890ABCDEF
//
//////////////////////////////////////////////////////

#define APP_NAME	"Marvel Vs Capcom 2 Player Name Edit Tool"
#define APP_VER		"v1.0"

// Used for cin
#include <iostream>

// Time for the random color //
#include <ctime>

// allows cString.$ shorthand
#define $		c_str()

// How many columns the display will use
#define MAX_COLUMNS 92

// Number of players in the mod
#define NUM_PLAYER_NAMES 16

// The distance between the [ PLAYER NAMES ]
// block and the COLORS
#define NAME_COLOR_OFFSET 288
// Same as above for the padding bytes
#define NAME_SPACE_OFFSET 305
// Number of font colors
#define NUM_COLORS 39

// String shorthand stuff //
using namespace std;

// defaults
string FileName;
int BlockLoc = 0;

FILE *fFile;

// The list of the different color codes //
char colorListCode[NUM_COLORS][3] = {
	"0B",
	"0C",
	"0D",
	"10",
	"12",
	"13",
	"15",
	"17",
	"18",
	"19",
	"1A",
	"1B",
	"1C",
	"1D",
	"20",
	"21",
	"22",
	"23",
	"24",
	"25",
	"26",
	"27",
	"28",
	"29",
	"2A",
	"2B",
	"2C",
	"2D",
	"2E",
	"2F",
	"30",
	"31",
	"32",
	"33",
	"34",
	"35",
	"36",
	"37",
	"38",
};

// The name given to each color code
char colorListName[NUM_COLORS][25] = {
	"Gold Gradient",
	"Dark Blue Gradient",
	"Lime Green Gradient",
	"Gray Gradient",
	"Light Gray Gradient",
	"Green Gradient [Default]",
	"Blue Gradient",
	"Violet",
	"Aqua Blue",
	"Yellow Gradient",
	"Yellow Orange Gradient",
	"Salmon Gradient",
	"Redish Magenta Gradient",
	"White",
	"Pink Red",
	"Cyan",
	"Golden Yellow",
	"Bright Orange",
	"ArtyClick Red",
	"Bright Neon Pink",
	"Bright Lavender",
	"Periwinkle",
	"Washed Out Green",
	"Light Greenish Blue",
	"Pale Turquoise",
	"Baby Blue",
	"Periwinkle Blue",
	"Light Purple",
	"Neon Pink",
	"Razzle Dazzle Rose",
	"Neon Pink",
	"Radical Red",
	"Orange Peel",
	"Rubber Ducky Yellow",
	"White/Yellow Gradient",
	"White/Red Gradient",
	"White/Purple Gradient",
	"White/Cyan Gradient",
	"Ochre",
};

// Prints a string with an acsii box around it //
// Text = string to frame
// Head / Foot = Display a line above/below the text
// Center
// CurCol = What column is this
// TotalCol = How many total columns
void Frame(string Text = "", int Head = 1,	int Foot = 1, int Cent = 1, int CurCol = 1, int TotalCol = 1)
{
	char tl = (char)201; // Top left corner
	char tb = (char)205; // Up/down bar
	char tr = (char)187; // Top right corner
	char lr = (char)186; // left/right bar
	char bl = (char)200; // Bottom left corner
	char br = (char)188; // Bottom right corner

	// How much space inbetween edge and the frame
	string BoxPad = " ";
	string Bar = "";

	// Some error checks about the columns //
	if (TotalCol < 1)
	{
		TotalCol = 1;
	}
	if (CurCol < 1)
	{
		CurCol = 1;
	}
	if (CurCol > TotalCol)
	{
		CurCol = TotalCol;
	}
	size_t BoxLen = (MAX_COLUMNS -(BoxPad.size()*2))/TotalCol;
	BoxLen = BoxLen + (CurCol%TotalCol)*BoxLen%TotalCol;

	Bar.append(BoxLen-2, tb);

	if (Head)
	{
		printf("\n%s%c%s%c\n", BoxPad.$, tl, Bar.$, tr);
	}

	// Format the text, either truncate it or add spaces so the borders line up //
	if (Text != "")
	{
		if (Text.size() > (BoxLen - 4))
		{
			Text.insert(Text.find(") ")+2, 3, '.');
			Text.erase(Text.find("...")+3,Text.size()-BoxLen+4);
		}

		if (Cent)
		{
			Text.insert(0,(BoxLen-Text.size()-3)*Cent/2, ' ');
		}

		Text.append(BoxLen-Text.size()-4, ' ');

		if (CurCol == 1)
		{
			printf("%s",BoxPad.$);
		}
		printf("%c %s ", lr, Text.$);
		if (CurCol == TotalCol)
		{
			printf("%c\n",lr);
		}
	}
	Bar = "";
	Bar.append(BoxLen - 2, tb);
	if (Foot)
	{
		printf("%s%c%s%c\n", BoxPad.$, bl, Bar.$, br);
	}
}


// Exit the program, clean stuff up //
void Quit(string error1 = "", string error2 = "")
{
	// An error occurred, make color red and display error //
	if (error1 != "")
	{
		system("color 4f");
		if (error2 != "")
		{
			Frame(error1,1,0,1);
			Frame(error2,0,1,1);
		}
		else
		{
			Frame(error1);
		}
	}

	Frame("Thank you for using " APP_NAME " " APP_VER, 1,0,1);
	Frame("www.paxtez.zachd.com  |  Discord: Paxtez  |  Reddit: Paxtez", 0,1,1);
	system("pause");
	_fcloseall();
	exit(1);
}

// Check if the file exists //
inline bool Exists(const std::string& name)
{
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

// Ensures a file can be opened //
int SafeOpen(string file, string Flags)
{
	if ((fFile = fopen(file.$, Flags.$)))
	{
		return 1;
	}

    return 0;
}

string f_trim(string str)
{
	//return str;
    size_t first = str.find_first_not_of(' ');
	if (first == string::npos)
		return "";
    if (string::npos == first)
    {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

// Checks the file for the target string //
int f_findNameBlock(string file)
{
	string Target = "[ PLAYER NAMES ]";
	int CurPos = 0, Loop, FileSize = 0, BlockSize = 8192;
	size_t ui;
	char *buffer;

	if (SafeOpen(file, "rw"))
	{
		printf("... Checking %s ...\n", file.$);

		// Get the file size //
		fseek(fFile, 0, SEEK_END);
		FileSize = ftell(fFile);
		fseek(fFile, 0, SEEK_SET);

		// Loop through the file BlockSize at a time //
		for(; CurPos + BlockSize != FileSize;)
		{
			// Make sure we don't go over //
			if ((CurPos+BlockSize) > FileSize)
			{
				BlockSize = FileSize - CurPos;
			}
			buffer = new char [BlockSize];
			CurPos = ftell(fFile);
			fread(buffer, BlockSize, 1, fFile);

			// Check each character by character to check if it exists //
			for (Loop = 0; Loop < BlockSize ; Loop++)
			{
				for (ui = 0; buffer[Loop+ui] == Target[ui]; ui++)
				{
					//		printf("... c:%i l: %i ui: %i...\n", CurPos, Loop, ui);
					// We found a string match //
					if (ui == 15)
					{
						BlockLoc = CurPos + Loop;
						printf("... Found Player Name Block %i ...\n",BlockLoc);
						return BlockLoc;
					}
				}
			}
		}
		Quit("Unable to Location Player Name data");
	}
	else
		printf("... Unable to Open %s ...\n", file  .$);
	return 0;
}

// Lookup the color name based on the color code //
string f_getColorName(string colorCode)
{
	int i;

	for (i = 0; i < NUM_COLORS; i++)
	{
		if (colorCode == colorListCode[i])
		{
			return colorListName[i];
		}
	}
	return "";
}

// Prompts the user for a new name and validates it //
string f_getName(int playerNumber, string currentName = "")
{
	string prompt = "";

	// If editing a player name
	if (playerNumber > 0)
	{

		if (currentName != "")
		{
			Frame(" ENTER A NEW PLAYER NAME FOR PLAYER #"+to_string(playerNumber)+" ("+currentName+")",1,1,0);
		}
		else
		{
			Frame(" ENTER PLAYER NAME FOR PLAYER #"+to_string(playerNumber),1,1,0);
		}

		// Show rules //
		printf(" - 14 Characters Max\n - Enter a blank line to remove this player\n");
	}
	// If changing the default player name //
	else
	{
		Frame(" ENTER THE DEFAULT PLAYER NAME",1,1,0);
		printf("  - 14 Characters Max\n");
	}

	// Print a character count guide //
	printf(" [123456789ABCDE]\n :");

	// Get the line of text //
	getline(cin, prompt);

	prompt = f_trim(prompt);

	// Can't remove the default name //
	if ((playerNumber == 0) && (prompt == ""))
	{
		Frame("DEFAULT NAME IS REQUIRED",1,1,0);
		return f_getName(playerNumber, currentName);
	}
	// Name is too big //
	else if (prompt.size() > 14)
	{
		Frame(prompt+" IS TOO LARGE!",1,1,0);
		return f_getName(playerNumber, currentName);
	}

	// All good
	return prompt;
}

// Prompts the user for the color wanted for the player //
string f_selectColor(string playerName = "")
{
	// Defaults //
	string prompt, code, name;
	int i;
	size_t ui;

	// Draw Header //
	if (playerName != "")
		Frame("SELECT COLOR FOR "+playerName, 1, 0);
	else
		Frame("SELECT COLOR", 1, 0);

	// Draw all of the colors in 2 columns //
	for (i = 0; i < NUM_COLORS; i++)
	{
		code = colorListCode[i];
		name = colorListName[i];
		Frame(code+" - "+name, 0, 0, 0, (i%2)+1, 2);
	}
	// Draw random color option //
	Frame("99 - Random Color", 0, 0, 0, 2, 2);

	// Close the frame //
	Frame("", 0, 1, 0);

	// Prompt user //
	printf("Select Color: ");
	getline(cin, prompt);
	f_trim(prompt);

	// Convert any lowercase letters to uppercase //
	for (ui = 0; ui <= prompt.size(); ui++)
	{
		prompt[ui] = toupper(prompt[ui]);
	}

	// Random select //
	if (prompt == "99")
	{
		prompt = colorListCode[rand() % NUM_COLORS];
	}

	// If able to lookup the color and find a name, it is valid //
	if (f_getColorName(prompt) == "")
	{
		return f_selectColor(playerName);
	}
	return prompt;
}

// Update the file with the default name //
// parameter: name, the new name to be written //
void f_updateDefaultName(string name)
{
	size_t ui;
//	printf("... UPDATEING DEFAULT NAME TO %s ...\n", name.$);

	// Open the file for appending //
	if (SafeOpen(FileName, "r+b"))
	{
		// Add spaces evenly around the name//
		for (ui = name.size(); ui < 15; ui++)
		{
			if (ui%2 == 0)
			{
				name = ' '+name;
			}
			else
			{
				name = name+' ';
			}
		}

		// Write the new text//
		fseek(fFile, BlockLoc - 16, SEEK_SET);
		fprintf(fFile, "%s", name.$);
	}

	// Be Kind rewind //
	fclose(fFile);

	return;
}

// Update the player block //
// parameters: player number, name, and color to be written to the file //
void f_updatePlayerBlock(int plNum, string plName, string plColor)
{
	size_t ui;
	int numSpaces = 0, colorInt;

	// Convert the color code hex value to an int "2b" = 43 //
	colorInt = (int)strtol(&plColor[0], NULL, 16);

	//printf("... UPDATING PLAYER NUMBER %i TO NAME TO %s / %s...\n", plNum, plName.$, plColor.$);
	// Open the file //
	if (SafeOpen(FileName, "r+b"))
	{
		// Player number 1 is really #0 //
		plNum--;
		plName = f_trim(plName);

		// Right justify the name and count the spaces //
		for (ui = plName.size(); ui < 15; ui++)
		{
			plName = ' '+plName;
			numSpaces++;
		}

		// Seek to the location and write the name //
		fseek(fFile, BlockLoc + 16 + (plNum * 16), SEEK_SET);
		fprintf(fFile, "%s", plName.$);

		// Write the font ID as an int //
		fseek(fFile, BlockLoc + NAME_COLOR_OFFSET + plNum, SEEK_SET);
		fprintf(fFile, "%s", (char *) &colorInt);

		// Write the number of spaces as an int //
		fseek(fFile, BlockLoc + NAME_SPACE_OFFSET + plNum, SEEK_SET);
		fprintf(fFile, "%s", (char *) &numSpaces);
	}

	// Clean up //
	fclose(fFile);
}

// Scan the current file and extract the names and colors //
void f_getPlayerNames()
{
	// Defaults //
	string name, defName, pad, colorS, prompt;
	char color;

	int i, selected;
	size_t ui;
	char *buffer;

	// Temp storage of the names this time //
	string nameBuffer[NUM_PLAYER_NAMES] = {};

	if (SafeOpen(FileName, "rw"))
	{
		// Get the default name //
		fseek(fFile, BlockLoc - 16, SEEK_SET);
		buffer = (char*)malloc(16);
		fread(buffer, 1, 16, fFile);
		defName = f_trim(buffer);

		// Draw title and the default name //
		Frame("CURRENT PLAYER NAMES",1,0);
		Frame("00) "+defName,0,0,0, 1, 1);

		// Draw all the player names //
		for (i = 0; i < NUM_PLAYER_NAMES; i++)
		{
			// Get the player name
			fseek(fFile, BlockLoc + 16 + (i * 16), SEEK_SET);
			buffer = (char*)malloc(16);
			fread(buffer, 1, 16, fFile);
			name = f_trim(buffer);

			// Store the player name in an array //
			nameBuffer[i] = name;

			// Get the color //
			fseek(fFile, BlockLoc + NAME_COLOR_OFFSET + i, SEEK_SET);
			color = fgetc(fFile);

			// Convert the color to hex //
			snprintf(buffer, 3, "%02x", color);
			colorS = buffer;

			// Make it uppercase //
			for (ui = 0; ui <= colorS.size(); ui++)
			{
				colorS[ui] = toupper(colorS[ui]);
			}

			// Format the string //
			if (colorS == "FF")
			{
				colorS = "[NONE]";
			}
			else
			{
				colorS = colorS + " - " + f_getColorName(colorS);
			}

			// Pad the number //
			if (i < 9)
				pad = "0";
			else
				pad = "";

			// Draw the row //
			Frame(pad+to_string(i+1)+") "+name,0,0,0, 1, 2);
			Frame("COLOR: "+colorS,0,0,0, 2, 2);
		}
		fclose(fFile);

		// Draw the exit, frame, and prompt //
		Frame("99) Quit",0,0,0, 1, 1);
		Frame("",0,1,0);
		printf("\n Enter Player Number to Edit: ");
		getline(cin, prompt);
		prompt = f_trim(prompt);

		// If they entered 99 quit //
		if (prompt == "99")
		{
			Quit();
		}
		// This hard coded since text gets atoi'ed to 0 //
		else if (prompt == "0" || prompt == "00")
		{
			name = f_getName(0, defName);
			f_updateDefaultName(name);
		}
		else
		{
			// Convert the string to an int //
			selected = atoi(prompt.$);

			// Make sure it is valid //
			if (selected > 0 && selected < 17)
			{
				// Get the name //
				name = f_getName(selected, nameBuffer[selected-1]);

				// Get the color //
				if (name != "")
				{
					colorS = f_selectColor(name);
				}
				// They entered a blank name, don't need the color //
				else
				{
					colorS = "FF";
				}

				// Update the code with the new data //
				f_updatePlayerBlock(selected, name, colorS);
			}
			else
			{
				printf(" ... UNKNOWN VALUE PLEASE ENTER THE NUMBER 0-16 TO EDIT ... \n");
			}
		}
	}

	// Loop forever until Quit //
	f_getPlayerNames();
	return;
}

// Main function, where c/c++ programs start //
int main(int argc, char* argv[])
{
	// Seed the random //
	std::srand(std::time(nullptr));

	// Set the title and the color
	system("title " APP_NAME " " APP_VER " By Paxtez");
	system("color 1f");

	int i;

	// process all the extra attributes (flags, dragged files, etc.) //
	for (i = 1; i < argc; i++)
	{
		// Check for dragged folder, process that folder //
		if (Exists((argv[i])))
		{
			FileName = argv[i];
			break;
		}
	}

	// Check for any 1ST_READ files nearby //
	if ((FileName == "") && (Exists("1ST_READ.BIN")))
	{
		FileName = "1ST_READ.BIN";
	}

	// Process file //
	if (FileName != "")
	{
		f_findNameBlock(FileName);
		f_getPlayerNames();
	}
	else
	{
		Quit("Please drag an MVC2 .CDI/GDI or 1ST_READ.BIN on to this program to edit player names.");
	}
	return 1;
}

