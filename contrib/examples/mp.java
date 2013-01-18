// this java code should show how a Java program can connect to the
// message passer and interact with prs kernels.  This code has a bug
// when it reads int on socket... assuming that they are less than
// 2**8. See my comment below. One should read the 4 bytes, make an
// int of it instead.

// If somebody correct this bug, please share the fix.

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.Socket;

import java.util.*;
import java.awt.*;
import javax.swing.*;
import java.awt.event.*;

/**
* This is a very light and dirty Message Passer (Open PRS) client in Java
* for testing purpose only. Based on STRINGS messages.  
* @author Stéphane Plais (LAAS) stephane.plais@free.fr
* @since 1.4
*/
public class MPClient extends JFrame implements ActionListener {
    public String OPRS_HOST = "space";
    public String OPRS_MANIP = "MANIP-SPACE";
    public String CLIENT_NAME = "JAVA-LAASKO";
    public int SOCKET_MP = 3300;
    public boolean debug = false;

    public PrintWriter ostream;
    public OutputStream out;
    public BufferedReader istream;
    public ListenMessagePasser lmp;
    public String sender;

    // Layout and constraints
    private GridBagConstraints constraints;
    private JLabel propice_host_label, propice_manip_label, client_name_label, 
	socket_mp_label, msg_to_send_label, msg_received_label;
    private JTextField msg_to_send_text, propice_host_text, propice_manip_text, 
	client_name_text, socket_mp_text;
    private JButton connect_button, send_button;
    public JTextArea msg_received_textarea; 

    // return states
    private int ERROR = -1;
    private int OK = 1;

    /**
     * Constructor
     */
    public MPClient() {
   	this.setSize(800,400);
   	this.setTitle("Message Passer very light client");

   	// components of the interface
   	propice_host_label = new JLabel("Propice host : ");
   	propice_host_text = new JTextField(OPRS_HOST, 10);
   	propice_manip_label = new JLabel("Propice manip : ");
   	propice_manip_text = new JTextField(OPRS_MANIP, 10);
   	client_name_label = new JLabel("Client name : ");
   	client_name_text = new JTextField(CLIENT_NAME, 10);
   	socket_mp_label = new JLabel("MP Socket : ");
   	socket_mp_text = new JTextField(""+SOCKET_MP+"", 10);
   	msg_to_send_label = new JLabel("Message to send :");
   	msg_to_send_text = new JTextField(40);
   	msg_to_send_text.setEnabled(false);
   	connect_button = new JButton("Connect to MP");
   	connect_button.addActionListener(this);
   	send_button = new JButton("Send message");
   	send_button.addActionListener(this);
   	send_button.setEnabled(false);
   	msg_received_label = new JLabel("Messages received :");
   	msg_received_textarea = new JTextArea("", 10, 40);
   	msg_received_textarea.setEditable(false);
   	
   	// layout
   	this.getContentPane().setLayout(new GridBagLayout());
   	constraints = new GridBagConstraints();
   	constraints.fill = GridBagConstraints.BOTH;
   	constraints.weightx = 1;
   	constraints.weighty = 1;
   	constraints.gridwidth = 1;
   	constraints.gridheight = 1;
   	constraints.gridx = 1;
   	constraints.gridy = 1;
   	this.getContentPane().add(propice_host_label, constraints);
   	constraints.gridx = 2;
   	this.getContentPane().add(propice_host_text, constraints);
   	constraints.gridx = 3;
   	this.getContentPane().add(propice_manip_label, constraints);
   	constraints.gridx = 4;
   	this.getContentPane().add(propice_manip_text, constraints);
   	constraints.gridx = 5;
   	constraints.gridheight = 2;
   	this.getContentPane().add(connect_button, constraints);
      	constraints.gridheight = 1;
   	constraints.gridx = 1;
   	constraints.gridy = 2;
   	this.getContentPane().add(client_name_label, constraints);
   	constraints.gridx = 2;
   	this.getContentPane().add(client_name_text, constraints);
   	constraints.gridx = 3;
   	this.getContentPane().add(socket_mp_label, constraints);
    	constraints.gridx = 4;
   	this.getContentPane().add(socket_mp_text, constraints);
   	constraints.gridwidth = 2;
   	constraints.gridy = 3;
	constraints.gridx = 1;
   	this.getContentPane().add(msg_to_send_label, constraints);
   	constraints.gridwidth = 4;
   	constraints.gridy = 4;
   	this.getContentPane().add(msg_to_send_text, constraints);
   	constraints.gridwidth = 1;
   	constraints.gridx = 5;
   	this.getContentPane().add(send_button, constraints);
   	constraints.gridwidth = 2;
   	constraints.gridx = 1;
   	constraints.gridy = 5;
   	this.getContentPane().add(msg_received_label, constraints);
   	constraints.gridwidth = 5;
   	constraints.gridy = 6;
   	this.getContentPane().add(msg_received_textarea, constraints);
   	validate();
   	this.show();
    }

