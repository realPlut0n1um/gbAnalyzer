#include <string>
#include <locale>
#include <vector>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

// Seek through our data passed and remove any trash data found
void seekerDelete(std::string& rawData, std::string subToDelete){
	
	std::string::size_type indx = rawData.find(subToDelete);
	while(indx != std::string::npos){
		rawData.erase(indx, subToDelete.length());
		indx = rawData.find(subToDelete, indx);
	}
	
}

// Useful function for converting HEX -> ASCII
std::string convHex(std::string hexVals){
	std::string tmpAsciiStr;
	for(size_t indx = 0; indx < hexVals.length(); indx+=2){
		std::string hexIndx = hexVals.substr(indx, 2);
		char asciiChar = std::stoul(hexIndx, nullptr, 16);
		tmpAsciiStr += asciiChar;
	}
	return tmpAsciiStr;
}
	
// This is used to analyze the header information for the GB Cartridge
// TODO: Edit how we parse char* &data, we use it as a string passed into stringstream, instead we can just static_cast<uint8_t> and act
// on the traditional data instead of acting with strings
void cartHeaderAnalyze(char* &data){

	// [NINTENDO HEADER]

	std::stringstream checksumByteStream; // Store ROM checksum information
	std::locale immIndx; // immutable index
	uint16_t romIndx = 0x0104; // Starting index to the ROM
	// NINTENDO CHECKSUM [0x0104 -> 0x0133]
	for(romIndx; romIndx < 0x0134; ++romIndx){
		checksumByteStream << std::hex << std::setw(2) << std::setfill('0') << (int)data[romIndx];
	}
	std::string nChecksum = (checksumByteStream.str());
	// Convert the recovered checksum to type std::string and delete trash ffffff data
	seekerDelete(nChecksum, "ffffff");
	std::cout << "[LOCATION] (0x0104 -> 0x0133)" << std::endl;
	std::cout << "- NINTENDO_CHECKSUM: ";

	// Itterate through the nChecksum, and raise the case of each character using std::toupper(); and display recovered checksum
	for(std::string::size_type x = 0; x < nChecksum.length(); ++x){
		std::cout << std::toupper(nChecksum[x], immIndx);
	}

	// Reset the checksumByteStream
	checksumByteStream.str(std::string());
	checksumByteStream.clear();
	
	// [TITLE]
	std::stringstream titleByteStream; // Store ROM title information
	std::string asciiRomTitle = "";
	for(romIndx; romIndx < 0x0144; romIndx++){
		titleByteStream << std::hex << std::setw(2) << std::setfill('0') << (int)data[romIndx];
	}
	
	std::string hexRomTitle(titleByteStream.str());
	
	// Convert the hex values to ASCII for printing and disply name
	std::cout << "\n\n[LOCATION] (0x0134 -> 0x143)" << std::endl;
	asciiRomTitle = convHex(hexRomTitle);
	std::cout << "- TITLE: " << asciiRomTitle << std::endl;	

	// [SGB FLAG]
	bool sgbSupport; // to be used later for new licensee code analysis
	std::stringstream sgbSet;
	romIndx = 0x0146;
	sgbSet << std::hex << std::setw(2) << std::setfill('0') << (int)data[romIndx];
	std::cout << "\n[LOCATION] (0x0146)" << std::endl;
	if(sgbSet.str() == "03"){
		std::cout << "- SGB SUPPORT: True" << std::endl;
		sgbSupport = true;
	} else {
		std::cout << "- SGB SUPPORT: False" << std::endl;
		sgbSupport = false;
	}

	// [NEW LICENSEE CODE]
	std::string nLicenseeCode;

	/* Now i KNOW this is a shitty way of doing this, but we index into nLicenseeCodeTable, match it with our data from the ROM
	   then we take the index and apply that to the nLicenseeNameTable to get the Licensee name  */
	
	// TODO: implement std::map<uint8_t, std::string> nLicenseeCodeTable;
	// This is a better method instead of using the method we have described here
	// Store codes for the Licensee
	std::string nLicenseeCodeTable[62] = {"00","01","08","13","18",
					      "19","20","22","24","25",
					      "28","29","30","31","32",
					      "33","34","35","37","38",
					      "39","41","42","44","46",
					      "47","49","50","51","52",
					      "53","54","55","56","57",
					      "58","59","60","61","64",
					      "67","69","70","71","72",
					      "73","75","78","79","80",
					      "83","86","87","91","92",
					      "93","95","96","97","99","A4"};

	// Store a table of the names related to the licensee code table
	std::string nLicenseeNameTable[62] = {"None", "Nintendo R&D1", "Capcom", "EA", "Hudson Soft", 
                                              "b-ai", "kss", "pow", "PCM Complete", "San-x", 
					      "Kemco Japan", "Seta", "Viacom", "Nintendo",
					      "Bandai", "Ocean/Acclaim", "Konami", "Hector", 
					      "Taito", "Hudson", "Banpresto", "Ubisoft", "Atlus", 
					      "Malibu", "Angel", "Bullet-Proof","Irem", "Absolute", 
					      "Acclaim", "Activision", "American Sammy", "Konami", "Hi Tech Entertainment", 
					      "LJN", "Matchbox", "Mattel", "Milton Bradley", "Titus", 
					      "Virgin", "LucasArts", "Ocean", "EA", "Infogrames", 
					      "Interplay", "Broderbund", "Sculptured", "Sci", "THQ", 
					      "Accolade", "Misawa", "Lozc", "Tokuma Shoten Intermedia", "Tsukuda Original", 
					      "Chunsoft", "Video System", "Ocean/Acclaim", "Varie", "Yonezawa/s'pal", "Kaneko", "Pack in soft", "Konami (Yu-Gi-Oh!)"};

	bool foundLicensee = false;
	uint8_t licenseeIndx = 0;
	// check to see if SGB is enabled, branch to respective memory location to retrieve the new licensee code
	if(sgbSupport == true){
		std::cout << "\n[LOCATION] (0x0145)" << std::endl;
		std::stringstream nLicenseeStream;
		romIndx = 0x0145; // SGB_true Licensee code location
		// since SBG is enabled, we will seek to (0x0145)
		for(romIndx; romIndx < 0x0146; romIndx++){
			nLicenseeStream << std::hex << std::setw(2) << std::setfill('0') << (int)data[romIndx];
		}
		// store our SGB_true Licensee code here
		nLicenseeCode = nLicenseeStream.str();

		// Seek through the table to lookup SGB_true Licensee
		while(foundLicensee != true){
			// if SGB_true Licensee Code was matched, stop loop for reporting
			if(nLicenseeCode.compare(nLicenseeCodeTable[licenseeIndx]) == 0){
				std::cout << "- LICENSEE: " << nLicenseeNameTable[licenseeIndx] << std::endl;
				foundLicensee = true;
			}
			licenseeIndx++;
		}
	}

	// does not use SGB, so use a different memory address
	else{
		std::cout << "\n[LOCATION] (0x014B)" << std::endl;
	    	std::stringstream nLicenseeStream;
		romIndx = 0x014B; // Non-SGB licensee code location
		nLicenseeStream << std::hex << std::setw(2) << std::setfill('0') << (int)data[romIndx];
		
		// Store our SGB_false Licensee code here
		nLicenseeCode = nLicenseeStream.str();
		while(foundLicensee != true){
			// If the code matches what was fetched, stop the loop for reporting 
			if(nLicenseeCode.compare(nLicenseeCodeTable[licenseeIndx]) == 0){
				std::cout << "- LICENSEE: " << nLicenseeNameTable[licenseeIndx] << std::endl;
				foundLicensee = true;
			}
			licenseeIndx++;
		}
	}	
	

	// [CARTRIDGE TYPE]
	
	// Check hardware information about the cartridge
	std::stringstream hwInfoStream;
	uint8_t hwIndx;
	bool foundHwType = false;
	romIndx = 0x0147;
	hwInfoStream << std::hex << std::setw(2) << std::setfill('0') << (int)data[romIndx];
	std::string hwInfo = (hwInfoStream.str());
  
	std::string hwCode[28] = {"00", "01", "02", "03", 
				  "05", "06", "08", "09", 
				  "0B", "0C", "0D", "0F", 
				  "10", "11", "12", "13", 
				  "19", "1A", "1B", "1C", 
				  "1D", "1E", "20", "22", 
				  "FC", "FD", "FE", "FF"};
                            
	std::string hwType[28] = {"ROM ONLY", "MBC1", "MBC1+RAM" , "MBC1+RAM+BATTERY",
			          "MBC2", "MBC2+BATTERY", "ROM+RAM", "ROM+RAM+BATTERY",
				  "MMM01", "MMM01+RAM", "MMM01+RAM+BATTERY", "MBC3+TIMER+BATTERY",
				  "MBC3+TIMER+RAM+BATTERY", "MBC3", "MBC3+RAM", "MBC3+RAM+BATTERY",
				  "MBC5", "MBC5+RAM", "MBC5+RAM+BATTERY", "MBC5+RUMBLE",
				  "MBC5+RUMBLE+RAM", "MBC5+RUMBLE+RAM+BATTERY", "MBC6", "MBC7+SENSOR+RUMBLE+RAM+BATTERY",
				  "POCKET CAMERA", "BANDAI TAMA5", "HuC3", "HuC1+RAM+BATTERY"};
	
	std::cout << "\n[LOCATION] = (0x0147)" << std::endl;
	
	// seek and find the hardware type
	while(foundHwType != true){
		if(hwInfo.compare(hwCode[hwIndx]) == 0){
			std::cout << "- HARDWARE TYPE: " << hwType[hwIndx] << std::endl;
			foundHwType = true;
		}
		hwIndx++;
	}
	

	// [ROM SIZE]

	/* so here we will get the information about the rom size, i COULD use my previous defintion in parseRom(); of romSize, but i
	   will grab it from the ROM for true analysis to get information on banks, etc. */

	std::stringstream romSizeRaw;
	romIndx = 0x0148;
	romSizeRaw << std::hex << std::setw(2) << std::setfill('0') << (int)data[romIndx];
	std::string romSizeTrue = romSizeRaw.str(); // create our string to hold the value
	std::string::size_type rSz;
	uint8_t romSizeCode = std::stoi(romSizeTrue, &rSz);
	uint16_t romSizeFinal = (0x20 << romSizeCode);
	std::cout << "\n[LOCATION] = (0x0148)" << std::endl;
	
	switch(romSizeCode){
		case 0:
			std::cout << "- ROM SIZE: " << romSizeFinal << " KByte" << std::endl;
			std::cout << "- BANKS: 2 (NO_ROM_BANKING)" << std::endl;
			break;
		
		case 1:
			std::cout << "- ROM SIZE: " << romSizeFinal << " KByte" << std::endl;
			std::cout << "- BANKS: " << (romSizeCode << 0x2) << std::endl;
			break;		

		case 2:
			std::cout << "- ROM SIZE: " << romSizeFinal << " KByte" << std::endl;
			std::cout << "- BANKS: " << (romSizeCode << 0x2) << std::endl;
			break;	
	
		case 3:
			std::cout << "- ROM SIZE: " << romSizeFinal << " KByte" << std::endl;
			std::cout << "- BANKS: " << ((romSizeCode-0x1) << 0x3) << std::endl; 
			break;		

		case 4:
			std::cout << "- ROM SIZE: " << romSizeFinal << " KByte" << std::endl;
			std::cout << "- BANKS: " << (romSizeCode << 0x3) << std::endl;
			break;		

		case 5:
			std::cout << "- ROM SIZE: " << romSizeFinal << " KByte" << std::endl;
			std::cout << "- BANKS: " << (((romSizeCode << 0x5) / 0x3) + 0xB) << std::endl;
			break;		

		case 6:
			std::cout << "- ROM SIZE: " << romSizeFinal << " KByte" << std::endl;
			std::cout << "- BANKS: " << ((romSizeCode << 0x5) - 0x40) << std::endl;
			break;		

		case 7:
			std::cout << "- ROM SIZE: " << romSizeFinal << " KByte" << std::endl;
			std::cout << "- BANKS: " << ((romSizeCode << 0x5) + 0x20) << std::endl;
			break;		

		case 8:
			std::cout << "- ROM SIZE: " << romSizeFinal << " KByte" << std::endl;
			std::cout << "- BANKS: " << ((romSizeCode << 0x5) * 0x2) << std::endl;
			break;

		case 52:
			std::cout << "- ROM SIZE: " << romSizeFinal << " KByte" << std::endl;
			std::cout << "- BANKS: " << (romSizeCode + 0x14) << std::endl;
			break;		

		case 53:
			std::cout << "- ROM SIZE: " << romSizeFinal << " KByte" << std::endl;
			std::cout << "- BANKS: " << (romSizeCode + 0x1B) << std::endl;
			break;		

		case 54:
			std::cout << "- ROM SIZE: " << romSizeFinal << " KByte" << std::endl;
			std::cout << "- BANKS: " << (romSizeCode + 0x2A) << std::endl;
			break;
	}
	

	// [DESTINATION CODE]
	std::stringstream dCodeStream;
	romIndx = 0x014A;
	dCodeStream << std::hex << std::setw(2) << std::setfill('0') << (int)data[romIndx];
	std::string destCode = dCodeStream.str();
	std::string::size_type destCodeSz;
	uint8_t destCodeFinal = std::stoi(destCode, &destCodeSz);
	std::cout << "\n[LOCATION] = (0x014A)" << std::endl;
	switch(destCodeFinal){
		case 0:
			std::cout << "- DESTINATION CODE: Japanese" << std::endl;
			break;
		case 1:
			std::cout << "- DESTINATION CODE: Non-Japanese" << std::endl;
			break;
	}	
}

