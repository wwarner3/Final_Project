#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <cmath>

using namespace std;

class Component{

	public:
		Component() : type("") {}  // , node1(0), node2(0), value(0.0) {} 
		
		void  setTYPE(string t){ type = t; }
			string getTYPE() { return(type);}
		void setNODE1(int n1){ node1 = n1; }
			int getNODE1() {return node1;}
		void setNODE2(int n2){ node2 = n2; }
			int getNODE2() {return node2;}
		void setVALUE(double v){ value = v;}
			double getVALUE() { return value; }
		void setMETRIC(string m) { metric = m; }
			string getMETRIC() { return(metric); }
		void setSTATUS(string s){ status = s; }
			string getSTATUS() { return(status); }
		~Component(){}
	private:
		string type;
		int node1;
		int node2;
		double value;
		string metric;
		string status;

};



double calculateVOLT( vector<Component> components, map<int, double> nodeV, ofstream &outFile){

	double current;
	nodeV[0] = 0;

	for( Component component : components){
		cout << "Processing components: " << component.getTYPE() << endl;// added for debuging

		if(component.getTYPE()[0] == 'V' || component.getTYPE()[0] == 'v'){ //have to uses [0] the first element or else R1 ..R2 will not work for multiple

			nodeV[component.getNODE1()] = component.getVALUE();

		}else if(component.getTYPE()[0] == 'R' || component.getTYPE()[0] == 'r'){ //used both R and r for each case just in case
			
				 current = (nodeV[component.getNODE1()] - nodeV[component.getNODE2()]) / component.getVALUE();
				nodeV[component.getNODE2()] = nodeV[component.getNODE1()] - current * component.getVALUE();

		}
	outFile << "NODE VOLTAGES: " << endl;
	for( auto entry : nodeV){ // map function from stackoverflow
		outFile << "NODE: " << entry.first << " = " << entry.second << " VOLTS" << endl; }
	} 
};

double CalculateVOLTAGESOURCE( vector<Component> components){//, map<int , double> nodeV){ //right now works with series voltage source/ but not parallele
										      //inorder to use parallel source have to compare nodes using map**

	double Vtot;
	//nodeV[0] = 0.0;
	for(Component component : components){

		if( component.getTYPE()[0] == 'V' || component.getTYPE()[0] == 'v'){
			
			Vtot += component.getVALUE();	
		}

	}

return Vtot;
};
double CalculateRTOT(vector<Component> components){

	double Rtot = 0.0;
	double Rs = 0.0;
	double Rp = 0.0;
	
	for(Component component : components){

		if( component.getTYPE()[0] == 'R' || component.getTYPE()[0] == 'r'){

			if(component.getSTATUS()[0] == 'S' || component.getSTATUS()[0] == 's'){

				Rs += component.getVALUE();
			}

			if(component.getSTATUS()[0] == 'P' || component.getSTATUS()[0] == 'p'){
				Rp += (1.0 / component.getVALUE());	
			}

		}
	}
	Rp = pow(Rp,-1);

	Rtot = Rs + Rp;

return Rtot;
};





int main(int argc, char *argv[]){

	if( argc != 2) { cout << "ERROR, INVALID INPUT.. NEED CIRCUIT TEXTFILE" << endl; return 0; }

	ifstream inFile(argv[1]);

	if(!inFile) { cout << "ERROR OPENING FILE" << endl; return 0; }

	vector<Component> components;
	double current;
	double Rtot;
	double Vtot;

	string line;
	while(getline(inFile, line)){
	
		istringstream iss(line);
		
		Component com;
	
		//iss >> com.setTYPE; we can not directly call the set function so we have to make a temp// we can avoid if we use struct but we need our private valueables
		string TYPE;
		iss >> TYPE;
		com.setTYPE(TYPE);

		int NODE1;
		iss >> NODE1;
		com.setNODE1(NODE1);

		int NODE2;
		iss >> NODE2;
		com.setNODE2(NODE2);
	
		double VALUE;
		iss >> VALUE;
		com.setVALUE(VALUE);

		string METRIC;
		iss >> METRIC;
		com.setMETRIC(METRIC);

		string STATUS;
		iss >> STATUS;
		com.setSTATUS(STATUS);

		components.push_back(com);
	}

	cout << "Componets read from file: " << endl;

	for( Component com : components){

		cout << com.getTYPE() <<" "<< com.getNODE1() <<" "<< com.getNODE2() <<" "<< com.getVALUE() << " " << com.getMETRIC() << " " <<  com.getSTATUS() << endl;
	}

	//vector<int i, double d> nodeVOLTAGE;// cannot use this stackoverflow recommands map or using boost:: variant
	//went with map because did some research and it works almost like vector plus the syntax looks easier
	
	map<int, double > nodeVOLTAGE;

	//calculateVOLT( components , nodeVOLTAGE);
	Vtot = CalculateVOLTAGESOURCE(components);
	Rtot = CalculateRTOT(components);
	current = Vtot / Rtot;
	

	ofstream outFile("measurements.dat");
	
	if(!outFile){ cout << "ERROR OPENING OUTPUT FILE FOR MEASUREMENTS" << endl; return 0; }
	

	calculateVOLT(components, nodeVOLTAGE, outFile);
	
		outFile << "the total volatges: " << Vtot << " VOLTS" << endl
			<< "the total current: " << current << " AMPS" << endl
			<< "the total resistance: " << Rtot << " OHMS" << endl;

		
	cout << "MEASUREMENT FILE CREATED" << endl;
	
outFile.close();
inFile.close();
return 0;
};