    /**
     * To manage the action of the buttons
     */
    public void actionPerformed(ActionEvent e) {
   	if (e.getSource() == connect_button) {
   	    OPRS_HOST = propice_host_text.getText();
   	    OPRS_MANIP = propice_manip_text.getText();
   	    CLIENT_NAME = client_name_text.getText();
   	    SOCKET_MP = Integer.parseInt(socket_mp_text.getText());
		
	    if (initConnection() == OK) {
		msg_to_send_text.setEnabled(true);
		send_button.setEnabled(true);
	    } else {
		msg_received_textarea.append("Problem connecting to the message passer\n");
	    }
   	}
   	if (e.getSource() == send_button) {
	    if (sendMessageToSupervisor(msg_to_send_text.getText()) == ERROR) {
		msg_received_textarea.append("Problem wile sending the message to messagePasser\n");
	    }
   	}
    }

    /**
     * Prepare the client to send mesages to the Message Passer. Initialize a connection to the message passer
     */
    public int initConnection() {
	/* Connection */
	try {
	    /* Create the sockets */
	    System.out.print("Connecting socket to Message Passer...");
	    Socket socket = new Socket(OPRS_HOST, SOCKET_MP);
	    System.out.println(" done.");
	    out = socket.getOutputStream();
	    ostream = new PrintWriter(out);
	    InputStream in = socket.getInputStream();
	    InputStreamReader reader = new InputStreamReader(in);
	    istream = new BufferedReader(reader);
	} catch (IOException e) {
	    System.out.println(" FAILED.");
	    System.out.println("MessagePasserClient->initConnection :\nError :" + e + "\nCould not connect to Message Passer...");
	    return ERROR;
	}
	       
	/* Now that we are connected, we'll send the protocol and client name to the message passer */
	try {
	    System.out.print("Sending client infos to Message Passer...");
	    write_int(1); // The protocol, corresponding to STRINGS_PT
	    write_string(CLIENT_NAME);
	    System.out.println(" done.");
	} catch (Exception e) {
	    System.out.println(" FAILED");
	    System.out.println("MessagePasserClient->initConnection :\nError :" + e + "\nCould not write on the socket...");
	    return ERROR;
	}
	
	/* And to end this initialisation, we will launch a thread that will listen for messages */
	lmp = new ListenMessagePasser();
	lmp.start();
	System.out.println("Now listening the Message Passer on socket : " + SOCKET_MP);
	return OK;
    }

    /**
     * To write an int to MP 
     * @param i The integer to send to MP
     */
    public void write_int(int i) {
	byte buffer[] = new byte[4]; // 4 is the length of an int in Java
	buffer[3] = (byte) ((i) & 0xff);
	buffer[2] = (byte) ((i >> 8) & 0xff);
	buffer[1] = (byte) ((i >> 16) & 0xff);
	buffer[0] = (byte) ((i >> 24) & 0xff);
	try {
	    out.write(buffer);
	    if (debug) System.out.println("Int sent to MP : " + buffer);
	} catch (IOException e) {
	    System.err.println("MessagePasserClient->write_int :\nException : " + e + "Couldn't write on the socket");
	}
    }

    /**
     * To send a string to MP 
     * @param s The string to send to MP
     */
    public void write_string(String s) {
	int length;
	length = s.length();
	byte buffer[] = new byte[4+length]; // 4 is the length of an int in Java
	buffer[3] = (byte) ((length) & 0xff);
	buffer[2] = (byte) ((length >> 8) & 0xff);
	buffer[1] = (byte) ((length >> 16) & 0xff);
	buffer[0] = (byte) ((length >> 24) & 0xff);
	int i = 0;
	while (i<length) {
	    buffer[i+4] = (byte)s.charAt(i);
	    i++;
	}
	//	buffer[i+4]='\0';
	try {
	    out.write(buffer);
	    if (debug) System.out.println("String sent to MP : " + buffer);
	} catch (IOException e) {
	    System.err.println("MessagePasserClient->write_string :\nException : " + e + "Couldn't write on the socket");
	}	
    }