int parseRom(std::string analysisType, std::string rom){

	// Open our GB_ROM so that we can parse the information
	std::ifstream romFile;
	romFile.open(rom, std::ios::out | std::ios::binary | std::ios::ate);
	
	// Unable to Open the ROM provided
	if(!romFile.is_open()){
		std::cout << "Error, cannot open file!" << std::endl;
		return -1;
	}

	std::streampos romSize = romFile.tellg();
	char* romData = new char[romSize];
	romFile.seekg(0, std::ios::beg);
	romFile.read(romData, romSize);

	// Disassemble the ROM
	if(analysisType == "-d"){
		// TODO:
		std::cout << "NOT SUPPORTED YET!" << std::endl;
		return 0;
	}
	
	// Analyze the cartridge header
	if(analysisType == "-h"){
		cartHeaderAnalyze(romData); 
		return 0;
	}

	// Invalid Analysis type provdided
	else{
		std::cout << "Invalid [TYPE] supplied!" << std::endl;
	}

	// Close the file
	delete[] romData;
	romFile.close();
	
	return 0;  
}

int main(int argc, const char* argv[]){

	if(argc != 3){
		std::cout << "Usage: ./gbanalyzer [TYPE] [ROM]" << std::endl;
		std::cout << "[Available Analysis types]" << std::endl;
		std::cout << "[-d] Disassemble ROM" << std::endl;
		std::cout << "[-h] Cartdrige Header Info" << std::endl;
		return -1;
	}

	// Store the type argument
	std::string type = argv[1];

	// check if the ROM exists
	std::filesystem::path romPath = argv[2];
	std::filesystem::directory_entry romEntry{romPath};

	// Unable to locate ROM
	if(!romEntry.exists()){
		std::cout << "Unable to locate GB_ROM" << std::endl;
		return -1;
	}		

	// start parsing
	parseRom(type, argv[2]);

	return 0;
}
