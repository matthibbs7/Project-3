#include "Wad.h"

Wad::Wad(uint8_t *pData){
    unsigned int    header_desc_num,    //header descriptor number
                    header_desc_off,    //header descriptors offset
                    i;                  //counter variables
    string          header_magic;       //header magic (file type)

    //Magic (File Type)
    for (i = 0; i < 4; i++){
        //debug
        cout << "debug> pData[" << i <<  "]: " << pData[i] << endl;

        header_magic += pData[i];     //store magic header
    }
    magic = header_magic;

    //Number of Descriptiors
    header_desc_num = int((unsigned char)(pData[4]) |   
                          (unsigned char)(pData[5]) << 8 | 
                          (unsigned char)(pData[6]) << 16 |
                          (unsigned char)(pData[7]) << 24);

    //Descriptor Offset
    header_desc_off = int((unsigned char)(pData[8]) |
                          (unsigned char)(pData[9]) << 8 |
                          (unsigned char)(pData[10]) << 16 |
                          (unsigned char)(pData[11]) << 24);
    int offset = header_desc_off;
    //debug
    cout << "debug> header_magic: " << header_magic << endl;
    cout << "debug> header_desc_num: " << header_desc_num << endl;
    cout << "debug> header_desc_off: " << header_desc_off << endl;

    string mapMarker = "";
    int ten_elements = 0, level = 0;
    string currPath = "/";
    TreeNode* root = new TreeNode(0,0,"root","/");
    TreeNode* curr_node = root;
    //Descriptors
    int curr_level; 
    for (i = 0; i < header_desc_num; i++){   //loop through number of descriptors
	 int curr_element_offset = 0;
	 int curr_element_length = 0;
	 string name = "";
	 
	 // Element Offset
	 curr_element_offset = int((unsigned char)(pData[offset]) |
				   (unsigned char)(pData[offset+1]) << 8 |
				   (unsigned char)(pData[offset+2]) << 16 |
				   (unsigned char)(pData[offset+3]) << 24);
	 // Element Length
	 curr_element_length = int((unsigned char)(pData[offset+4]) |
				   (unsigned char)(pData[offset+5]) << 8 |
				   (unsigned char)(pData[offset+6]) << 16 |
				   (unsigned char)(pData[offset+7]) << 24);
	// Descriptor Name
	for (int j = 0; j < 8; j++) {
		name += pData[offset+j+8];
	}
	cout << currPath + name << endl;
	
	// CHECK FOR MARKER ELEMENTS
	// 1. First Check if of form "E#M#"
	// 2. Check for _START or _END

	// If it is a directory
	if (ten_elements > 0) {
		int counter = count(currPath.begin(), currPath.end(), '/');
		//TODO add to E#M# dir
		curr_level = 0;
		curr_node = root;
		while (curr_level < level) {
			curr_node = curr_node->children[curr_node->children.size()-1];
			curr_level++;
		}
		curr_node->children.push_back(new TreeNode(curr_element_offset, curr_element_length, name, currPath + name));
		ten_elements--;
		if (ten_elements == 0) {
			level--;
			int pathSize = currPath.length();
			for (int k = pathSize-2; k > -1; k--) {
				if (currPath[k] == '/') {
					currPath.pop_back();
					break;
				}
				currPath.pop_back();
			}
			offset += 16;
			continue;
		}
	}
	if (curr_element_length == 0) {
		// Start 10 element Dir
		if (name[0] == 'E' && isdigit(name[1]) && name[2] == 'M' && isdigit(name[3])) {
			curr_level = 0;
			curr_node = root;
			
			// at root '/'
			if (level == 0) {
				curr_node->children.push_back(new TreeNode(curr_element_offset, curr_element_length, name, currPath += (name + '/')));
			}
			// create root in subdirectory ex: '/example/HERE'
			else {
				while (curr_level < level) {
					curr_node = curr_node->children[curr_node->children.size()-1];
					curr_level++;
				}
				curr_node->children.push_back(new TreeNode(curr_element_offset, curr_element_length, name, currPath += (name + '/')));
			}
		
		
			//root->children.push_back(new TreeNode(curr_element_offset, curr_element_length, name, currPath += (name + '/'))); 
			ten_elements = 10;
			mapMarker = name;
			level++;
		} // Open Dir
		else if (name[name.length()-6] == '_' && name[name.length()-5] == 'S' && name[name.length()-4] == 'T' && name[name.length()-3] == 'A' && name[name.length()-2] == 'R' && name[name.length()-1] == 'T') {
			curr_level = 0;
			curr_node = root;
			if (level == 0) {
				curr_node->children.push_back(new TreeNode(curr_element_offset, curr_element_length, name, currPath += (name + '/')));
			} else {
				while (curr_level < level) {
					curr_node = curr_node->children[curr_node->children.size()-1];
					curr_level++;
				}
				curr_node->children.push_back(new TreeNode(curr_element_offset, curr_element_length, name, currPath += (name + '/')));
			}

			level++;
		} // End Dir
		else if (name[name.length()-6] == '_' && name[name.length()-5] == 'E' && name[name.length()-4] == 'N' && name[name.length()-3] == 'D') {
			level--;
			int pathSize = currPath.length();
			for (int k = pathSize-2; k > -1; k--) {
				if (currPath[k] == '/') {
					currPath.pop_back();
					break;
				}
				currPath.pop_back();
			}
		}
		
	
	} else if (ten_elements==0) {
		curr_level = 0;
		curr_node = root;
		if (level == 0) {
			curr_node->children.push_back(new TreeNode(curr_element_offset, curr_element_length, name, currPath + name));
		} else {
			while (curr_level < level) {
				curr_node = curr_node->children[curr_node->children.size()-1];
				curr_level++;
			}
			curr_node->children.push_back(new TreeNode(curr_element_offset, curr_element_length, name, currPath + name));
		}
	}
	
	//TODO Load into data structure
	//TODO Interpret Directory type E#M# or 




	offset += 16;
    }
    cout << "Printing: " << root->children[0]->children[9]->name << endl;
    cout << "Printing: " << root->children[1]->name << endl;
    my_root = root;
}

Wad* Wad::loadWad(const string &path){
    ifstream wadFile(path, std::ios::binary | std::ios::ate);
    streamsize wadFilesize = wadFile.tellg();
    uint8_t *pData = new uint8_t[(unsigned int)wadFilesize];
    wadFile.seekg(0, std::ios::beg);
    wadFile.read((char *)pData, wadFilesize);

    //debug
    cout << "debug> filesize: " << wadFilesize << endl;

    return new Wad(pData);
}

string Wad::getMagic(){
    return magic;
}

bool Wad::isContent(const string &path){
    return 0;
}

bool Wad::isDirectory(const string &path){
    return 0;
}

int Wad::getSize(const string &path){
    return 0;
}

int Wad::getContents(const string &path, char *buffer, int length, int offset){
    return 0;
}

int Wad::getDirectory(const string &path, vector<string> *directory){
    return 0;
}
