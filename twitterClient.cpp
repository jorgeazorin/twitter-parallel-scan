#include <cstdio>
#include <iostream>
#include <fstream>
#include "libtwitcurl/twitcurl.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <ctype.h>
using namespace std;



///////////////////////////////////////////////////////////////
//Constantes globales
//////////////////////////////////////////////////////////////
const int MAX_USUARIOS_A_MIRAR=200;
const int MAX_BUSCARAMIGOS=2; //maximo de veces que se buscan amigos si es 1 solo buscara los amigos de jorgeazorin si es mas busca tambien los amigos de los amigos de jorgeazorin
const string USUARIO_INICIO="144533310"; //Este es @jorgeazorin
string palabrabuscada="ébola";

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
	if	   (mes=="Jan") return "01"; 	else if(mes=="Feb") return "02";
	else if(mes=="Mar") return "03";	else if(mes=="Apr") return "04";
	else if(mes=="May") return "05";	else if(mes=="Jun") return "06";
	else if(mes=="Jul") return "07";	else if(mes=="Aug") return "08";
	else if(mes=="Sep") return "09";	else if(mes=="Oct") return "10";
	else if(mes=="Nov") return "11";	else if(mes=="Dec") return "12";
	else         		return "00";
}




//////////////////////////////////////////////////////////////
//Modulo para obtener fecha y texto de un tweet
/////////////////////////////////////////////////////////////
Tweet StringToTweet(string t){
	Tweet tweet;
	///Obtener la fecha del tweet//////////////////
 	vector<string> fecha = split(t.substr(3,30), ' ');
 	string fechastring=fecha[2]+MesToNum(fecha[1])+fecha[5];
 	int fechaint = atoi(fechastring.c_str());   
 	tweet.fecha=fechaint;    

	///Obtener el texto del tweet//////////////////
	tweet.texto=t.substr(t.find("text")+7,t.find("source")-t.find("text")-10);
	return tweet;
}

string fechaInt2fechaString(int fechaint){
	string fechabruto="";
    string fechalimpia="";
	fechabruto=to_string(fechaint);

	//Si el dia es menor de 10 le ponemos un 0 delante
	if(fechabruto.length()<8)
		fechabruto="0"+fechabruto;

	//ponemos la fecha con /
	fechalimpia=fechabruto.at(0);	fechalimpia+=fechabruto.at(1);	fechalimpia+="/";  	//Dia
	fechalimpia+=fechabruto.at(2);	fechalimpia+=fechabruto.at(3);	fechalimpia+="/";	//Mes
	fechalimpia+=fechabruto.at(4);	fechalimpia+=fechabruto.at(5); 	fechalimpia+=fechabruto.at(6); fechalimpia+=fechabruto.at(7); //Año
	return fechalimpia;
}



//////////////////////////////////////////////////////////////
//Modulo para reemplazar todas las veces un texto de otro
/////////////////////////////////////////////////////////////
void replaceAll(string& str, const string& from, const string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}






