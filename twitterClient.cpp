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
#include <mpi.h>
//para mandar los maps en json y reconvertirlos
//#include "jsoncpp/json/json.h"
#include "jsoncpp/jsoncpp.cpp"

using namespace std;

//////////////////////////////////////////////////////////////
//Clase Tweet
/////////////////////////////////////////////////////////////
class Tweet {
	public:
		int fecha=0;
		string texto="";
};


//////////////////////////////////////////////////////////////
//Modulo para dividir string mediante un caracter delimitador
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
//Modulo para Buscar amigos
/////////////////////////////////////////////////////////////
vector<string> obtenerUsuarios(int max, string usuarioInicial) {
	//en este vector guardaremos los usuarios que vamos encontrando
	vector<string> usuarios; //Lista de usuarios final
	string respuesta; //La respuesta del api de de twitter
    vector<string> respuestas, ids; //vector de la respuesta dividida en ids y los ids finales

	//INICIALIZAR EL OBJETO DE TWITCURL
	twitCurl twitterObj;
	twitterObj.setTwitterUsername( "jorgeazorin" );
    twitterObj.setTwitterPassword( "179832" );
	twitterObj.getOAuth().setConsumerKey( std::string( "ycPUlEPhZVdxushiDdXbNcDUH" ) );
 	twitterObj.getOAuth().setConsumerSecret( std::string( "zJW9NJY8IlOYoaG4zr1LEBdeHcTfKZ2mbTeI9WzcQ4Q19KJT0a" ) );
    //Añadimos el usuario Inicial a la lista
    usuarios.push_back(usuarioInicial);
    
    for(int i=0; usuarios.size()<max && i<usuarios.size() ; i++){
    	string replyMsg = "";

    	bool esUsuarioUnID=true;	//los demas usuarios son representados por su id
    	if(usuarios[i]==usuarioInicial) //solo el usuario inicial sera texto
    		esUsuarioUnID=false;
	 	cout<<"Obteniendo amigos de "+ usuarios[i]<<endl;
	 	//Pedimos los ids de sus amigos y los separamos en un vector/////////
		twitterObj.friendsIdsGet( "",usuarios[i],esUsuarioUnID);
	    twitterObj.getLastWebResponse( replyMsg );
	    respuesta=replyMsg.c_str();
	    respuestas= split(respuesta,']');
	    respuesta=respuestas[0].substr(8,respuestas[0].length()-8);
	    ids= split(respuesta,',');

	    //Si no está ya en la lista de usuarios lo añadimos
	    for (int i=0; i<ids.size(); i++){
	   		bool EstaEnUsuarios=false;
	    	for (int t=0; t<usuarios.size(); t++)
	    		if(ids[i]==usuarios[t])
	    			EstaEnUsuarios=true;
	    	if(!EstaEnUsuarios)
	    	 	usuarios.push_back(ids[i]);
		}
	}
	return usuarios;
}


//////////////////////////////////////////////////////////////
//Modulo para convertir el resultado en un csv
/////////////////////////////////////////////////////////////
void crearCSV(unordered_map<int, int> VecesPorFecha) {
	//Crear un archivo con los resultados//////////////////
    ofstream Fichero("datos.csv");
    Fichero << "Fecha"<<";"<<"Numero de tweets" << endl; //los nombres de las "columnas"

	for(int a=2006;a<=2015;a++){
    	for(int m=1;m<=12;m++){
    		int veces=0;
    		for(int d=1;d<=31;d++){
    			int f=(d*1000000)+(m*10000)+a;
    			unordered_map<int,int>::const_iterator got = VecesPorFecha.find (f);
				if (got != VecesPorFecha.end() )
					veces+=VecesPorFecha.at(f);
    		}
    		Fichero<<m<<"/"<<a<<";"<<veces<<endl;
    	}
    }
    //Cerrar fichero
    Fichero.close();
    cout<<endl<<"Resultados en datos.csv preparados para excel"<<endl<<endl;
}