    /**
     * Send a message to the supervisor. The message is encapsulated into the protocol defined by Message Passer.
     * @param msg The message to send
     */
    public int sendMessageToSupervisor(String msg) {
	/* The format of a message is :
	   MessageType+sizeof(DestinationName)+DestinationName+sizeof(msg)+msg*/
	int message_mt = 1; // MESSAGE_MT
	int lengthdest = OPRS_MANIP.length();
	int lengthmsg = msg.length();
	int size = 4 + 4 + lengthdest + 4 + lengthmsg;
	byte buffer[] = new byte[size];
	// Write the message byte per byte
	// Message type
	buffer[0] = (byte) ((message_mt >> 24) & 0xff);
	buffer[1] = (byte) ((message_mt >> 16) & 0xff);
	buffer[2] = (byte) ((message_mt >> 8) & 0xff);
	buffer[3] = (byte) ((message_mt) & 0xff);
	// Length of the recipient's name
	buffer[4] = (byte) ((lengthdest >> 24) & 0xff);
	buffer[5] = (byte) ((lengthdest >> 16) & 0xff);
	buffer[6] = (byte) ((lengthdest >> 8) & 0xff);
	buffer[7] = (byte) ((lengthdest) & 0xff);
	// The recipient's name
	int i = 0;
	while (i<lengthdest) {
	    buffer[i+8] = (byte)OPRS_MANIP.charAt(i);
	    i++;
	}
	// The length of the message
	buffer[8+i] = (byte) ((lengthmsg >> 24) & 0xff);
	buffer[9+i] = (byte) ((lengthmsg >> 16) & 0xff);
	buffer[10+i] = (byte) ((lengthmsg >> 8) & 0xff);
	buffer[11+i] = (byte) ((lengthmsg) & 0xff);
	// The message
	int j=0;
	while (j<lengthmsg) {
	    buffer[12+i+j] = (byte) msg.charAt(j);
	    j++;
	}
	
	// Send the message
	try {
	    out.write(buffer);
	    if (debug) System.out.println("Message sent to supervisor : " + buffer);
	} catch (IOException e) {
	    System.err.println("MessagePasserClient->sendMessageToSupervisor :\nException : " + e + "Couldn't write on the socket");
	    return ERROR;
	}	
	return OK;
    }

    /**
     * The class that will be listening on the socket 
     */
    public class ListenMessagePasser extends Thread {
	public ListenMessagePasser () {
	
	}
	
	/**
	 * The code executed when the thread is started.
	 */
	public void run() {
	    try {
		// At the very beginning, we receive four 0, so we get rid of them
		// They correspond to the REGISTER_OK
		istream.read();istream.read();istream.read();istream.read();
		// In fact, we should check that we are getting REGISTER_OK, otherwise something went wrong.

		/* Then we are ready to receive a message
		 * As we are in STRINGS_PT mode, a message will look like :
		 * three zeros, an int giving the length of the string for the sender, the sender's name (ascii),
		 * three more zeros, an int for the length of the message, and the message itself (ascii).
		 * We'll skip the zeros, consider the first number and send the following string to the parser
		 */

		// This is partly wrong... it is not 3 zeros + an int
		// (in one byte) but an int on 4 bytes. So if your int
		// is less than 2**8, fine, otherwise, you need
		// something smarter (i.e. read the 4 bytes and make an int of it).

		while (true) {
		    int ch = istream.read();
		    if (ch == 0) continue;
		    else { // The first number after a serie of zeros is the size of the upcoming string
			char[] chaine = new char[ch];
			istream.read(chaine, 0, ch);
			messageParser(new String(chaine));
		    }
		}
	    } catch (IOException e) {
		System.out.println("Error :" + e + "\nError while listening on the socket...");
	    }
	}
    }

    /** 
     * To analyse what is received, we need a parser. 
     * The message parser will receive alternatively the two strings... 
     * We will differentiate them with a comparison between the name of the sender and the string received.
     * Once the message is parsed, we display it in the textarea
     * @param s The next string
     */
    public void messageParser(String s) {
	if (!s.equalsIgnoreCase(OPRS_MANIP)) { // Then this is the message that is coming
	    Calendar cal = Calendar.getInstance();
	    msg_received_textarea.append(cal.get(Calendar.HOUR_OF_DAY)+":"+cal.get(Calendar.MINUTE)+":"+cal.get(Calendar.SECOND)+" from "+sender+"> "+s+"\n");		
	    if (debug) System.out.println(s+"\n=>"+s);
	}
	else {	// here we receive the name of the sender.
	    sender = s;
	}
    } 

    public static void main(String[] args) {
   	new MPClient();
    }
}
