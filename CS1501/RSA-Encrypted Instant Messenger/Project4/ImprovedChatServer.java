//for running with original client

/* CS 1501 Summer 2013
   Simple Chat server (without encryption).   The actual server used for
   Assignment 4 is similar to this, but with handshaking and encryption
   as specified in the Assignment sheet.  This server (together with
   the client) will run as is. Run it to see how the messages appear and
   how clients connect and disconnect.
*/
import java.util.*;
import java.io.*;
import java.net.*;

public class ImprovedChatServer 
{

	public static final int PORT = 5678;

	private int MaxUsers;
	private Socket [] users;        // Need array of Sockets and Threads,
	private UserThread [] threads;  // one of each per client -- using an
	private int numUsers;           // ArrayList for these would make less work
									// for the programmer

	public ImprovedChatServer(int MaxU)
	{
		MaxUsers = MaxU;
		users = new Socket[MaxUsers];
		threads = new UserThread[MaxUsers];   // Set things up and start
		numUsers = 0;                         // Server running

		try
		{
			runServer();
		}
		catch (Exception e)
		{
			System.out.println("Problem with server");
		}
	}

	public synchronized void SendMsg(String msg)  // Send current message to
		// all clients (even sender).
		// This must be synchronized so that chatters do not
		// "interrupt" each other
	{
		for (int i = 0; i < numUsers; i++)
		{
			PrintWriter currWriter = threads[i].getWriter();
			currWriter.println(msg);
		}
	}

	public synchronized void removeClient(int id, String name)
	{
		try                          // Remove a client from the server.  This
		{                            // also must be synchronized, since we
			users[id].close();       // could have an inconsistent state if this
		}                            // is interrupted in the middle
		catch (IOException e)
		{
			System.out.println("Already closed");
		}
		users[id] = null;
		threads[id] = null;
		for (int i = id; i < numUsers-1; i++)   // Shift remaining clients in
		{                                       // array up one position
			users[i] = users[i+1];
			threads[i] = threads[i+1];
			threads[i].setId(i);
		}
		numUsers--;
		SendMsg(name + " has logged off");    // Announce departure
	}

	private void runServer() throws IOException
	{
		ServerSocket s = new ServerSocket(PORT);
		System.out.println("Started: " + s);

		try 
		{
			while(true) 
			{

				if (numUsers < MaxUsers)
				{
					try 
					{

						Socket newSocket = s.accept();    // get next client
						synchronized (this)
						{
							users[numUsers] = newSocket;
							BufferedReader tempReader = new BufferedReader(
								new InputStreamReader(newSocket.getInputStream()));
							String newName = tempReader.readLine();
							SendMsg(newName + " has just joined the chat group");
							// Above the server gets the new chatter's and announces
							// to the rest of the group

							threads[numUsers] = new UserThread(newSocket, numUsers,
								newName, tempReader);
							threads[numUsers].start();
							// Above a new thread is created and started for the
							// new user

							System.out.println("Connection " + numUsers + users[numUsers]);
							numUsers++;
						}
					}
					catch (Exception e)
					{
						System.out.println("Problem with connection...terminating");
					}
				}  // if

			}  // while
		}   // try
		finally 
		{
			System.out.println("Server shutting down");
		}
	}

	// Below is the class used by the server to keep track of the clients.  Each
	// client is a new UserThread object, with the data shown.

	private class UserThread extends Thread
	{
		private Socket mySocket;
		private BufferedReader myReader;
		private PrintWriter myWriter;
		private int myId;
		private String myName;

		private UserThread(Socket newSocket, int id, String newName,
			BufferedReader newReader) throws IOException
		{
			mySocket = newSocket;
			myId = id;
			myName = newName;
			myReader = newReader;
			myWriter =
				new PrintWriter(
				new BufferedWriter(
				new OutputStreamWriter(mySocket.getOutputStream())), true);
		}

		public BufferedReader getReader()
		{
			return myReader;
		}

		public PrintWriter getWriter()
		{
			return myWriter;
		}

		public synchronized void setId(int newId)
		{
			myId = newId;   // id may change when a previous chatter quits
		}

		// While running, each UserThread will get the next message from its
		// corresponding client, and then send it to the other clients (through
		// the Server).  A departing client is detected by an IOException in
		// trying to read, which causes the removeClient method to be executed.

		public void run()
		{
			boolean ok = true;
			while (ok)
			{
				String newMsg = null;
				try 
				{
					newMsg = myReader.readLine();
					// We now also check for an explicit closing message from
					// the client.  Since "normal messages" have the client's
					// name followed by a colon, the closing message could not
					// be sent by mistake.
					if (newMsg == null || newMsg.equals("CLIENT CLOSING"))
						ok = false;
					else
						ImprovedChatServer.this.SendMsg(newMsg);
				}
				catch (IOException e)
				{
					System.out.println("Client closing!!");
					ok = false;
				}
			}
			removeClient(myId, myName);
		}
	}

	public static void main(String [] args)
	{
		ImprovedChatServer JR = new ImprovedChatServer(20);
	}
}


