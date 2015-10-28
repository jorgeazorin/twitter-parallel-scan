#include "twitterClient.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
using namespace std;


///////////////////////////////////////////////////////////////
//Variables globales
//////////////////////////////////////////////////////////////
string ultimotweet;
int usuariosMirados=0;
vector<int> UsuariosSinMirar;
vector<int> UsuariosMirados;
const int MAX_USUARIOS_A_MIRAR=10;
const int USUARIO_INICIO=144533310;




//////////////////////////////////////////////////////////////
//Clase Tweet
/////////////////////////////////////////////////////////////
class Tweet {
	public:
		int fecha=0;
		string texto="";

};



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
//Modulo para pasar los meses a numeros
/////////////////////////////////////////////////////////////
string MesToNum(string mes){
	if	   (mes=="Jan") return "01";
	else if(mes=="Feb") return "02";
	else if(mes=="Mar") return "03";
	else if(mes=="Apr") return "04";
	else if(mes=="May") return "05";
	else if(mes=="Jun") return "06";
	else if(mes=="Jul") return "07";
	else if(mes=="Aug") return "08";
	else if(mes=="Sep") return "09";
	else if(mes=="Oct") return "10";
	else if(mes=="Nov") return "11";
	else if(mes=="Dec") return "12";
	else         		return "00";
}




//////////////////////////////////////////////////////////////
//Modulo para obtener fecha y texto de un tweet
/////////////////////////////////////////////////////////////
Tweet StringToTweet(string t){
	Tweet tweet;
	if(t.length()>35){

		///Obtener la fecha del tweet//////////////////
	 	vector<string> fecha = split(t.substr(3,30), ' ');
	 	string fechastring=fecha[2]+MesToNum(fecha[1])+fecha[5];
	 	int fechaint = atoi(fechastring.c_str());   
	 	tweet.fecha=fechaint;    

		///Obtener el texto del tweet//////////////////
		tweet.texto=t.substr(t.find("text")+7,t.find("source")-t.find("text")-10);

		///Obtener el id del tweet/////////////////////
		ultimotweet=t.substr(t.find("id")+4,t.find("id_str")-t.find("id")-6);
	}
	return tweet;
}






//////////////////////////////////////////////////////////////
//                        MAIN                            ////
//////////////////////////////////////////////////////////////
int main( int argc, char* argv[] )
{



    ///////////////////////////////////////////////////////
    //Inicializar la libreria twitcurl/////////////////////
    ///////////////////////////////////////////////////////

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



    //Crear un archivo con los resultados//////////////////
    ofstream Fichero("datos.csv");         //Opening file to print info to
    Fichero << "Time Force(N/m)"<<";"<<"Otra cosa" << endl;          //Headings for file






    ///////////////////////////////////////////////////////
    //Recorrer la lista de usuarios que no se han visitado/
    ///////////////////////////////////////////////////////

    //Añadimos el usuario Inicial a la lista
    UsuariosSinMirar.push_back(USUARIO_INICIO);

    while(usuariosMirados<MAX_USUARIOS_A_MIRAR && !UsuariosSinMirar.empty()){
    	usuariosMirados++;

    	int UsuarioID=*UsuariosSinMirar.begin();
    	ultimotweet="";
    	cout<<endl<<endl<<"Usuario id "<<UsuarioID<<endl;
    	UsuariosSinMirar.erase(UsuariosSinMirar.begin());
       	UsuariosMirados.push_back(UsuarioID);

	    ///////////////////////////////////////////////////////
	    //Obtener los tweets de un usuario/////////////////////
	    ///////////////////////////////////////////////////////

	    bool tweetscompletos=false;

	    ///Mientas la respuesta no sea un json vacio va a ir pidiendo tweets desde el ultimo recibido
	    while(!tweetscompletos){
	    	tweetscompletos=true;					 
		    if( twitterObj.timelineUserGet(true, false,1000000,to_string(UsuarioID),true,ultimotweet)){//trimUser //retweets // nº //usuario//esusuarioID//ultimotweet
		        twitterObj.getLastWebResponse( replyMsg );
		        string respuesta=replyMsg.c_str();

		        //Veces que encuentra un tweet en el json si encuentra menos de 3 es seguro que ya no quedan tweets por mirar
		        int repeticiones=0;

		        if(respuesta.length()>50){
		        		tweetscompletos=false;
		        		cout<<"Mirando tweets desde "<<ultimotweet<<endl;

				        //Dividir el json de respuesta entre tweets//////////////////////////////////////////////
				        Tweet tweet;
				        size_t pos = 0;
						string token;
						while ((pos = respuesta.find("created_at")) !=string::npos) {
							repeticiones++;
						    token = respuesta.substr(0, pos);
						    //Quitamos el tweet original del retwiteado (esto no borra los rt si no que por culpa del json el tweet aparece 2 veces cuando esta retuiteado, deja 1)
						    if(!token.find("retweeted_status")!=string::npos){
						    	tweet=StringToTweet(token);
						    	Fichero<<tweet.fecha<<";"<<tweet.texto<<endl;
						    }
						    respuesta.erase(0, pos + 10);
						}



		        		//Veces que encuentra un tweet en el json si encuentra menos de 3 es seguro que ya no quedan tweets por mirar
						if(repeticiones<3)
							tweetscompletos=true;;

		    	}else{
			    	printf( "\nSe acabó\n" );
			    	tweetscompletos=true;
			       // twitterObj.getLastCurlError( replyMsg );
		    	}  
		 	}
	     }



	     ///////////////////////////////////////////////////////
	    //Obtener los amigos de un usuario/////////////////////
	    ///////////////////////////////////////////////////////

	    replyMsg = "";
	    tmpStr = "techcrunch";
	    if( twitterObj.friendsIdsGet( "", to_string(UsuarioID),true ) )
	    {
	        //Pedimos los ids de los usuarios y los separamos en un vector/////////
	        twitterObj.getLastWebResponse( replyMsg );
	        string respuesta=replyMsg.c_str();
	        vector<string> respuestas= split(respuesta,']');
	        respuesta=respuestas[0].substr(8,respuestas[0].length()-8);
	        vector<string> ids= split(respuesta,',');


	        //Miramos si lo hemos mirado y si esta por mirar/////////////////////
	        bool EstaEnUsuariosSinMirar=false;
	        bool EstaEnUsuariosMirados=false;

	        for (int i=0; i<ids.size(); i++){
	        	int EnteroId=atoi(ids[i].c_str());
	        	for (int t=0; t<UsuariosSinMirar.size(); t++)
	        		if(EnteroId==UsuariosSinMirar[t])
	        			EstaEnUsuariosSinMirar=true;
	        	for (int j=0; j<UsuariosMirados.size(); j++)
	        		if(EnteroId==UsuariosMirados[j])
	        			EstaEnUsuariosMirados=true;
	        	//Si no esta en ninguna de las 2 listas lo añadimos a la lista que vamos a mirar
	        	if(!EstaEnUsuariosMirados && !EstaEnUsuariosSinMirar)
	        	 	UsuariosSinMirar.push_back(EnteroId);	
			}




	      //  cout<<respuesta;
	       // printf( "\ntwitterClient:: twitCurl::friendsIdsGet web response:\n%s\n", replyMsg.c_str() );
	    }
	    else
	    {
	        twitterObj.getLastCurlError( replyMsg );
	        printf( "\ntwitterClient:: twitCurl::friendsIdsGet error:\n%s\n", replyMsg.c_str() );
	    }


 	}
    //Cerrar fichero
    Fichero.close();
}