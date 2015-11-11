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

#include <chrono> //para los cronometros


#include <omp.h> 
using namespace std;



///////////////////////////////////////////////////////////////
//Constantes globales
//////////////////////////////////////////////////////////////
const int MAX_USUARIOS_A_MIRAR=10;
const int MAX_BUSCARAMIGOS=2; //maximo de veces que se buscan amigos si es 1 solo buscara los amigos de jorgeazorin si es mas busca tambien los amigos de los amigos de jorgeazorin
const string USUARIO_INICIO="144533310"; //Este es @jorgeazorin
string palabrabuscada="ébola";

//////////////////////////////////////////////////////////////
//Clase Tweet
///////////////////////////////////////////////gma//////////////
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


vector<string> obtenerUsuarios() {
	twitCurl twitterObj;


	//en este vector guardaremos los usuarios que aun no hemos mirado y leido sus tuits
	vector<string> UsuariosSinMirar;

	//en este vector se van almacenando los usuarios que ya hayamos visto
	vector<string> UsuariosMirados;

	//INICIALIZAR EL OBJETO DE TWITCURL
	twitterObj.setTwitterUsername( "jorgeazorin" );
    twitterObj.setTwitterPassword( "179832" );
    //memset( tmpBuf, 0, 2048 );
	twitterObj.getOAuth().setConsumerKey( std::string( "ycPUlEPhZVdxushiDdXbNcDUH" ) );
 	twitterObj.getOAuth().setConsumerSecret( std::string( "zJW9NJY8IlOYoaG4zr1LEBdeHcTfKZ2mbTeI9WzcQ4Q19KJT0a" ) );


    //Añadimos el usuario Inicial a la lista
    UsuariosSinMirar.push_back(USUARIO_INICIO);


	twitterObj.friendsIdsGet( "",USUARIO_INICIO,true );
 	//Pedimos los ids de los usuarios y los separamos en un vector/////////
 	string replyMsg = "";
    twitterObj.getLastWebResponse( replyMsg );
    string respuesta=replyMsg.c_str();
    //cout << "LA RESPUESTA ES " << endl << respuesta << endl;
    vector<string> respuestas= split(respuesta,']');
    respuesta=respuestas[0].substr(8,respuestas[0].length()-8);
    vector<string> ids= split(respuesta,',');

   
    //Miramos si lo hemos mirado y si esta por mirar/////////////////////
    bool EstaEnUsuariosSinMirar=false;
    bool EstaEnUsuariosMirados=false;

    //AQUI QUIZAS PODRIAMOS PONER PARALELIZACION PRAGMA PARALLEL FOR
    //AUNQUE QUIZAS AL ESTAR HACIENDO PUSH_BACKS EN USUARIOSSINMIRAR
    //PASE ALGO, NUSENUSE
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
    	 	UsuariosSinMirar.push_back(id);	
    	}
	}

	return UsuariosSinMirar;
}

void crearCSV(unordered_map<int, int> VecesPorFecha) {
	//Crear un archivo con los resultados//////////////////
    ofstream Fichero("datos.csv");         //Opening file to print info to
    Fichero << "Fecha"<<";"<<"Numero de tweets" << endl;          //Headings for file

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
    //Cerrar fichero
    Fichero.close();
}


