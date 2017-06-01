#ifndef OPRS_MP
# define OPRS_MP

//Used for connection to OPRS Message Passer
#include <list>
#include "macro-pub.h"
#include "constant-pub.h"
#include "oprs-type-pub.h"
#include "mp-pub.h"

using namespace std;



void *Producer(void*);


class Oprslink
{
	public:
	
	  ~Oprslink();
	  int m_Port;
	  int m_Socket;
	  
	  void init(string);
	  list<string> read();
	  void write(const char*);
	 
	  
	private:
	  
	  pthread_t pid;  
	  pthread_mutexattr_t MutexAttr;
};


	

#endif // oprsMP
