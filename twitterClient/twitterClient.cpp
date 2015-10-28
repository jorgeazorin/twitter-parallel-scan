#include "twitterClient.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
using namespace std;


//////////////////////////////////////////////////////////////
//Modulo para dividir string
/////////////////////////////////////////////////////////////
	vector<string> split(string str, char delimiter) {
		  vector<string> internal;
		  stringstream ss(str);
		  string tok;
		  
		  while(getline(ss, tok, delimiter)) {
		    internal.push_back(tok);
		  }
		  
		  return internal;
	}



//////////////////////////////////////////////////////////////
//MAIN
//////////////////////////////////////////////////////////////
int main( int argc, char* argv[] )
{

	//Crear objeto para twitter e iniciar sesion

    twitCurl twitterObj;
    std::string tmpStr, tmpStr2;
    std::string replyMsg;
    char tmpBuf[2048];

    twitterObj.setTwitterUsername( "jorgeazorin" );
    twitterObj.setTwitterPassword( "179832" );
    memset( tmpBuf, 0, 2048 );
  	twitterObj.getOAuth().setConsumerKey( std::string( "2Kdg60HDmZEu2NXIp7MRMBQIm" ) );
    twitterObj.getOAuth().setConsumerSecret( std::string( "IQcdiWnJd1bsWHytbLmSZa4aNxlPJ5Jr9ZjwOPjiDp31Tyactn" ) );
    replyMsg = "";					


    //Crear un archivo con los resultados
    ofstream Fichero("datos.csv");         //Opening file to print info to
    Fichero << "Time Force(N/m)"<<";"<<"Otra cosa" << endl;          //Headings for file

    //Obtener los tweets de un usuario
    bool tweetscompletos=false;
    string ultimotweet="";
    while(!tweetscompletos){
    	tweetscompletos=true;
   							 //trimUser //retweets // nº //usuario//esusuarioID//ultimotweet
	    if( twitterObj.timelineUserGet(  true, false,1000000,"encarnaamoros",false,ultimotweet) ){
	        twitterObj.getLastWebResponse( replyMsg );
	        string a=replyMsg.c_str();
	        vector<string> sep = split(a, ',');
	        bool ya=false;
	        bool ya2=false;
	        bool ya3=true;
		   	for(string t : sep){
			   	if(t.length()>3){
			   	//	 cout << t<<endl;
		    		 if(t.substr (2,10)=="created_at" ){
		    		 	Fichero <<t.substr(15,t.length()-16) <<";";                                
		    		 	ya=true;
		    		 	ya3=true;
		    		 	tweetscompletos=false;
		    		 }else if(ya3==true && t.substr (1,2)== "id"){
		     			ultimotweet=t.substr(5,t.length()-5);
		     			ya3=false;
		     			 
					}else
						ya3=false;


		    		 if( t.substr (1,4)== "text" && ya==true && t.substr (1,4)== "text"){
		    		 	Fichero <<t.substr(8,t.length()-9); 
		    		 	ya2=true;
		     		}//"source"
		     		if(t.substr(1,6)=="source" && ya==true){
		     			Fichero <<endl;
		     			ya2=false;
		     			ya=false;
		     		}
		     		if(t.substr(1,6)=="source" && ya==true){
		     			Fichero <<endl;
		     			ya2=false;
		     			ya=false;
		     		}
                   		     
		     		if(ya2==true){
		       			Fichero<<t;
		     		}
		     	}else
					tweetscompletos=true;
			}
	    }else{
	    	printf( "\nNo va\n" );
	    	tweetscompletos=true;
	        twitterObj.getLastCurlError( replyMsg );
	    }  
	 }

      

     //Cerrar fichero
     Fichero.close();

}