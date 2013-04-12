#include "MotorControl.h"

#include "LinuxDARwIn.h"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include <sstream>
#include <iterator>

using namespace Robot;

const std::string fileName = "log.txt";
const int uDt = 16; // Time step in micro second

int main()
{   
   std::cout << "Starting program. Sleeping 4 sec\n";
   sleep(4);
   
   // Framework initialization
   LinuxCM730 linux_cm730("/dev/ttyUSB0");
   CM730 cm730(&linux_cm730);
   if(cm730.Connect() == false)
   {
      std::cout << "Fail to connect CM-730!\n";
      return 0;
   }
   
   std::string strLine;
   std::ifstream file;
   
   file.open( fileName.c_str() );   
   
   MotorControl motion = MotorControl( &cm730 );
   
   // Enable Torque
   if( !motion.SetTorque( MotorControl::ALL_LEGS ) )
   {
      std::cout << "SetTorque Failed\n";
      file.close(); 
      return 0;
   }
   
   // Set inital position
   if( !file.eof() )
   {
      std::vector<double> vPos;
      
      getline( file, strLine );
      std::istringstream iss(strLine);

      std::copy(std::istream_iterator<double>(iss),
	        std::istream_iterator<double>(),
	        std::back_inserter(vPos));      
      
      if( !motion.InitPosition( vPos, 4000 ) )
      {
	 file.close(); 
	 return 0;
      }
   }   
   
   // Process mouvement
   while( !file.eof() )
   {
      std::vector<double> vPos;
      //motion.readPosition( vPos );
      
      //
      getline( file, strLine );
      std::istringstream iss(strLine);

      std::copy(std::istream_iterator<double>(iss),
	        std::istream_iterator<double>(),
	        std::back_inserter(vPos));
      //
      
      if( !motion.SetPosition( vPos, MotorControl::ALL_LEGS ) )
      {
	 break;
      }
      usleep(uDt);
   }   
   
   std::cout << "Finishing program. Sleeping 4 sec\n";
   sleep(4);
   
   // Disable torque
   motion.unInit();
   file.close();
   std::cout << "Program finished\n";
   
   return 0;	
}