//////////////////////////////////////////////////////////////
//Modulo para explicar cómo ejecutar correctamente el binario
/////////////////////////////////////////////////////////////
void uso(const char* nombre_programa) {
	cout <<endl<< "Uso: " << nombre_programa << " palabra num_usuarios (usuario_inicial)" << endl;
	cout << "Ejemplo: " << nombre_programa << " fútbol 20 (domingogallardo) "<<endl <<"        El usuario inicial es opcional" << endl<<endl;
}


//////////////////////////////////////////////////////////////
//Modulo para quitar acentos y poner en minúsculas
/////////////////////////////////////////////////////////////
void limpiarpalabra(string& palabrabuscada){
	for(int i = 0; i<palabrabuscada.length(); i++)
	palabrabuscada[i] = tolower(palabrabuscada[i]);

	//le quitamos los acentos a la palabra
	QuitarAcentos(palabrabuscada);		
}


//////////////////////////////////////////////////////////////
//Funcion que usa la api para leer tweets de un usuario y va
//incrementando los contadores locales de cada hilo
/////////////////////////////////////////////////////////////
bool BuscarTweetsUsuario(string palabrabuscada, string UsuarioID, unordered_map<int, int>& VecesPorFecha, int& tweetsMiradosThread, int& vecesQueApareceLaPalabraThread, twitCurl& api, bool elIDesNombre){

	//vamos a usar 2 api keys, si las usamos las 2 y sigue habiendo restriccion, fin del programa...
	int num_api_key=0;
    bool tweetscompletos=false;
    string ultimotweet="";
    string replyMsg="";
    string respuesta="";
    ///Mientas la respuesta no sea un json vacio va a ir pidiendo tweets desde el ultimo recibido
    while(!tweetscompletos) {
    	tweetscompletos=true;
    	if( api.timelineUserGet(true, false,200,UsuarioID,!elIDesNombre,ultimotweet)){//trimUser //retweets // nº //usuario//esusuarioID//ultimotweet
	        replyMsg="";
	        api.getLastWebResponse( replyMsg );
	        respuesta=replyMsg.c_str();
	        if(respuesta.substr(2,6)!="errors" && respuesta.length()>50 ){
        		tweetscompletos=false;
		        
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
						//incrementamos la cantidad local de tweets mirados
						tweetsMiradosThread++;

				    	//quitamos acentos y minusculas
						limpiarpalabra(tweet.texto) ;

						//si el texto contiene la palabra
				    	if(tweet.texto.find(palabrabuscada)!=string::npos){
				    		vecesQueApareceLaPalabraThread++;
			    			//Sumar a la fecha 1 si lo encuentra
			    			unordered_map<int,int>::const_iterator got = VecesPorFecha.find (tweet.fecha);
							if (got == VecesPorFecha.end() ){
		    					pair<int,int> parfechanumerotweets (tweet.fecha,1);
		    					VecesPorFecha.insert(parfechanumerotweets);
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
	    	}
	    	else{
		    	if(respuesta.substr(2,6)=="errors")
		    	{
		    		num_api_key++;
		    		if(num_api_key>1) //ya ha probado las 2 api key, asi que las dos estan restringidas
		    		{
		    			cout << "Las dos keys se han restringido, habrá que esperar 15 minutos..." << endl;
		    			return false; //ya no podemos leer tuits
		    		}
		    		else{ //si aun le queda 1 api key por probar, usarla
			    		cout<<"La primera key se ha restringido, vamos a usar ahora la segunda key..."<<endl;
			    		tweetscompletos=false; //lo ponemos a falso, para que siga intentando leer tuits
			    		api.setTwitterUsername( "jorgeazorin" );
						api.setTwitterPassword( "179832" );
				    	api.getOAuth().setConsumerKey( std::string( "2Kdg60HDmZEu2NXIp7MRMBQIm" ) );
	   					api.getOAuth().setConsumerSecret( std::string( "IQcdiWnJd1bsWHytbLmSZa4aNxlPJ5Jr9ZjwOPjiDp31Tyactn" ) );
   					}
				}
	    	}  
	 	}
    }
    return true;
}

string MapToJSON(unordered_map<int, int>& VecesPorFecha, int identificadorProceso) {

	Json::Value jsonMap;
	unordered_map<int,int>::const_iterator it = VecesPorFecha.begin(), end = VecesPorFecha.end();
	for ( ; it != end; ++it) {
	    jsonMap[to_string(it->first)] = it->second;
	    // ^ beware: std::to_string is C++11
	}

	Json::Value root;
	root[to_string(identificadorProceso)]	= jsonMap;
	Json::StyledWriter writer;
	string output = writer.write(root);
	return output;
}

unordered_map<int, int> JSONToMap(string cadena_json, int identificadorProceso) {
	Json::Value nuevo_json;
	Json::Reader reader;
	bool parsedSuccess = reader.parse(cadena_json,nuevo_json,false);
	if(parsedSuccess) {
		unordered_map<int, int> mapa;
		nuevo_json = nuevo_json[to_string(identificadorProceso)];
		for(Json::ValueIterator it = nuevo_json.begin();it!=nuevo_json.end();it++) {
			int clave = stoi(it.key().asString());
			int valor = stoi((*it).asString());
			pair<int,int> par (clave,valor);
			mapa.insert(par);
		}
		return mapa;
	}
}



//////////////////////////////////////////////////////////////
//                        MAIN                            ////
//////////////////////////////////////////////////////////////
int main( int argc, char* argv[] )
{
	
	string palabrabuscada;
	int MAX_USUARIOS_A_MIRAR;
	string usuarioInicial;

	//si hay 4 argumentos (sin contar ./twitterClient)
	//entonces usamos el usuario por parametro
	if(argc==4) {
		palabrabuscada = argv[1];
		MAX_USUARIOS_A_MIRAR = atoi(argv[2]);
		usuarioInicial=argv[3];
	//si hay 3, usamos el usuario por defecto
	}else if(argc==3){
		palabrabuscada = argv[1];
		MAX_USUARIOS_A_MIRAR = atoi(argv[2]);
		usuarioInicial="jorgeazorin";
	}
	else
	{
		uso(argv[0]);
		return 1;
	}



	//Empezamos a contar el tiempo que dura la ejecución
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	//Variables globales, al final root les da valor
	unsigned int usuariosMirados=0; 			
	unsigned int tweetsMirados=0;				
	unsigned int vecesQueApareceLaPalabra=0;
	//mapa de clave fecha (entero) y valor numero de veces que se ha encontrado la palabra en esa fecha
	unordered_map<int, int> VecesPorFecha;



	//AQUI EMPIEZA EL PARALELISMO
    int myrank, nprocs;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	MPI_Barrier(MPI_COMM_WORLD); //barrera para sincronizar todos los procesos

	vector<string> usuarios_recibidos; //cada proceso tendra una copia, con los usuarios que le toca leer sus tweets

	if(myrank==0) 
	{ //EL PROCESO MAESTRO COGERA TODOS LOS USUARIOS, Y ENTONCES LOS REPARTIRA EQUITATIVAMENTE
		//Quitamos acentos y ponemos en minusculas la palabra a buscar
		limpiarpalabra(palabrabuscada) ;
		cout << "Se va a buscar la cadena " << palabrabuscada<< endl;

		//conseguimos una lista de usuarios
	    vector<string> usuarios = obtenerUsuarios(MAX_USUARIOS_A_MIRAR,usuarioInicial);

		for(int i=0;i<nprocs;i++) { //para cada proceso
			string resultado="";
			int tam_string=0;
			//si hay 3 procesos en total, el proceso 2 cogera el usuario 2,5,8...
			for(int j=i;j<MAX_USUARIOS_A_MIRAR;j+=nprocs) //va asignandose ids de usuarios
			{
				resultado= resultado + usuarios[j] + ";"; //vamos acumulando los ids de usuarios en un string con separadores
			}
			if(i!=0) //no hace falta mandarselo a si mismo
			{
				resultado+="."; //esto indica el fin del mensaje
				//el proceso root envia los usuarios asignados a los diferentes procesos
				MPI_Send(resultado.c_str(),resultado.size(),MPI_CHAR,i,i,MPI_COMM_WORLD);
			}
			else {
				usuarios_recibidos = split(resultado,';'); //el proceso 0 no necesita mandarse los usuarios, ya los tiene
			}
		}
	}
	else 
	{
		MPI_Status estado;
		//Probe es como receive pero no recibe el mensaje, solo hace una query para saber datos utiles como el tamaño del mensaje
		MPI_Probe(0,myrank,MPI_COMM_WORLD,&estado); //tenemos que usar probe primero, para obtener el tamaño del string
		int tam = 0;
		//cout << "AQUI LLEGA" << endl;
		MPI_Get_count(&estado,MPI_CHAR,&tam); //obtenemos el tamaño del mensaje
		char recibir[tam]; //con el tamaño, podemos instanciar un array de chars
		MPI_Recv(&recibir,tam,MPI_CHAR,0,myrank,MPI_COMM_WORLD,&estado); //guardamos en el array el mensaje recibido, del proceso 0, con el tag id de nuestro proceso
		//convertimos a string
		string str(recibir);
		str = split(str,'.')[0]; //cogemos el mensaje que queremos del buffer, hasta el separador que indicaba el fin de mensaje

		usuarios_recibidos = split(str,';'); //inicializamos el vector de usuarios, con los usuarios que iban separados por ;
	}

	//MPI_Barrier(MPI_COMM_WORLD); //barrera para sincronizar todos los procesos

	//Variables locales, cada proceso dispondrá de una copia
	int usuariosMiradosThread=0; //Número usuarios mirados por thread
	int tweetsMiradosThread=0;   //Número tweets mirados por thread
	int vecesQueApareceLaPalabraThread=0; //Número de veces que esta la palabra buscada por thread
	unordered_map<int, int> VecesPorFechaThread;

	
    //Inicializamos el objeto twitcurl, cada proceso tiene una copia
	twitCurl api;
	api.setTwitterUsername( "jorgeazorin" );
    api.setTwitterPassword( "179832" );
	api.getOAuth().setConsumerKey( std::string( "ycPUlEPhZVdxushiDdXbNcDUH" ) );
 	api.getOAuth().setConsumerSecret( std::string( "zJW9NJY8IlOYoaG4zr1LEBdeHcTfKZ2mbTeI9WzcQ4Q19KJT0a" ) );

 	string usuarioID; //variable que guarda el usuario al que se van a leer los tweets
    for(int i=0;i<usuarios_recibidos.size();i++){ //para cada usuario perteneciente a un proceso x
    	//incrementamos la variable local de usuarios mirados por hilo
    	usuariosMiradosThread++;
    	usuarioID = usuarios_recibidos[i];
    	cout << "Leidos tweets del usuario " << usuarioID <<" en el proceso "<< myrank << endl;
    	bool esNombreUsuario=false;
    	if (usuarioID==usuarioInicial) //solo el usuario inicial tiene como id su nickname
    		esNombreUsuario=true;
    	if(!BuscarTweetsUsuario(palabrabuscada, usuarioID, VecesPorFechaThread, tweetsMiradosThread, vecesQueApareceLaPalabraThread, api,esNombreUsuario))
    		break; //si las 2 keys estan totalmente restringidas, habra que esperar 15 minutos
	}


	/////////////////////////////////////////////////////////////////////////////////////////
	//ZONA CRITICA///////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	MPI_Barrier(MPI_COMM_WORLD); //barrera para sincronizar todos los procesos


	//RECOPILAR TODOS LOS DATOS DE LOS MAPAS
	if(myrank!=0) //para los procesos slave
	{
		string mapa_string = MapToJSON(VecesPorFechaThread,myrank);
		//mandara su mapa, al proceso padre, destinatario 0, y tag sera su id de procesador
		MPI_Send(mapa_string.c_str(),mapa_string.size(),MPI_CHAR,0,myrank,MPI_COMM_WORLD);
	}

	else { //para el proceso padre
		VecesPorFecha = VecesPorFechaThread; //se inicializa con los datos que ha recopilado el proceso padre
		//todos los procesos hijo DEBEN mandarle el mapa
		for(int p=1;p<nprocs;p++)
		{
			MPI_Status estado;
			//Probe es como receive pero no recibe el mensaje, 
			//solo hace una query para saber datos utiles como el tamaño del mensaje
			MPI_Probe(p,p,MPI_COMM_WORLD,&estado); 
			int tam = 0;
			MPI_Get_count(&estado,MPI_CHAR,&tam); //obtenemos el tamaño del mensaje
			char recibir[tam]; //con el tamaño, podemos instanciar un array de chars
			MPI_Recv(&recibir,tam,MPI_CHAR,p,p,MPI_COMM_WORLD,&estado);
			//convertimos a string
			string str(recibir);
			unordered_map<int, int> mapa_recibido = JSONToMap(str,p); //ya tenemos el mapa

			//en este bucle, se actualizara el mapa PRINCIPAL con los datos del mapa que se ha recibido
			for ( unsigned i = 0; i < mapa_recibido.bucket_count(); ++i) {
			    for ( auto local_it = mapa_recibido.begin(i); local_it!= mapa_recibido.end(i); ++local_it ){
			    	unordered_map<int,int>::const_iterator got = VecesPorFecha.find (local_it->first);
					if (got == VecesPorFecha.end() ){ //si esa entrada no existe, se añade
						pair<int,int> parfechanumerotweets (local_it->first,local_it->second);
						VecesPorFecha.insert(parfechanumerotweets);
					}
					else{ //si la entrada ya existe, se incrementa
						int i=local_it->second;
						VecesPorFecha.at(local_it->first)+=i;
					}
			    }
			}
		}
	}

	//vamos a enviar el numero de usuarios mirados locales al root, y que haga el sumatorio
	MPI_Reduce(&usuariosMiradosThread,&usuariosMirados,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
	//vamos a enviar el numero de tweets mirados locales al root, y que haga el sumatorio
	MPI_Reduce(&tweetsMiradosThread,&tweetsMirados,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
	//vamos a enviar el numero de veces que aparece la palabra localmente al root, y que haga el sumatorio
	MPI_Reduce(&vecesQueApareceLaPalabraThread,&vecesQueApareceLaPalabra,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD); //barrera para sincronizar todos los procesos

	/////////////////////////////////////////////////////////////////////////////////////////
	//FIN DE ZONA CRITICA///////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////


	//el proceso maestro imprime los resultados
	if(myrank==0) {
		//que esto solo lo haga el proceso 0
	    std::chrono::steady_clock::time_point end= std::chrono::steady_clock::now();
	    cout<<endl<<endl<<"Total tweets mirados "<<tweetsMirados << " de "<< usuariosMirados <<" usuarios"<<endl;
	    cout<<"la palabra "<<palabrabuscada<<" aparece "<<vecesQueApareceLaPalabra<<" veces"<<endl;
	   	std::cout << "Ha tardado = " << chrono::duration_cast<chrono::milliseconds> (end - begin).count() << " milisegundos" << std::endl;
		std::cout << "Ha tardado = " << chrono::duration_cast<chrono::seconds> (end - begin).count() << " segundos" << std::endl;
		//creamos un archivo .csv con los datos para representarlos en excel
		crearCSV(VecesPorFecha);
	}

	MPI_Finalize();
	return 0;
}
