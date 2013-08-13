// This C++ code illustrates how can a C++ program connect to the
// message passer, send commands, and receive messages (in a separate
// thread).


#include <iostream>

#include "oprsMP.hh"

pthread_mutex_t MutexMP;
list<string> replyList;

OprsLink::~OprsLink()
{
   pthread_join(pid, NULL);
}

void OprsLink::init(string addr)
{
   m_Port = 3300;		// This is the message passer default port.

   char const * m_addr = addr.c_str();

   m_Socket = external_register_to_the_mp_host_prot( "MY-CPP-PROGRAM", m_addr, m_Port, STRINGS_PT );

   //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
   /* thread ids and attributes */
   pthread_attr_t attr;
   pthread_attr_init(&attr);
   pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS);
   pthread_mutexattr_init(&MutexAttr);
   pthread_mutex_init(&MutexMP, &MutexAttr);

   cout << "\t\t\t[OprsLink] Message Passer Succesfully Connected" << std::endl;
   pthread_create(&pid, &attr, Producer, &m_Socket);
   //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
}

// Here we send command to an agent (PRS Kernel) named "DALA", tune for your own app.
void OprsLink::write(const char* cmd)
{
   // The command has been built and is sent over the socket
   send_message_string( const_cast< char* >(cmd) , const_cast< char* >("DALA") );

   std::cout << "\t\t\t[OprsLink] Sent message " << cmd << " to " << "DALA" << "\n";
}

list<string> OprsLink::read()
{
   list<string> retList;

   pthread_mutex_lock(&MutexMP);    

   while (!replyList.empty()) 
   {
       retList.push_front(replyList.front());
       replyList.pop_front();
   }

   pthread_mutex_unlock(&MutexMP);

   return retList;
}

void *Producer(void *arg)
{
   int *sock;
   sock = static_cast<int *>(arg);
   int length;

   while(true)
   {
       char* sender = read_string_from_socket(*sock, &length );
       char* message = read_string_from_socket(*sock, &length ); 

       std::cout << "\t\t\t[OprsLink] Got message " << message << " from sender " << sender << "\n";

       pthread_mutex_lock(&MutexMP);

       string msg(message);
       replyList.push_back(msg); 

       pthread_mutex_unlock(&MutexMP);
   }
}