//////////////////////////////////////////////////////////////
//                        MAIN                            ////
//////////////////////////////////////////////////////////////
int main( int argc, char* argv[] )
{
	//***C++11 Style:***
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    //pasamos a minusculas la palabra a buscar
	for(int i = 0; i<palabrabuscada.length(); i++)
		palabrabuscada[i] = tolower(palabrabuscada[i]);

	//le quitamos los acentos a la palabra
	QuitarAcentos(palabrabuscada);			

	//VARIABLES CRITICAS
	//numero de usuarios que se han mirado hasta el momento
	unsigned int usuariosMirados=0;

	//numero de tweets que se han mirado
	unsigned int tweetsMirados=0;

	//veces que aparece la palabra
	unsigned int vecesQueApareceLaPalabra=0;

	//mapa de clave fecha (entero) y valor numero de veces que se ha encontrado la palabra
	//en esa fecha
	unordered_map<int, int> VecesPorFecha;


	//conseguimos una lista de usuarios
    vector<string> usuarios;
    usuarios = obtenerUsuarios();


	//AHORA COGER UNO A UNO AL USUARIO Y LEER SUS TUITS
	///////////////////////////////////////////////////////
    //Obtener los tweets de un usuario/////////////////////
    ///////////////////////////////////////////////////////


    //PARALELIZANDO LA COSA
	omp_set_num_threads(omp_get_num_procs());
	//omp_set_num_threads(0);

	//para conseguir el correcto numero usuariosmirados
	//hay que ir sumando en la zona critica
	int usuariosMiradosThread;

	//para conseguir el numero correcto de tweets en thread
	//despues en la zoma critica se les añade
	int tweetsMiradosThread;

	//para conseguir el numero correcto de veces en thread
	//despues en la zona critica se les añade
	int vecesQueApareceLaPalabraThread;


	#pragma omp parallel private(usuariosMiradosThread,tweetsMiradosThread,vecesQueApareceLaPalabraThread)
	{
		usuariosMiradosThread=0;
		tweetsMiradosThread=0;
		vecesQueApareceLaPalabraThread=0;
		string replyMsg="";
	    for(int i=omp_get_thread_num();i<usuarios.size() && i<MAX_USUARIOS_A_MIRAR;i+=omp_get_num_threads()) 
	    {
	    	twitCurl api;
		    //INICIALIZAR EL OBJETO DE TWITCURL
			api.setTwitterUsername( "jorgeazorin" );
		    api.setTwitterPassword( "179832" );
			api.getOAuth().setConsumerKey( std::string( "ycPUlEPhZVdxushiDdXbNcDUH" ) );
		 	api.getOAuth().setConsumerSecret( std::string( "zJW9NJY8IlOYoaG4zr1LEBdeHcTfKZ2mbTeI9WzcQ4Q19KJT0a" ) );

	    	usuariosMiradosThread++;
	    	string UsuarioID = usuarios[i];
	    	//cout << "Faltan por analizar los tweets de " << (MAX_USUARIOS_A_MIRAR-i) << " usuarios" << endl;
	    	cout << "EL THREAD " << omp_get_thread_num() << " VA A MIRAR EL USER " << UsuarioID << "EN LA ITERACION " << i << endl;

	    	
		    bool tweetscompletos=false;
		    string ultimotweet="";
		    ///Mientas la respuesta no sea un json vacio va a ir pidiendo tweets desde el ultimo recibido

		    while(!tweetscompletos) {
		    	tweetscompletos=true;
		    	if( api.timelineUserGet(true, false,200,UsuarioID,true,ultimotweet)){//trimUser //retweets // nº //usuario//esusuarioID//ultimotweet
			        api.getLastWebResponse( replyMsg );
			        string respuesta=replyMsg.c_str();
			        if(respuesta.substr(2,6)!="errors" && respuesta.length()>50 ){
		        		tweetscompletos=false;
		        		//cout<<"Mirando tweets desde "<<ultimotweet<<endl;

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
								tweetsMiradosThread++;


						    	//pasamos a minusculas el tweet y le quitamos los acentos
						    	for(int i = 0; i<tweet.texto.length(); i++)
										tweet.texto[i] = tolower(tweet.texto[i]);
								QuitarAcentos(tweet.texto);


						    	if(tweet.texto.find(palabrabuscada)!=string::npos){
						    		vecesQueApareceLaPalabraThread++;
					    			//Sumar a la fecha 1 si lo encuentra
					    			unordered_map<int,int>::const_iterator got = VecesPorFecha.find (tweet.fecha);
									if (got == VecesPorFecha.end() )
									{
				    					pair<int,int> parfehcanumerotweets (tweet.fecha,1);
				    					VecesPorFecha.insert(parfehcanumerotweets);
			    					}
			    					else
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
					    	api.getOAuth().setConsumerKey( std::string( "2Kdg60HDmZEu2NXIp7MRMBQIm" ) );
		   					api.getOAuth().setConsumerSecret( std::string( "IQcdiWnJd1bsWHytbLmSZa4aNxlPJ5Jr9ZjwOPjiDp31Tyactn" ) );
							cout << "SE HA CAMBIADO DINAMICAMENTE EL APP KEY, A VER SI ASI SIGUE FUNCIONANDO" << endl;
						}
			    	}  
			 	}

		    }
		}
		#pragma omp critical
		{
			usuariosMirados+=usuariosMiradosThread;
			tweetsMirados+=tweetsMiradosThread;
			vecesQueApareceLaPalabra+=vecesQueApareceLaPalabraThread;
		}
	}


	crearCSV(VecesPorFecha);

    std::chrono::steady_clock::time_point end= std::chrono::steady_clock::now();

    cout<<endl<<endl<<"Total tweets mirados "<<tweetsMirados << " de "<< usuariosMirados <<" usuarios"<<endl;
    cout<<"la palabra "<<palabrabuscada<<" aparece "<<vecesQueApareceLaPalabra<<" veces"<<endl;
   

	//std::cout << "Time difference = " << chrono::duration_cast<chrono::microseconds>(end - begin).count() << " microsegundos" << std::endl;
	//std::cout << "Time difference = " << chrono::duration_cast<chrono::nanoseconds> (end - begin).count() << " nanosegundos" << std::endl;
	std::cout << "Time difference = " << chrono::duration_cast<chrono::milliseconds> (end - begin).count() << " milisegundos" << std::endl;
	std::cout << "Time difference = " << chrono::duration_cast<chrono::seconds> (end - begin).count() << " segundos" << std::endl;
}