//////////////////////////////////////////////////////////////
//Modulo para quitar los acentos de un texto
/////////////////////////////////////////////////////////////
void QuitarAcentos(string& texto){
	//minusculas en json				//mayusculas en json				//minusculasen unicode		//mayusculas en unicode
	replaceAll(texto,"\\u00e1","a");	replaceAll(texto,"\\u00c1","a");	replaceAll(texto,"á","a");	replaceAll(texto,"Á","a");
	replaceAll(texto,"\\u00e9","e");	replaceAll(texto,"\\u00c9","e");	replaceAll(texto,"é","e");	replaceAll(texto,"É","e");
	replaceAll(texto,"\\u00ed","i");	replaceAll(texto,"\\u00cd","i");	replaceAll(texto,"í","i");	replaceAll(texto,"Í","i");
	replaceAll(texto,"\\u00f3","o");	replaceAll(texto,"\\u00d3","o");	replaceAll(texto,"ó","o");	replaceAll(texto,"Ó","o");
	replaceAll(texto,"\\u00fa","u");	replaceAll(texto,"\\u00dA","u");	replaceAll(texto,"ú","u");	replaceAll(texto,"Ú","u");
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
    std::string replyMsg;
    char tmpBuf[2048];
    twitterObj.setTwitterUsername( "jorgeazorin" );
    twitterObj.setTwitterPassword( "179832" );
    memset( tmpBuf, 0, 2048 );
	twitterObj.getOAuth().setConsumerKey( std::string( "ycPUlEPhZVdxushiDdXbNcDUH" ) );
 	twitterObj.getOAuth().setConsumerSecret( std::string( "zJW9NJY8IlOYoaG4zr1LEBdeHcTfKZ2mbTeI9WzcQ4Q19KJT0a" ) );

//  	twitterObj.getOAuth().setConsumerKey( std::string( "2Kdg60HDmZEu2NXIp7MRMBQIm" ) );
//    twitterObj.getOAuth().setConsumerSecret( std::string( "IQcdiWnJd1bsWHytbLmSZa4aNxlPJ5Jr9ZjwOPjiDp31Tyactn" ) );
    replyMsg = "";					



    //Crear un archivo con los resultados//////////////////
    ofstream Fichero("datos.csv");         //Opening file to print info to
    Fichero << "Fecha"<<";"<<"Numero de tweets" << endl;          //Headings for file

    //pasamos a minusculas la palabra a buscar
	for(int i = 0; i<palabrabuscada.length(); i++)
		palabrabuscada[i] = tolower(palabrabuscada[i]);

	//le quitamos los acentos a la palabra
	QuitarAcentos(palabrabuscada);



    ///////////////////////////////////////////////////////
    //Recorrer la lista de usuarios que no se han visitado/
    ///////////////////////////////////////////////////////

	int iteradorusuarios=0;

	//id del ultimo tweet desde que lee la api
	string ultimotweet;

	//numero de usuarios que se han mirado hasta el momento
	unsigned int usuariosMirados=0;

	//numero de tweets que se han mirado
	unsigned int tweetsMirados=0;

	//veces que aparece la palabra
	unsigned int vecesQueApareceLaPalabra=0;

	//en este vector guardaremos los usuarios que aun no hemos mirado y leido sus tuits
	vector<string> UsuariosSinMirar;

	//en este vector se van almacenando los usuarios que ya hayamos visto
	vector<string> UsuariosMirados;


	//mapa de clave fecha (entero) y valor numero de veces que se ha encontrado la palabra
	//en esa fecha
	unordered_map<int, int> VecesPorFecha;

    //Añadimos el usuario Inicial a la lista
    UsuariosSinMirar.push_back(USUARIO_INICIO);
    while(usuariosMirados<MAX_USUARIOS_A_MIRAR && !UsuariosSinMirar.empty()){
    	//cout<<"Quedan por analizar los tweets de "<<UsuariosSinMirar.size() << " usuarios";
    	cout<<"Quedan por analizar los tweets de "<< MAX_USUARIOS_A_MIRAR-usuariosMirados << " usuarios";
    	usuariosMirados++;
    	ultimotweet="";
    	string UsuarioID=*UsuariosSinMirar.begin();
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
		    if( twitterObj.timelineUserGet(true, false,1000000,UsuarioID,true,ultimotweet)){//trimUser //retweets // nº //usuario//esusuarioID//ultimotweet
		        twitterObj.getLastWebResponse( replyMsg );
		        string respuesta=replyMsg.c_str();

		      // cout<<endl<<respuesta.substr(3,6)<<endl;

		        if(respuesta.substr(2,6)!="errors" && respuesta.length()>50 ){
	        		tweetscompletos=false;
	        		cout<<"Mirando tweets desde "<<ultimotweet<<endl;

			        //Dividir el json de respuesta entre tweets//////////////////////////////////////////////
			        Tweet tweet;
			        size_t pos = 0;
					string token;

					//Veces que encuentra un tweet en el json si encuentra menos de 3 es seguro que ya no quedan tweets por mirar
		        	int repeticiones=0;
					while ((pos = respuesta.find("created_at")) !=string::npos) {
						repeticiones++;
					    token = respuesta.substr(0, pos);
					    //Quitamos el tweet original del retwiteado (esto no borra los rt si no que por culpa del json el tweet aparece 2 veces cuando esta retuiteado, deja 1)
					    if(!token.find("retweeted_status")!=string::npos && token.length()>35){
					    	tweet=StringToTweet(token);
					    	///Obtener el id del tweet/////////////////////
					    	ultimotweet=token.substr(token.find("id")+4,token.find("id_str")-token.find("id")-6);
							tweetsMirados++;


					    	//pasamos a minusculas el tweet y le quitamos los acentos
					    	for(int i = 0; i<tweet.texto.length(); i++)
  								tweet.texto[i] = tolower(tweet.texto[i]);
  							QuitarAcentos(tweet.texto);


					    	if(tweet.texto.find(palabrabuscada)!=string::npos){
					    		vecesQueApareceLaPalabra++;
				    			//Sumar a la fecha 1 si lo encuentra
				    			unordered_map<int,int>::const_iterator got = VecesPorFecha.find (tweet.fecha);
									if (got == VecesPorFecha.end() ){
				    				pair<int,int> parfehcanumerotweets (tweet.fecha,1);
				    				VecesPorFecha.insert(parfehcanumerotweets);
			    				}else
				    				VecesPorFecha.at(tweet.fecha)=VecesPorFecha.at(tweet.fecha)+1;
					    	}
					    }
					    respuesta.erase(0, pos + 10);
					}
	        		//Veces que encuentra un tweet en el json si encuentra menos de 3 es seguro que ya no quedan tweets por mirar
					if(repeticiones<3)
						tweetscompletos=true;

		    	}else{
			    	cout<<respuesta<<endl;
			    	if(respuesta.substr(2,6)=="errors")
			    	{
				    	twitterObj.getOAuth().setConsumerKey( std::string( "2Kdg60HDmZEu2NXIp7MRMBQIm" ) );
	   					twitterObj.getOAuth().setConsumerSecret( std::string( "IQcdiWnJd1bsWHytbLmSZa4aNxlPJ5Jr9ZjwOPjiDp31Tyactn" ) );
						cout << "SE HA CAMBIADO DINAMICAMENTE EL APP KEY, A VER SI ASI SIGUE FUNCIONANDO" << endl;
					}
		    	}  
		 	}
	    }



	     ///////////////////////////////////////////////////////
	    //Obtener los amigos de un usuario/////////////////////
	    ///////////////////////////////////////////////////////
	    if(UsuariosSinMirar.size()<100){

	    	string UsuarioID=UsuariosMirados[iteradorusuarios];
	     	iteradorusuarios++;

		    replyMsg = "";
		    if( twitterObj.friendsIdsGet( "",UsuarioID,true ) && usuariosMirados<=MAX_BUSCARAMIGOS){

		    	cout<<"Buscando amigos de "<<UsuarioID<<endl;
		        //Pedimos los ids de los usuarios y los separamos en un vector/////////
		        twitterObj.getLastWebResponse( replyMsg );
		        string respuesta=replyMsg.c_str();
		        if(respuesta.find("error") !=string::npos)
		        	cout<<respuesta<<endl;
		        vector<string> respuestas= split(respuesta,']');
		        respuesta=respuestas[0].substr(8,respuestas[0].length()-8);
		        vector<string> ids= split(respuesta,',');

		       
		        //Miramos si lo hemos mirado y si esta por mirar/////////////////////
		        bool EstaEnUsuariosSinMirar=false;
		        bool EstaEnUsuariosMirados=false;

		        for (int i=0; i<ids.size(); i++){
		        	EstaEnUsuariosSinMirar=false;
		       		EstaEnUsuariosMirados=false;

		        	//int EnteroId=atoi(ids[i].c_str());
		        	string id = ids[i];

		        	for (int t=0; t<UsuariosSinMirar.size(); t++)
		        		if(id==UsuariosSinMirar[t])
		        			EstaEnUsuariosSinMirar=true;


		        	for (int j=0; j<UsuariosMirados.size(); j++)
		        		if(id==UsuariosMirados[j])
		        			EstaEnUsuariosMirados=true;
		        	//Si no esta en ninguna de las 2 listas lo añadimos a la lista que vamos a mirar
		        	if(!EstaEnUsuariosMirados && !EstaEnUsuariosSinMirar)
		        	{
		        		//cout << "SE VA A AÑADIR " << EnteroId << endl;
		        	 	UsuariosSinMirar.push_back(id);	
		        	}
				}
		    }
		}
 	}
    

    for(int a=2006;a<=2016;a++){
    	for(int m=1;m<=12;m++){
    		int veces=0;
    		for(int d=1;d<=31;d++){
    			int f=(d*1000000)+(m*10000)+a;
    			unordered_map<int,int>::const_iterator got = VecesPorFecha.find (f);
				if (got != VecesPorFecha.end() )
					veces+=VecesPorFecha.at(f);
    		}
    		Fichero<<m<<"/"<<a<<";"<<veces<<endl;
    		cout<<m<<"/"<<a<<";"<<veces<<endl;
    	}
    }
    cout<<endl<<endl<<"Total tweets mirados "<<tweetsMirados << " de "<< usuariosMirados <<" usuarios"<<endl;
    cout<<"la palabra "<<palabrabuscada<<" aparece "<<vecesQueApareceLaPalabra<<" veces"<<endl;
   

    //Cerrar fichero
    Fichero.close();
}
